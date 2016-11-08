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

    int callableDestroyedCount = 0;
};

template<class ResultType>
class TestDispatcherCallable_
{
public:
    TestDispatcherCallable_( std::function<ResultType()> func, Thread::Id expectedThreadId, TestDispatcherData_* pData )
    {
        _func = func;
        _expectedThreadId = expectedThreadId;
    }

    ~TestDispatcherCallable_()
    {
        if(!_ignoreThisInstance)
        {
            // destructor MUST also be called from the expected thread
            REQUIRE( Thread::getId() == _expectedThreadId );

            pData->callableDestroyedCount++;
        }
    }

    ResultType operator()()
    {
        REQUIRE( Thread::getId() == _expectedThreadId );

        return _func();
    }

    void ignoreThisInstance()
    {
        _ignoreThisInstance = true;
    }

private:
    bool                        _ignoreThisInstance = false;
    std::function<ResultType()> _func;
    Thread::Id                  _expectedThreadId;
};

inline void _testDispatcherTimer(IDispatcher* pDispatcher, bool throwException, Thread::Id expectedDispatcherThreadId)
{
    P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    TestDispatcherCallable_<bool> callable(
        [pData, throwException]() -> bool        
        {
            pData->callTimes.push_back( std::chrono::steady_clock::now() );

            // sleep a little so that we can verify that subsequent call times
            // are scheduled based on the scheduled time, not the finish time of
            // the handler.
            Thread::sleepSeconds( 0.05 );

            if(pData->callTimes.size()>=20)
                return false;

            if(throwException)
                throw InvalidArgumentError("test");

            return true;
        },
        expectedDispatcherThreadId,
        pData);
    
    pDispatcher->createTimer(
        0.1,
        callable
        );

    // disable the destructor test stuff on THIS copy of the callable.
    // We only want to verify it for the copy that the timer got.
    callable.ignoreThisInstance();

    CONTINUE_SECTION_AFTER_SECONDS(2.5, pData, startTime)
    {
        // our timer is expected to be called once every 100 ms.            
        // Verify that all expected timer calls have happened.
        // This also verifies that the timer has stopped once
        // we returned false.
        REQUIRE( pData->callTimes.size()==20 );

        // the callable should have been destroyed
        REQUIRE( pData->callableDestroyedCount>0 );

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


/** Tests an IDispatcher implementation. The dispatcher must execute the enqueued
    items automatically. It may do so in the same thread or a separate thread. */
inline void testDispatcher(IDispatcher* pDispatcher, Thread::Id expectedDispatcherThreadId)
{
    SECTION("single call")
    {
        IDispatcher::Priority prio;

        SECTION("Priority normal")
            prio = IDispatcher::Priority::normal;
        SECTION("Priority idle")
            prio = IDispatcher::Priority::idle;

        P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

        TestDispatcherCallable_<void> callable(
            [pData]()
            {
                pData->callOrder.push_back(0);
            },
            expectedDispatcherThreadId,
            pData );

        pDispatcher->enqueue(
            callable,
            prio);

        callable.ignoreThisInstance();

        CONTINUE_SECTION_AFTER_SECONDS(1, pData)
        {
            REQUIRE( pData->callOrder.size()==1 );
            REQUIRE( pData->callableDestroyedCount>0 )
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
                [expectedDispatcherThreadId]()
                {
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);

                    Thread::sleepSeconds(1);
                } );

            pDispatcher->enqueue(
                [pData, expectedDispatcherThreadId]()
                {
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
                    pData->callOrder.push_back(0);
                },
                prio);

            pDispatcher->enqueue(
                [pData, expectedDispatcherThreadId]()
                {
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
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
                [pData, expectedDispatcherThreadId]()
                {
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
                    pData->callOrder.push_back(0);
                },
                IDispatcher::Priority::normal );

            pDispatcher->enqueue(
                [pData, expectedDispatcherThreadId]()
                {
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
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
                [pData, expectedDispatcherThreadId]()
                {
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
                    pData->callOrder.push_back(0);
                },
                IDispatcher::Priority::idle );

            pDispatcher->enqueue(
                [pData, expectedDispatcherThreadId]()
                {
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
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
        SECTION("positive time")
        {
            IDispatcher::Priority prio;

            SECTION("Priority normal")
                prio = IDispatcher::Priority::normal;
            SECTION("Priority idle")
                prio = IDispatcher::Priority::idle;

            P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

            TestDispatcherCallable_<void> callable(
                [pData]()
                {     
                    pData->callOrder.push_back(0);
                },
                expectedDispatcherThreadId,
                pData );

            pDispatcher->enqueueInSeconds(
                2,
                callable,                
                prio);

            callable.ignoreThisInstance();

            CONTINUE_SECTION_AFTER_SECONDS(0.5, pData)
            {
                // should NOT have been called immediately
                REQUIRE( pData->callOrder.size()==0 );

                // should not have been destructed yet
                REQUIRE( pData->callableDestroyedCount==0 );

                CONTINUE_SECTION_AFTER_SECONDS(0.5, pData)
                {
                    // also not after 1 second
                    REQUIRE( pData->callOrder.size()==0 );
                    REQUIRE( pData->callableDestroyedCount==0 );

                    CONTINUE_SECTION_AFTER_SECONDS(2, pData)
                    {
                        // but after another 2 seconds it should have been called
                        REQUIRE( pData->callOrder.size()==1 );
                        REQUIRE( pData->callOrder[0]==0 );

                        REQUIRE( pData->callableDestroyedCount==1 );
                    };
                };            
            };
        }

        SECTION("time=0")
        {
            IDispatcher::Priority prio;

            SECTION("Priority normal")
                prio = IDispatcher::Priority::normal;
            SECTION("Priority idle")
                prio = IDispatcher::Priority::idle;

            P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

            pDispatcher->enqueueInSeconds(
                0,
                [pData, expectedDispatcherThreadId]()
                {       
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
                    pData->callOrder.push_back(0);
                },
                prio);

            CONTINUE_SECTION_AFTER_SECONDS(0.5, pData)
            {
                // should already have been called.
                REQUIRE( pData->callOrder.size()==1 );      
            };
        }

        SECTION("time=-1")
        {
            IDispatcher::Priority prio;

            SECTION("Priority normal")
                prio = IDispatcher::Priority::normal;
            SECTION("Priority idle")
                prio = IDispatcher::Priority::idle;

            P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

            pDispatcher->enqueueInSeconds(
                -1,
                [pData, expectedDispatcherThreadId]()
                {                
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
                    pData->callOrder.push_back(0);
                },
                prio);

            CONTINUE_SECTION_AFTER_SECONDS(0.5, pData)
            {
                // should already have been called.
                REQUIRE( pData->callOrder.size()==1 );      
            };
        }
    }

    SECTION("timer")
    {
        SECTION("no exception")
            _testDispatcherTimer(pDispatcher, false, expectedDispatcherThreadId );
            
        SECTION("exception")
            _testDispatcherTimer(pDispatcher, true, expectedDispatcherThreadId);        
    }
}


}
}

#endif

