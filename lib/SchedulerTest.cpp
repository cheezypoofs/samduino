#include <atomic>
#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <thread>

#include "Scheduler.h"
#include "ArduinoTestState.h"

using namespace samduino;

namespace
{

class SchedulerTest : public ::testing::Test
{
public:
    SchedulerTest()
    {
        m_state.SetTimeProvider( &m_time );
    }

    virtual ~SchedulerTest()
    {
        if ( m_scheduler )
        {
            m_scheduler->Stop();
            m_thread->join();
        }
    }

protected:

    void StartScheduler( std::unique_ptr< Scheduler >&& scheduler )
    {
        m_scheduler = std::move( scheduler );

        m_thread.reset( new std::thread( [&]() {
            m_scheduler->Loop();
        }));
    }

private:
    MonotonicTimeProvider m_time;
    ArduinoTestState m_state;
    std::unique_ptr< std::thread > m_thread;
    std::unique_ptr< Scheduler > m_scheduler;
};

class CountingWorkItem : public ScheduledWork
{
public:
    CountingWorkItem()
        : m_count( 0 )
    {}

    size_t GetCount() const { return m_count; }

    void DoWork() override
    {
        m_count++;
    }

    unsigned long DueAtMillis() override
    {
        // Always due
        return 0;
    }

private:
    std::atomic< size_t > m_count;
};

}

TEST_F( SchedulerTest, TestScheduling )
{
    SchedulerConfig config;
    config.MaxSleepMs = 10;

    CountingWorkItem counter;

    {
        std::unique_ptr< Scheduler > scheduler( new Scheduler( config ) );
        scheduler->AddWork( counter );

        // Kick it in a new managed thread
        StartScheduler( std::move( scheduler ) );
    }

    // Let the work items run
    std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

    // We should have seen a few executions
    EXPECT_NE( counter.GetCount(), 0 );
}
