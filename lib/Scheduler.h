#ifndef Samduino_Scheduler_h
#define Samduino_Scheduler_h

#ifdef __cplusplus

#include <stdint.h>

namespace samduino
{

class ScheduledWork
{
public:
    ScheduledWork() = default;
    ScheduledWork( const ScheduledWork& ) = delete;
    virtual ~ScheduledWork() = default;

    virtual unsigned long DueAtMillis() = 0;
    virtual void DoWork() = 0;
};

struct SchedulerConfig
{
    unsigned long MaxSleepMs;
};

class Scheduler final
{
public:
    explicit Scheduler( SchedulerConfig );
    ~Scheduler();

    Scheduler( const Scheduler& ) = delete;

    void AddWork( ScheduledWork& );

    void Loop();

    void Stop();

private:
    const SchedulerConfig m_config;
    volatile uint8_t m_stopped;
    uint8_t m_numWorks;
    ScheduledWork** m_works;
};

}

#endif // c++

#endif
