#ifndef ArduinoTestState_h
#define ArduinoTestState_h

#include <chrono>
#include <mutex>
#include <unordered_map>

#include "Arduino.h"

class TimeProvider
{
public:
    virtual ~TimeProvider() = default;

    virtual unsigned long Millis() = 0;
    virtual unsigned long Micros() = 0;
    virtual void Delay( unsigned long ) = 0;
    virtual void DelayMicroseconds( unsigned long ) = 0;
};

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

class InputOutputProvider
{
public:
    virtual ~InputOutputProvider() = default;

    virtual void PinMode( uint8_t pin, uint8_t mode ) = 0;
    virtual void DigitalWrite( uint8_t pin, uint8_t val ) = 0;
    virtual int DigitalRead( uint8_t pin ) = 0;
};

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

class ArduinoTestState
{
public:
    ArduinoTestState();
    virtual ~ArduinoTestState();

    void SetTimeProvider( TimeProvider* time ) { m_time = time; }
    TimeProvider& GetTimeProvider() const;

    void SetInputOutputProvider( InputOutputProvider* io ) { m_io = io; }
    InputOutputProvider& GetInputOutputProvider() const;

private:
    TimeProvider* m_time;
    InputOutputProvider* m_io;
};

#endif
