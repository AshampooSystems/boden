#ifndef BDN_TEST_testDispatcher_H_
#define BDN_TEST_testDispatcher_H_

#include <bdn/IDispatcher.h>
#include <bdn/Array.h>


namespace bdn
{
namespace test
{

struct TestDispatcherData_ : public Base
{
    Array<int> callOrder;

    Array< std::chrono::steady_clock::time_point > callTimes;

    int callableDestroyedCount = 0;
    Array< Thread::Id > callableDestructWrongThreadIds;

    int unhandledProblemCount = 0;
};

class TestDispatcherCallableDataDestruct_ : public Base
{
public:
    TestDispatcherCallableDataDestruct_(Thread::Id expectedThreadId, TestDispatcherData_* pData )
    {
        _expectedThreadId = expectedThreadId;
        _pData = pData;
    }

    ~TestDispatcherCallableDataDestruct_()
    {
        // destructor MUST also be called from the expected thread

        // note that we cannot use REQUIRE here since it throws an exception and we are in a destructor.
        // instead we set a member in pData if the test fails and that is checked
        // from somewhere else.
        Thread::Id threadId = Thread::getCurrentId();
        if( threadId != _expectedThreadId )
            _pData->callableDestructWrongThreadIds.push_back( threadId );

        _pData->callableDestroyedCount++;
    }
    
private:
    Thread::Id                  _expectedThreadId;

	P<TestDispatcherData_>		_pData;
};


inline void _testDispatcherTimer(IDispatcher* pDispatcher, bool throwException, Thread::Id expectedDispatcherThreadId)
{
    P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    P<TestDispatcherCallableDataDestruct_> pDestructTest = newObj<TestDispatcherCallableDataDestruct_>( expectedDispatcherThreadId, pData);

    P<bdn::test::RedirectUnhandledProblem> pRedirectUnhandled;
    if(throwException)
    {
        pRedirectUnhandled = newObj<bdn::test::RedirectUnhandledProblem>(
            [pData](IUnhandledProblem& problem)
            {
                REQUIRE( problem.getType()==IUnhandledProblem::Type::exception );
                REQUIRE( problem.getErrorMessage()=="bla" );
                
                // ignore and continue.
                REQUIRE( problem.canKeepRunning() );
                problem.keepRunning();

                pData->unhandledProblemCount++;
            } );
    }
        
    pDispatcher->createTimer(
        0.25,
        [pData, expectedDispatcherThreadId, throwException, pDestructTest]() -> bool                
        {
            REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId );

            pData->callTimes.push_back( std::chrono::steady_clock::now() );

            // sleep a little so that we can verify that subsequent call times
            // are scheduled based on the scheduled time, not the finish time of
            // the handler.
            Thread::sleepSeconds( 0.1 );

            if(pData->callTimes.size()>=20)
                return false;

            if(throwException)
                throw InvalidArgumentError("bla");
            
            return true;
        }
        );

    pDestructTest = nullptr;

    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

    CONTINUE_SECTION_AFTER_SECONDS(7, pData, startTime, pRedirectUnhandled, throwException, waitStartTime)
    {
        std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
        std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

        // sanity check: verify that we have waited the expected amount of time.
        REQUIRE( waitDurationMillis.count() >= 6900 );
        
        // our timer is expected to be called once every 100 ms.            
        // Verify that all expected timer calls have happened.
        // This also verifies that the timer has stopped once
        // we returned false.
        REQUIRE( pData->callTimes.size()==20 );

        // the callable should have been destroyed
        REQUIRE( pData->callableDestroyedCount==1 );

        REQUIRE( pData->callableDestructWrongThreadIds.size()==0 );

        for(size_t i=0; i<pData->callTimes.size(); i++)
        {
            std::chrono::steady_clock::time_point callTime = pData->callTimes[i];

            double timeAfterStart = std::chrono::duration_cast<std::chrono::microseconds>(callTime-startTime).count() / 1000000.0;

            double expectedTimeAfterStart = (i+1)*0.25;

            // should not have been called before the expected time.
            // Note that we allow for minor rounding errors or drift that might have caused the call to happen
            // a tiny fractional time too early.
            REQUIRE( timeAfterStart>=expectedTimeAfterStart-0.01);

            // and not too much after the expected time. Note that we do allow for small
            // hiccups and differences, but the actual time must never be too far behind
            // the expected time.
            REQUIRE( timeAfterStart < expectedTimeAfterStart + 0.2);
        }

        if(throwException)
        {
            // we got 20 calls. 19 threw an exception, the last one did not because
            // it needed to stop the timer.
            REQUIRE( pData->unhandledProblemCount==19 );
        }
    };

}


/** Tests an IDispatcher implementation. The dispatcher must execute the enqueued
    items automatically. It may do so in the same thread or a separate thread. */
inline void testDispatcher(IDispatcher* pDispatcher, Thread::Id expectedDispatcherThreadId, bool canKeepRunningAfterUnhandledExceptions=true)
{
    SECTION("enqueue")
    {
        SECTION("single call")
        {
            IDispatcher::Priority prio;

            SECTION("Priority normal")
                prio = IDispatcher::Priority::normal;
            SECTION("Priority idle")
                prio = IDispatcher::Priority::idle;

            P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

            P<TestDispatcherCallableDataDestruct_> pDestructTest = newObj<TestDispatcherCallableDataDestruct_>( expectedDispatcherThreadId, pData);

            pDispatcher->enqueue(
                [pData, expectedDispatcherThreadId, pDestructTest]()
                {
                    // ensure that the enqueuing thread has time to release its reference to the destruct test object
                    Thread::sleepSeconds(0.1); 

                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId );

                    pData->callOrder.push_back(0);
                },
                prio);

            pDestructTest = nullptr;

            std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

            CONTINUE_SECTION_AFTER_SECONDS(1, pData, waitStartTime)
            {
                std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                // sanity check: verify that we have waited the expected amount of time.
                REQUIRE( waitDurationMillis.count() >= 900 );
        
                REQUIRE( pData->callOrder.size()==1 );
                REQUIRE( pData->callableDestroyedCount==1 );
                REQUIRE( pData->callableDestructWrongThreadIds.size()==0 );
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

                std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                CONTINUE_SECTION_AFTER_SECONDS(3, pData, waitStartTime)
                {
                    std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                    std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                    // sanity check: verify that we have waited the expected amount of time.
                    REQUIRE( waitDurationMillis.count() >= 2900 );
        
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

                std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                CONTINUE_SECTION_AFTER_SECONDS(1.5, pData, waitStartTime)
                {
                    std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                    std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                    // sanity check: verify that we have waited the expected amount of time.
                    REQUIRE( waitDurationMillis.count() >= 1400 );

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

                std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                CONTINUE_SECTION_AFTER_SECONDS(1.5, pData, waitStartTime)
                {
                    std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                    std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                    // sanity check: verify that we have waited the expected amount of time.
                    REQUIRE( waitDurationMillis.count() >= 1400 );

                    REQUIRE( pData->callOrder.size()==2);
            
                    // should have been called in reverse order
                    REQUIRE( pData->callOrder[0] == 1);
                    REQUIRE( pData->callOrder[1] == 0);
                };
            }
        }
        
        if(canKeepRunningAfterUnhandledExceptions)
        {
            SECTION("exception")
            {
                IDispatcher::Priority prio;

                SECTION("Priority normal")
                    prio = IDispatcher::Priority::normal;
                SECTION("Priority idle")
                    prio = IDispatcher::Priority::idle;

                P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

                P<TestDispatcherCallableDataDestruct_> pDestructTest = newObj<TestDispatcherCallableDataDestruct_>( expectedDispatcherThreadId, pData);

                P<bdn::test::RedirectUnhandledProblem> pRedirectUnhandled = newObj<bdn::test::RedirectUnhandledProblem>(
                    [pData](IUnhandledProblem& problem)
                    {
                        IUnhandledProblem::Type type = problem.getType();
                        String                  message = problem.getErrorMessage();
                        
                        REQUIRE( type==IUnhandledProblem::Type::exception );
                        REQUIRE( message=="bla" );

                        bool ok=false;
                        bool didntthrow = false;
                        bool wrongType = false;

                        do
                        {
                            try
                            {
                                problem.throwAsException();
                                didntthrow = true;
                            }
                            catch( InvalidArgumentError )
                            {
                                ok = true;
                            }
                            catch( ... )
                            {
                                wrongType = true;
                            }
                        } while( bdn::alwaysFalse() );

                        //REQUIRE_THROWS_AS( problem.throwAsException(), InvalidArgumentError );
                    
                        // ignore and continue.
                        REQUIRE( problem.canKeepRunning() );
                        problem.keepRunning();

                        pData->unhandledProblemCount++;
                    } );

                pDispatcher->enqueue(
                    [pDestructTest, pData]()
                    {
                        // ensure that the enqueuing thread has time to release its reference to the destruct test object
                        Thread::sleepSeconds(0.5); 

                        pData->callOrder.push_back(0);

                        throw InvalidArgumentError("bla");                
                    },
                    prio);

                pDestructTest = nullptr;

                std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                CONTINUE_SECTION_AFTER_SECONDS(2, pData, pRedirectUnhandled, waitStartTime)
                {
                    std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                    std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                    // sanity check: verify that we have waited the expected amount of time.
                    REQUIRE( waitDurationMillis.count() >= 1900 );

                    REQUIRE( pData->callOrder.size()==1 );
                    REQUIRE( pData->callableDestroyedCount==1 );
                    REQUIRE( pData->callableDestructWrongThreadIds.size()==0 );

                    REQUIRE( pData->unhandledProblemCount==1 );
                };
            }
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

            P<TestDispatcherCallableDataDestruct_> pDestructTest = newObj<TestDispatcherCallableDataDestruct_>( expectedDispatcherThreadId, pData);

            pDispatcher->enqueueInSeconds(
                2,
                [pData, expectedDispatcherThreadId, pDestructTest]()
                {     
                    REQUIRE( Thread::getCurrentId() == expectedDispatcherThreadId);
                    pData->callOrder.push_back(0);
                },
                prio);

            pDestructTest = nullptr;

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

                    std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                    CONTINUE_SECTION_AFTER_SECONDS(2, pData, waitStartTime)
                    {
                        std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                        std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                        // sanity check: verify that we have waited the expected amount of time.
                        REQUIRE( waitDurationMillis.count() >= 1900 );
                        
                        // but after another 2 seconds it should have been called
                        REQUIRE( pData->callOrder.size()==1 );
                        REQUIRE( pData->callOrder[0]==0 );

                        REQUIRE( pData->callableDestroyedCount==1 );
                        REQUIRE( pData->callableDestructWrongThreadIds.size()==0 );
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

            std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

            CONTINUE_SECTION_AFTER_SECONDS(0.9, pData, waitStartTime)
            {
                std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                // sanity check: verify that we have waited the expected amount of time.
                REQUIRE( waitDurationMillis.count() >= 800 );

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

            std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

            CONTINUE_SECTION_AFTER_SECONDS(0.9, pData, waitStartTime)
            {
                std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                // sanity check: verify that we have waited the expected amount of time.
                REQUIRE( waitDurationMillis.count() >= 800 );
                
                // should already have been called.
                REQUIRE( pData->callOrder.size()==1 );      
            };
        }


        if(canKeepRunningAfterUnhandledExceptions)
        {
            SECTION("exception")
            {
                IDispatcher::Priority prio;

                SECTION("Priority normal")
                    prio = IDispatcher::Priority::normal;
                SECTION("Priority idle")
                    prio = IDispatcher::Priority::idle;

                P<TestDispatcherData_> pData = newObj<TestDispatcherData_>();

                P<TestDispatcherCallableDataDestruct_> pDestructTest = newObj<TestDispatcherCallableDataDestruct_>( expectedDispatcherThreadId, pData);

                P<bdn::test::RedirectUnhandledProblem> pRedirectUnhandled = newObj<bdn::test::RedirectUnhandledProblem>(
                    [pData](IUnhandledProblem& problem)
                    {
                        REQUIRE( problem.getType()==IUnhandledProblem::Type::exception );
                        REQUIRE( problem.getErrorMessage()=="bla" );
                    
                        // ignore and continue.
                        REQUIRE( problem.canKeepRunning() );
                        problem.keepRunning();

                        pData->unhandledProblemCount++;
                    } );

                pDispatcher->enqueueInSeconds(
                    2,
                    [pDestructTest, pData]()
                    {
                        // ensure that the enqueuing thread has time to release its reference to the destruct test object
                        Thread::sleepSeconds(0.1); 

                        pData->callOrder.push_back(0);

                        throw InvalidArgumentError("bla");                
                    },
                    prio);

                pDestructTest = nullptr;

                std::chrono::steady_clock::time_point waitStartTime = std::chrono::steady_clock::now();

                CONTINUE_SECTION_AFTER_SECONDS(3, pData, pRedirectUnhandled, waitStartTime)
                {
                    std::chrono::steady_clock::time_point    waitEndTime = std::chrono::steady_clock::now();
                    std::chrono::milliseconds	             waitDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitEndTime - waitStartTime );

                    // sanity check: verify that we have waited the expected amount of time.
                    REQUIRE( waitDurationMillis.count() >= 2900 );

                    REQUIRE( pData->callOrder.size()==1 );
                    REQUIRE( pData->callableDestroyedCount==1 );
                    REQUIRE( pData->callableDestructWrongThreadIds.size()==0 );

                    REQUIRE( pData->unhandledProblemCount==1 );
                };

            }
        }
    }

    SECTION("timer")
    {
        SECTION("no exception")
            _testDispatcherTimer(pDispatcher, false, expectedDispatcherThreadId );
        
        if(canKeepRunningAfterUnhandledExceptions)
        {
            SECTION("exception")
                _testDispatcherTimer(pDispatcher, true, expectedDispatcherThreadId );
        }
    }
}


}
}

#endif

