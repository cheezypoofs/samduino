#ifndef ArduinoTestState_h
#define ArduinoTestState_h

/**
 * ArduinoTestState exposes helpers to setup and teardown
 * the global arduino emulation state.
 */

#include <chrono>
#include <mutex>
#include <unordered_map>

#include "Arduino.h"

/**
 * TimeProvider describes a test implementation of the time
 * functions.
 */
class TimeProvider
{
public:
    virtual ~TimeProvider() = default;

    virtual unsigned long Millis() = 0;
    virtual unsigned long Micros() = 0;
    virtual void Delay( unsigned long ) = 0;
    virtual void DelayMicroseconds( unsigned long ) = 0;
};

/**
 * MonotonicTimeProvider implements TimeProvider using the c++11
 * monotonic clock.
 */
class MonotonicTimeProvider : public TimeProvider
{
public:
    MonotonicTimeProvider();

    unsigned long Millis() override;
    unsigned long Micros() override;
    void Delay( unsigned long ) override;
    void DelayMicroseconds( unsigned long ) override;

private:
    std::chrono::steady_clock m_clock;
    std::chrono::steady_clock::time_point m_start;
};

/**
 * InputOutputProvider describes a test implementation of the i/o
 * functions in the arduino.
 */
class InputOutputProvider
{
public:
    virtual ~InputOutputProvider() = default;

    virtual void PinMode( uint8_t pin, uint8_t mode ) = 0;
    virtual void DigitalWrite( uint8_t pin, uint8_t val ) = 0;
    virtual int DigitalRead( uint8_t pin ) = 0;
};

/**
 * InMemoryInputOutputProvider is an implemenrtation of InputOutputProvider
 * that allows for direct manipulation and checking of i/o state during testing.
 */
class InMemoryInputOutputProvider : public InputOutputProvider
{
public:
    struct PinState
    {
        uint8_t number;
        uint8_t value;
        uint8_t mode;

        PinState() = default;
        PinState( uint8_t number, uint8_t mode )
            : number( number )
            , value( 0xFF )
            , mode( mode )
        {}
    };

    // ReadState returns a copy of the current state
    PinState ReadState( uint8_t number );

    // WriteState allows you to atomically modify the internal state
    // of any pin.
    void WriteState( PinState );

    virtual void PinMode( uint8_t pin, uint8_t mode ) override;
    virtual void DigitalWrite( uint8_t pin, uint8_t val ) override;
    virtual int DigitalRead( uint8_t pin ) override;

private:
    std::mutex m_lock;
    std::unordered_map< uint8_t, PinState > m_pins;
};

/**
 * An ArduinoTestState should be created per test case to setup and teardown
 * the global arduino functions available for testing.
 * No providers are setup by default, and each needed provider must be
 * set explicitly.
 */
class ArduinoTestState
{
public:
    ArduinoTestState();
    ArduinoTestState( const ArduinoTestState& ) = delete;
    virtual ~ArduinoTestState();

    ArduinoTestState& SetTimeProvider( TimeProvider* time )
    {
        m_time = time;
        return *this;
    }

    TimeProvider& GetTimeProvider() const;

    ArduinoTestState& SetInputOutputProvider( InputOutputProvider* io )
    {
        m_io = io;
        return *this;
    }

    InputOutputProvider& GetInputOutputProvider() const;

private:
    TimeProvider* m_time;
    InputOutputProvider* m_io;
};

#endif
