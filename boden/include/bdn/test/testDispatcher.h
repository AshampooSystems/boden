#ifndef BDN_TEST_testDispatcher_H_
#define BDN_TEST_testDispatcher_H_

#include <bdn/IDispatcher.h>


namespace bdn
{
namespace test
{

struct TestDispatcherData_ : public Base
{
    std::vector<int> callOrder;

    std::vector< std::chrono::steady_clock::time_point > callTimes;
};


/** Tests an IDispatcher implementation. The dispatcher must execute the enqueued
    items automatically. It may do so in the same thread or a separate thread. */
inline void testDispatcher(IDispatcher* pDispatcher)
{
    SECTION("single call")
    {
        IDispatcher::Priority prio;

        SECTION("Priority normal")
            prio = IDispatcher::Priority::normal;
        SECTION("Priority idle")
            prio = IDispatcher::Priority::idle;

        P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

        pDispatcher->enqueue(
            [pData]()
            {                
                pData->callOrder.push_back(0);
            },
            prio);

        CONTINUE_SECTION_AFTER_SECONDS(1, pData)
        {
            REQUIRE( pData->callOrder.size()==1 );
        };
    }

    SECTION("multiple calls")
    {
        SECTION("same priority")
        {
            IDispatcher::Priority prio;

            SECTION("Priority normal")
                prio = IDispatcher::Priority::normal;
            SECTION("Priority idle")
                prio = IDispatcher::Priority::idle;

            P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

            // there is some potential for race conditions here if the dispatcher
            // runs in a separate thread. Since we cannot block it, it can start
            // executing the first item immediately, before we can enqueue the
            // second item. Which means that priority might not factor into the
            // execution order.
            // To work around this, we first enqueue an item that takes a certain
            // time to execute. That ensures that if the dispatcher runs on a separate
            // thread then we will have time to enqueue all our real items before
            // it starts executing any of them.
            pDispatcher->enqueue(
                []()
                {
                    Thread::sleepSeconds(1);
                } );

            pDispatcher->enqueue(
                [pData]()
                {
                    pData->callOrder.push_back(0);
                },
                prio);

            pDispatcher->enqueue(
                [pData]()
                {
                    pData->callOrder.push_back(1);
                },
                prio);

            CONTINUE_SECTION_AFTER_SECONDS(2, pData)
            {
                REQUIRE( pData->callOrder.size()==2);
            
                // should have been called in the same order in which they were enqueued.
                REQUIRE( pData->callOrder[0] == 0);
                REQUIRE( pData->callOrder[1] == 1);
            };
        }

        SECTION("normal, then idle")
        {
            P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

            // block execution to avoid race conditions. See above.
            pDispatcher->enqueue( [](){ Thread::sleepSeconds(1); } );

            pDispatcher->enqueue(
                [pData]()
                {
                    pData->callOrder.push_back(0);
                },
                IDispatcher::Priority::normal );

            pDispatcher->enqueue(
                [pData]()
                {
                    pData->callOrder.push_back(1);
                },
                IDispatcher::Priority::idle );

            CONTINUE_SECTION_AFTER_SECONDS(1.5, pData)
            {
                REQUIRE( pData->callOrder.size()==2);
            
                // should have been called in the same order in which they were enqueued.
                REQUIRE( pData->callOrder[0] == 0);
                REQUIRE( pData->callOrder[1] == 1);
            };
        }

        SECTION("idle, then normal")
        {
            P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

            // block execution to avoid race conditions. See above.
            pDispatcher->enqueue( [](){ Thread::sleepSeconds(1); } );

            pDispatcher->enqueue(
                [pData]()
                {
                    pData->callOrder.push_back(0);
                },
                IDispatcher::Priority::idle );

            pDispatcher->enqueue(
                [pData]()
                {
                    pData->callOrder.push_back(1);
                },
                IDispatcher::Priority::normal );

            CONTINUE_SECTION_AFTER_SECONDS(1.5, pData)
            {
                REQUIRE( pData->callOrder.size()==2);
            
                // should have been called in reverse order
                REQUIRE( pData->callOrder[0] == 1);
                REQUIRE( pData->callOrder[1] == 0);
            };
        }
    }

    SECTION("timed")
    {
        IDispatcher::Priority prio;

        SECTION("Priority normal")
            prio = IDispatcher::Priority::normal;
        SECTION("Priority idle")
            prio = IDispatcher::Priority::idle;

        P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

        pDispatcher->enqueueInSeconds(
            2,
            [pData]()
            {                
                pData->callOrder.push_back(0);
            },
            prio);

        CONTINUE_SECTION_AFTER_SECONDS(0.5, pData)
        {
            // should NOT have been called immediately
            REQUIRE( pData->callOrder.size()==0 );

            CONTINUE_SECTION_AFTER_SECONDS(0.5, pData)
            {
                // also not after 1 second
                REQUIRE( pData->callOrder.size()==0 );

                CONTINUE_SECTION_AFTER_SECONDS(2, pData)
                {
                    // but after another 2 seconds it should have been called
                    REQUIRE( pData->callOrder.size()==1 );
                    REQUIRE( pData->callOrder[0]==0 );
                };
            };            
        };
    }

    SECTION("timer")
    {
        P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

        pDispatcher->createTimer(
            0.1,
            [pData]() -> bool
            {
                pData->callTimes.push_back( std::chrono::steady_clock::now() );

                // sleep a little so that we can verify that subsequent call times
                // are scheduled based on the scheduled time, not the finish time of
                // the handler.
                Thread::sleepSeconds( 0.05 );

                return (pData->callTimes.size()<20);
            } );

        CONTINUE_SECTION_AFTER_SECONDS(2.5, pData, startTime)
        {
            // our timer is expected to be called once every 100 ms.            
            // Verify that all expected timer calls have happened.
            // This also verifies that the timer has stopped once
            // we returned false.
            REQUIRE( pData->callTimes.size()==20 );

            for(size_t i=0; i<pData->callTimes.size(); i++)
            {
                std::chrono::steady_clock::time_point callTime = pData->callTimes[i];

                double timeAfterStart = std::chrono::duration_cast<std::chrono::microseconds>(callTime-startTime).count() / 1000000.0;

                double expectedTimeAfterStart = (i+1)*0.1;

                // should not have been called before the expected time
                REQUIRE( timeAfterStart>=expectedTimeAfterStart);

                // and not too much after the expected time. Note that we do allow for small
                // hiccups and differences, but the actual time must never be too far behind
                // the expected time.
                REQUIRE( timeAfterStart < expectedTimeAfterStart + 0.2);
            }
        };
    }
}


}
}

#endif

