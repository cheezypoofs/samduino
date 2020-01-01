#ifndef Samduino_Scheduler_h
#define Samduino_Scheduler_h

/**
 * The Scheduler provides a pretty simple scheduling loop which
 * can be used inside the arduino loop() function to handle
 * all of your recurring work items that need to run on
 * potentially different intervals.
 */

#ifdef __cplusplus

#include <stdint.h>

namespace samduino
{

/**
 * ScheduledWork describes a single recurring work item in the
 * Scheduler.
 */
class ScheduledWork
{
public:
    ScheduledWork() = default;
    ScheduledWork( const ScheduledWork& ) = delete;
    virtual ~ScheduledWork() = default;

    // DueAtMillis returns the time (from millis()) this work's
    // DoWork() should be called.
    // Return 0 to indicate it is ready now and avoid the millis() check.
    virtual unsigned long DueAtMillis() = 0;

    // DoWork executes your work item.
    virtual void DoWork() = 0;
};

struct SchedulerConfig
{
    unsigned long MaxSleepMs;
};

/**
 * Scheduler takes all of your ScheduledWork items and runs the Loop() to
 * execute anything ready and then delay() the appropriate amount of time
 * until something else is ready.
 *
 * This class is not thread-safe and assumes that in production it will be
 * created and held forever (or until shutdown).
 *
 * Typically, the ScheduledWork instances will be created in setup() and then
 * executed in loop() (by way of .Loop() here).
 */
class Scheduler
{
public:
    explicit Scheduler( SchedulerConfig );
    ~Scheduler();

    Scheduler( const Scheduler& ) = delete;

    // AddWork adds another of your work items to the list. It may only be
    // called prior to starting the Loop().
    void AddWork( ScheduledWork& );

    // Loop handles the logic of looping through all work items and
    // delay()'ing as needed between times when nothing is ready to execute.
    // Loop() will run forever or until Stop() (which is only for testing)
    // is called.
    void Loop();

    // Stop is provided as a way to unblock Loop() and it only makes sense to run
    // in testing. One could call it from a work item to unblock the loop
    // though a use case for that is not obvious at this time.
    void Stop();

private:
    const SchedulerConfig m_config;
    volatile uint8_t m_stopped;
    uint8_t m_numWorks;
    ScheduledWork** m_works;
};

} // samduino

#endif // c++

#endif
