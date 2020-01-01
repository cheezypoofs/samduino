#include <samduino.h>

/**
 * Read the clock via `millis()` and sets the
 * whole-seconds value on a 4-digit 7-segment LED.
 */

namespace
{

// How I wired up the 5641AS
struct MyLayout
{
  uint8_t dPins[4];
  uint8_t dBits[4];
  samduino::SevenSegmentState layout;

  MyLayout()
  {
    layout.NumD = 4;
    layout.DBits = dBits;
    layout.DPins = dPins;

    dPins[0] = 5;
    dPins[1] = 4;
    dPins[2] = 3;
    dPins[3] = 2;

    layout.PinA = 11;
    layout.PinB = 13;
    layout.PinC = 6;
    layout.PinD = 10;
    layout.PinE = 9;
    layout.PinF = 12;
    layout.PinG = 7;
    layout.PinDot = 8;
  }
};

// A ScheduledWork that reads the system clock and
// sets the digits in the display.
class ClockWork : public samduino::ScheduledWork
{
public:
  explicit ClockWork( samduino::SevenSegmentState& layout )
    : m_layout( layout )
    , m_nextDueAt( 0 )
  {}

  unsigned long DueAtMillis() override { return m_nextDueAt; }

  void DoWork() override
  {
    unsigned long now = millis();
    m_nextDueAt = now + 100;

    // Display as like 100.7 (seconds) when 1007xx milliseconds have elapsed.
    m_layout.DBits[0] = samduino::SevenSegment::MakeBits( static_cast< uint8_t >( ( now / 100000 ) % 10 ) );
    m_layout.DBits[1] = samduino::SevenSegment::MakeBits( static_cast< uint8_t >( ( now / 10000 ) % 10 ) );
    m_layout.DBits[2] = samduino::SevenSegment::MakeBits( static_cast< uint8_t >( ( now / 1000 ) % 10 ), samduino::Dotted::kWithDot );
    m_layout.DBits[3] = samduino::SevenSegment::MakeBits( static_cast< uint8_t >( ( now / 100 ) % 10 ) );
  }

private:
  samduino::SevenSegmentState& m_layout;
  unsigned long m_nextDueAt;
};

// My globals initialized in setup()
MyLayout* gLayout;
samduino::SevenSegment* gSeven;
samduino::Scheduler* gScheduler;
samduino::ScheduledWork* gDisplayWork;
samduino::ScheduledWork* gClockWork;

}

void setup() {
  gLayout = new MyLayout();
  gSeven = new samduino::SevenSegment( gLayout->layout );

  samduino::SchedulerConfig config;
  gScheduler = new samduino::Scheduler( config );

  gDisplayWork = new samduino::SevenSegmentDisplayWork( *gSeven );
  gClockWork = new ClockWork( gLayout->layout );

  // Keep the display refreshed as scheduled work
  gScheduler->AddWork( *gDisplayWork );

  // Also read the clock regularly and update our state.
  gScheduler->AddWork( *gClockWork );
}

void loop() {
  gScheduler->Loop();
}