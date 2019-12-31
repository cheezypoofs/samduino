#ifndef Samduino_SevenSegment_h
#define Samduino_SevenSegment_h

#include "Arduino.h"
#include "Scheduler.h"

#ifdef __cplusplus

namespace samduino
{

#define SEVEN_SEGMENT_BIT_A_MASK 0x80
#define SEVEN_SEGMENT_BIT_B_MASK 0x40
#define SEVEN_SEGMENT_BIT_C_MASK 0x20
#define SEVEN_SEGMENT_BIT_D_MASK 0x10
#define SEVEN_SEGMENT_BIT_E_MASK 0x08
#define SEVEN_SEGMENT_BIT_F_MASK 0x04
#define SEVEN_SEGMENT_BIT_G_MASK 0x02
#define SEVEN_SEGMENT_BIT_DOT_MASK 0x01

struct SevenSegmentState
{
    uint8_t PinA;
    uint8_t PinB;
    uint8_t PinC;
    uint8_t PinD;
    uint8_t PinE;
    uint8_t PinF;
    uint8_t PinG;
    uint8_t PinDot;

    uint8_t NumD;
    uint8_t* DPins;
    uint8_t* DBits;

    SevenSegmentState()
        : NumD( 0 )
        , DPins( 0 )
        , DBits( 0 )
    {}
};

enum class Dotted : uint8_t
{
    kWithoutDot = 0,
    kWithDot = 1
};

class SevenSegment
{
public:
    explicit SevenSegment( SevenSegmentState& );

    void Display( uint8_t which );
    SevenSegmentState& State() { return m_state; }

    static uint8_t MakeBits( uint8_t value, Dotted dotted = Dotted::kWithoutDot );

    void SetError();

private:
    SevenSegmentState& m_state;
    ScheduledWork* m_displayWork;
};

class SevenSegmentDisplayWork : public ScheduledWork
{
public:
    explicit SevenSegmentDisplayWork( SevenSegment& );
    unsigned long DueAtMillis() override;
    void DoWork() override;

private:
    SevenSegment& m_7;
    unsigned long m_nextRun;
    uint8_t m_which;
};

} // samduino

#endif // c++

#endif
