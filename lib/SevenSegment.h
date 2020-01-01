#ifndef Samduino_SevenSegment_h
#define Samduino_SevenSegment_h

/**
 * The 7-segment classes here are meant to be used
 * with various 7-segment LED devices including
 * those with multiple digits like the 5641AS.
 *
 * The naming convention used here is for a layout
 * like:
 *          a
 *        -----
 *      f|     |b
 *       |  g  |
 *        -----
 *      e|     |c
 *       |     |
 *        -----
 *          d    * (dot)
 *
 * All values are set and stored as a single uint8_t mapped
 * acording to the masks described below. So, a zero where
 * a, b, c, d, e, and f are set would be 0xFC.
 */

#include "Arduino.h"
#include "Scheduler.h"

#define SEVEN_SEGMENT_BIT_A_MASK 0x80
#define SEVEN_SEGMENT_BIT_B_MASK 0x40
#define SEVEN_SEGMENT_BIT_C_MASK 0x20
#define SEVEN_SEGMENT_BIT_D_MASK 0x10
#define SEVEN_SEGMENT_BIT_E_MASK 0x08
#define SEVEN_SEGMENT_BIT_F_MASK 0x04
#define SEVEN_SEGMENT_BIT_G_MASK 0x02
#define SEVEN_SEGMENT_BIT_DOT_MASK 0x01

#ifdef __cplusplus

namespace samduino
{

/**
 * A configuration and state structure that is setup to
 * describe how you have wired your device.
 * PinA to PinDot are required.
 *
 * For single-digit devices, NumD should still be set to `1`
 * and DBits should be an array of one. DPins can be set to
 * `nullptr` when there are no digit selection pins.
 */
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

    // Describes the number of digits
    uint8_t NumD;
    // If selection pins are in use, this should be set
    // to a NumD-sized array.
    uint8_t* DPins;
    // Should be set to a NumD-sized array of places
    // to hold the current value as a bitmap.
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

/**
 * SevenSegment is a helper that manipulates your SevenSegmentState
 * to display values on your device.
 */
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
};

/**
 * SevenSegmentDisplayWork implements the ScheduledWork to
 * handle the necessary recurring work needed to properly display
 * values on a multi-digit 7-segment LED display. It schedules
 * the repeated selection of which digit to display and cycles
 * fast enough for the eye to think all digits are on.
 */
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
