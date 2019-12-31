#include <string.h>

#include "Arduino.h"
#include "Scheduler.h"

namespace samduino
{

Scheduler::Scheduler( SchedulerConfig config )
    : m_config( config )
    , m_stopped( 0 )
    , m_numWorks( 0 )
    , m_works( nullptr )
{}

Scheduler::~Scheduler()
{
    if ( m_works )
    {
        delete[] m_works;
    }
}

void Scheduler::AddWork( ScheduledWork& work )
{
    ScheduledWork** works = new ScheduledWork*[m_numWorks + 1];

    if ( m_works )
    {
        ::memcpy( works, m_works, sizeof( ScheduledWork* ) * m_numWorks );
    }
    works[m_numWorks] = &work;

    m_works = works;
    m_numWorks++;
}

void Scheduler::Stop()
{
    m_stopped = 1;
}

void Scheduler::Loop()
{
    while ( !m_stopped )
    {
        // Don't sleep longer than this
        unsigned long wakeUpBy = millis() + m_config.MaxSleepMs;

        // todo: it might be nice to bookmark our last index and
        // start there instead of always starting at work item 0.

        // Run through each of the work items seeing if anyone is ready.
        for ( uint8_t i = 0; i < m_numWorks; i++ )
        {
            ScheduledWork& work = *m_works[i];

            unsigned long now = millis();
            unsigned long due = work.DueAtMillis();

            // Setup our total delay calculated for this loop
            if ( now < due )
            {
                if ( due < wakeUpBy )
                {
                    wakeUpBy = due;
                }

                continue;
            }

            // This item is ready. Call it.
            work.DoWork();
        }

        // Now, only sleep for up to wakeUpBy if it still applies
        unsigned long now = millis();
        if ( now < wakeUpBy )
        {
            delay( wakeUpBy - now );
        }
    }
}

} // samduino
