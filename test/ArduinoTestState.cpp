#include "ArduinoTestState.h"

#include <assert.h>
#include <cstdint>
#include <string>
#include <thread>

namespace
{

ArduinoTestState* g_state;

ArduinoTestState& AssertState()
{
    if ( g_state == nullptr )
    {
        throw std::logic_error( "Arduino test state not configured" );
    }

    return *g_state;
}

}

////////////

MonotonicTimeProvider::MonotonicTimeProvider()
    : m_start( m_clock.now() )
{
}

unsigned long MonotonicTimeProvider::Millis()
{
    return std::chrono::duration_cast< std::chrono::milliseconds >(
        m_clock.now() - m_start ).count();
}

unsigned long MonotonicTimeProvider::Micros()
{
    return static_cast< unsigned long >(
        std::chrono::duration_cast< std::chrono::microseconds >(
            m_clock.now() - m_start ).count() );
}

void MonotonicTimeProvider::Delay( unsigned long ms )
{
    std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
}

void MonotonicTimeProvider::DelayMicroseconds( unsigned long us )
{
    std::this_thread::sleep_for( std::chrono::microseconds( us ) );
}

////////////

InMemoryInputOutputProvider::PinState InMemoryInputOutputProvider::ReadState( uint8_t number )
{
    std::lock_guard< std::mutex > lock( m_lock );

    auto it = m_pins.find( number );
    if ( it == m_pins.end() )
    {
        throw std::logic_error( "Illegal pin " + std::to_string( number ) + " specified in test" );
    }

    return it->second;
}

void InMemoryInputOutputProvider::WriteState( PinState state )
{
    std::lock_guard< std::mutex > lock( m_lock );
    m_pins[ state.number ] = state;
}

void InMemoryInputOutputProvider::PinMode( uint8_t pin, uint8_t mode )
{
    std::lock_guard< std::mutex > lock( m_lock );
    m_pins[ pin ] = PinState( pin, mode );
}

void InMemoryInputOutputProvider::DigitalWrite( uint8_t pin, uint8_t val )
{
    std::lock_guard< std::mutex > lock( m_lock );
    auto it = m_pins.find( pin );
    if ( it == m_pins.end() )
    {
        throw std::logic_error( "Illegal pin " + std::to_string( pin ) + " specified in test" );
    }

    PinState& state = it->second;
    if ( state.mode != OUTPUT )
    {
        throw std::logic_error( "Pin " + std::to_string( pin ) + " is not configured for output" );
    }

    state.value = val;
}

int InMemoryInputOutputProvider::DigitalRead( uint8_t pin )
{
    std::lock_guard< std::mutex > lock( m_lock );
    auto it = m_pins.find( pin );
    if ( it == m_pins.end() )
    {
        throw std::logic_error( "Illegal pin " + std::to_string( pin ) + " specified in test" );
    }

    PinState& state = it->second;
    if ( state.mode != INPUT )
    {
        throw std::logic_error( "Pin " + std::to_string( pin ) + " is not configured for input" );
    }

    return it->second.value;
}

////////////

ArduinoTestState::ArduinoTestState()
    : m_time( nullptr )
    , m_io( nullptr )
{
    assert( g_state == nullptr );
    g_state = this;
}

ArduinoTestState::~ArduinoTestState()
{
    assert( g_state != nullptr );
    g_state = nullptr;
}

TimeProvider& ArduinoTestState::GetTimeProvider() const
{
    if ( m_time == nullptr )
    {
        throw std::logic_error( "TimeProvider not configured for this test" );
    }

    return *m_time;
}

InputOutputProvider& ArduinoTestState::GetInputOutputProvider() const
{
    if ( m_io == nullptr )
    {
        throw std::logic_error( "InputOutputProvider not configured for this test" );
    }

    return *m_io;
}

////////////

extern "C"
{

void pinMode( uint8_t pin, uint8_t mode )
{
    return AssertState().GetInputOutputProvider().PinMode( pin, mode );
}

void digitalWrite( uint8_t pin, uint8_t val )
{
    return AssertState().GetInputOutputProvider().DigitalWrite( pin, val );
}

int digitalRead( uint8_t pin )
{
    return AssertState().GetInputOutputProvider().DigitalRead( pin );
}

unsigned long millis()
{
    return AssertState().GetTimeProvider().Millis();
}

unsigned long micros()
{
    return AssertState().GetTimeProvider().Micros();
}

void delay( unsigned long ms )
{
    return AssertState().GetTimeProvider().Delay( ms );
}

void delayMicroseconds( unsigned long us )
{
    return AssertState().GetTimeProvider().DelayMicroseconds( us );
}

}
