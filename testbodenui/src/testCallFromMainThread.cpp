#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/mainThread.h>
#include <bdn/Thread.h>
#include <bdn/StopWatch.h>

using namespace bdn;


void testCallFromMainThread(bool throwException)
{
    StopWatch watch;

    SECTION("mainThread")
    {
        int callCount = 0;

        StopWatch watch;

        std::cout << "1" << std::endl;

        std::future<int> result = callFromMainThread( [&callCount, throwException](int x){ callCount++; if(throwException){ throw InvalidArgumentError("hello"); } return x*2; }, 42 );

        std::cout << "2" << std::endl;

        // should have been called immediately, since we are currently in the main thread
        REQUIRE( callCount==1 );

        std::cout << "3" << std::endl;

        REQUIRE( result.wait_for( std::chrono::milliseconds(0)) == std::future_status::ready  );

        std::cout << "4" << std::endl;

        if(throwException)
            REQUIRE_THROWS_AS( result.get(), InvalidArgumentError );
        else
            REQUIRE( result.get()==84 );

        std::cout << "5" << std::endl;

        // should not have waited at any point.
        REQUIRE( watch.getMillis()<1000 );
    }
    
#if BDN_HAVE_THREADS

	SECTION("otherThread")
	{
        SECTION("storingFuture")
        {
            CONTINUE_SECTION_IN_THREAD_WITH(
                         [throwException]()
                         {
                             volatile int   callCount = 0;
                             Thread::Id     threadId;

                             std::future<int> result = callFromMainThread(
                                                                          [&callCount, throwException, &threadId](int x)
                                                                          {
                                                                              // sleep a little to ensure that we have time to check callCount
                                                                              Thread::sleepSeconds(1);
                                                                              threadId = Thread::getCurrentId();
                                                                              callCount++;
                                                                              if(throwException)
                                                                                  throw InvalidArgumentError("hello");
                                                                              return x*2;
                                                                          },
                                                                          42 );


                             // should NOT have been called immediately, since we are in a different thread.
                             // Instead the call should have been deferred to the main thread.
                             REQUIRE( callCount==0 );

                             StopWatch threadWatch;

                             REQUIRE( result.wait_for( std::chrono::milliseconds(5000) ) == std::future_status::ready );

                             REQUIRE( threadWatch.getMillis()>=500 );
                             REQUIRE( threadWatch.getMillis()<=5500 );

                             REQUIRE( callCount==1 );

                             REQUIRE( threadId==Thread::getMainId() );
                             REQUIRE( threadId!=Thread::getCurrentId() );

                             threadWatch.start();

                             if(throwException)
                                 REQUIRE_THROWS_AS(result.get(), InvalidArgumentError);
                             else
                                 REQUIRE( result.get()==84 );

                             // should not have waited
                             REQUIRE( threadWatch.getMillis()<=500 );
                         } );


            // time to start thread should have been less than 1000ms
            REQUIRE( watch.getMillis()<1000 );
        }

        SECTION("notStoringFuture")
        {
            CONTINUE_SECTION_IN_THREAD_WITH(
                         [throwException]()
                         {
                             struct Data : public Base
                             {
                                 volatile int callCount = 0;
                             };

                             P<Data> pData = newObj<Data>();

                             StopWatch threadWatch;

                             callFromMainThread(   [pData, throwException](int x)
                                                {
                                                    Thread::sleepMillis(1000);

                                                    pData->callCount++;
                                                    if(throwException)
                                                        throw InvalidArgumentError("hello");
                                                    return x*2;
                                                },
                                                42 );


                             // should NOT have been called immediately, since we are in a different thread.
                             // Instead the call should have been deferred to the main thread.
                             REQUIRE( pData->callCount==0 );

                             // should NOT have waited in this thread.
                             REQUIRE( threadWatch.getMillis()<1000 );

                             // wait until the call happened before we exit
                             while(true)
                             {
                                 Thread::sleepMillis(100);
                                 if(pData->callCount!=0)
                                     break;
                             }

                         } );


            // time to start thread should have been less than 1000ms
            REQUIRE( watch.getMillis()<1000 );
        }
    }
	    
#endif
}

#if BDN_HAVE_THREADS


class TestCallFromMainThreadOrderingBase : public Base
{
public:

    virtual void scheduleCall(std::function<void()> func)=0;
    virtual bool mainThreadCallsShouldExecuteImmediately()=0;

    void start()
    {
        std::list< std::future<void> > futures;

        P<TestCallFromMainThreadOrderingBase> pThis = this;

        // add a call from the main thread first
        {
            MutexLock lock(_mutex);
            _expectedOrder.push_back(-1);

            scheduleCall([pThis]()
                         {
                             pThis->_actualOrder.push_back(-1);
                         });
        }

        // start 100 threads. Each schedules a call in the main thread.
        _scheduledPending = 101;
        for(int i=0; i<_scheduledPending-1 ; i++)
        {
            futures.push_back( Thread::exec(
                    [i, pThis]()
                    {
                        MutexLock lock(pThis->_mutex);
                        pThis->_expectedOrder.push_back(i);

                        pThis->scheduleCall([i, pThis]()
                                           {
                                               pThis->_actualOrder.push_back(i);
                                               pThis->onScheduledDone();
                                           });

                    } ) );
        }

        // also add a call from the main thread
        {
            MutexLock lock(_mutex);

            scheduleCall([pThis]()
                          {
                              pThis->_actualOrder.push_back(9999);
							  pThis->onScheduledDone();
                          });

            if(mainThreadCallsShouldExecuteImmediately())
            {
                // if main thread calls are executed immediately then the -1 call already happened
                // and the 9999 call was immediately executed above. So the 9999 call should
                // be on the second position in the order.
                _expectedOrder.insert( _expectedOrder.begin()+1, 9999);
            }
            else
                _expectedOrder.push_back(9999);
        }

        // wait for all threads to finish (i.e. for all callbacks to be scheduled)
        for( std::future<void>& f: futures)
            f.get();

        scheduleTestContinuationIfNecessary();
    }

    void onScheduledDone()
    {
        _scheduledPending--;
        if(_scheduledPending==0)
            onDone();
    }

    void onDone()
    {
        // now verify that the scheduling order and the call order are the same
        for(size_t i=0; i<_expectedOrder.size(); i++)
        {
            REQUIRE( _actualOrder.size()>i);
            REQUIRE( _expectedOrder[i] == _actualOrder[i] );
        }
        REQUIRE( _actualOrder.size() == _expectedOrder.size());

        _done = true;
    }

    void scheduleTestContinuationIfNecessary()
    {
        if(!_done)
        {
            P<TestCallFromMainThreadOrderingBase> pThis = this;

            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pThis)
            {
                pThis->scheduleTestContinuationIfNecessary();
            };
        }
    }

    Mutex               _mutex;
    std::vector<int>	_expectedOrder;
    std::vector<int>	_actualOrder;

    int                 _scheduledPending;

    bool                _done = false;
};


class TestCallFromMainThreadOrdering_Sync : public TestCallFromMainThreadOrderingBase
{
public:

    void scheduleCall(std::function<void()> func) override
    {
        callFromMainThread(func);
    }

    bool mainThreadCallsShouldExecuteImmediately() override
    {
        return true;
    }
};



void testCallFromMainThreadOrdering()
{
    P<TestCallFromMainThreadOrdering_Sync> pTest = newObj<TestCallFromMainThreadOrdering_Sync>();

    pTest->start();
}

#endif

TEST_CASE("callFromMainThread")
{
    SECTION("noException")
        testCallFromMainThread(false);

    SECTION("exception")
        testCallFromMainThread(true);

#if BDN_HAVE_THREADS
	SECTION("ordering")
		testCallFromMainThreadOrdering();
#endif
}



void testAsyncCallFromMainThread(bool throwException)
{
    StopWatch watch;

    struct Data : public Base
    {
        volatile int callCount = 0;
    };


    SECTION("mainThread")
    {
        P<Data> pData = newObj<Data>();

        StopWatch watch;
        
        asyncCallFromMainThread(    [pData, throwException](int x)
                                    {
                                        pData->callCount++;
                                        
                                        // schedule another call. We verify that
                                        // additional calls are still processed even if
                                        // an exception occurred in a previous call
                                        asyncCallFromMainThread(
                                            [pData]()
                                            {
                                                pData->callCount++;
                                            } );
                                        
                                        if(throwException)
                                            throw InvalidArgumentError("hello");
                                        
                                        return x*2;
                                    }, 42 );
        
        // should NOT have been called immediately, even though we are on the main thread
        REQUIRE( pData->callCount==0 );
        
        // should not have waited
        REQUIRE( watch.getMillis()<1000 );

        CONTINUE_SECTION_AFTER_PENDING_EVENTS(pData)
        {
            // the test continuation will be executed after the async call we scheduled.
            REQUIRE( pData->callCount==1 );

            // another async call was scheduled by the previous one. Check that in another
            // test continuation.
            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pData)
            {
                REQUIRE( pData->callCount==2 );
                // done.
            };
        };
    }

#if BDN_HAVE_THREADS

    SECTION("otherThread")
    {
        CONTINUE_SECTION_IN_THREAD_WITH(
                     [throwException]()
                     {
                         P<Data> pData = newObj<Data>();

                         StopWatch threadWatch;

                         asyncCallFromMainThread(   [pData, throwException](int x)
                                                    {
                                                        Thread::sleepMillis(2000);
                                                        pData->callCount++;
                                                        if(throwException)
                                                            throw InvalidArgumentError("hello");
                                                        return x*2;
                                                    }
                                                    ,42 );


                         // should NOT have been called immediately, since we are in a different thread.
                         // Instead the call should have been deferred to the main thread.
                         REQUIRE( pData->callCount==0 );

                         // should NOT have waited.
                         REQUIRE( threadWatch.getMillis()<1000 );

                         Thread::sleepMillis(3000);

                         // NOW the function should have been called
                         REQUIRE( pData->callCount==1 );
                     } );

    }
    
#endif

}



#if BDN_HAVE_THREADS


class TestCallFromMainThreadOrdering_Async : public TestCallFromMainThreadOrderingBase
{
public:

    void scheduleCall(std::function<void()> func) override
    {
        asyncCallFromMainThread(func);
    }

    bool mainThreadCallsShouldExecuteImmediately() override
    {
        return false;
    }
};

void testAsyncCallFromMainThreadOrdering()
{
    P<TestCallFromMainThreadOrdering_Async> pTest = newObj<TestCallFromMainThreadOrdering_Async>();

    pTest->start();
}

#endif


TEST_CASE("asyncCallFromMainThread")
{
    SECTION("noException")
        testAsyncCallFromMainThread(false);

    SECTION("exception")
        testAsyncCallFromMainThread(true);

#if BDN_HAVE_THREADS
	SECTION("ordering")
		testAsyncCallFromMainThreadOrdering();
#endif
}


void testWrapCallFromMainThread(bool throwException)
{
    StopWatch watch;

    SECTION("mainThread")
    {
        int callCount = 0;

        StopWatch watch;

        auto wrapped = wrapCallFromMainThread<int>( [&callCount, throwException](int val)
                                                    {
                                                        callCount++;
                                                        if(throwException)
                                                            throw InvalidArgumentError("hello");
                                                        return val*2;
                                                    } );

        // should not have been called yet
        REQUIRE( callCount==0 );

        std::future<int> result = wrapped(42);

        // should have been called immediately, since we are currently in the main thread
        REQUIRE( callCount==1 );

        REQUIRE( result.wait_for( std::chrono::milliseconds(0)) == std::future_status::ready  );

        if(throwException)
            REQUIRE_THROWS_AS( result.get(), InvalidArgumentError );
        else
            REQUIRE( result.get()==84 );

        // should not have waited at any point.
        REQUIRE( watch.getMillis()<1000 );
    }
    
#if BDN_HAVE_THREADS

    SECTION("otherThread")
    {
        SECTION("storingFuture")
        {
            CONTINUE_SECTION_IN_THREAD(throwException)
            {
                volatile int   callCount = 0;
                Thread::Id     threadId;

                auto wrapped = wrapCallFromMainThread<int>([&callCount, throwException, &threadId](int x)
                                                        {
                                                            // sleep a little to ensure that we have time to check callCount
                                                            Thread::sleepSeconds(1);
                                                            threadId = Thread::getCurrentId();
                                                            callCount++;
                                                            if(throwException)
                                                                throw InvalidArgumentError("hello");
                                                            return x*2;
                                                        } );

                // should NOT have been called.
                REQUIRE( callCount==0 );

                Thread::sleepSeconds(2);

                // should STILL not have been called, since the wrapper was not executed yet
                REQUIRE( callCount==0 );

                StopWatch threadWatch;

                std::future<int> result = wrapped(42);

                // should NOT have been called immediately, since we are in a different thread.
                // Instead the call should have been deferred to the main thread.
                REQUIRE( callCount==0 );

                // should not have waited
                REQUIRE( threadWatch.getMillis()<500 );

                REQUIRE( result.wait_for( std::chrono::milliseconds(5000) ) == std::future_status::ready );

                // the inner function sleeps for 1 second.
                REQUIRE( threadWatch.getMillis()>=1000-10 );
                REQUIRE( threadWatch.getMillis()<2500 );

                REQUIRE( callCount==1 );

                REQUIRE( threadId==Thread::getMainId() );
                REQUIRE( threadId!=Thread::getCurrentId() );

                threadWatch.start();

                if(throwException)
                    REQUIRE_THROWS_AS(result.get(), InvalidArgumentError);
                else
                    REQUIRE( result.get()==84 );

                // should not have waited
                REQUIRE( threadWatch.getMillis()<=500 );
            };
        }

        SECTION("notStoringFuture")
        {
            CONTINUE_SECTION_IN_THREAD_WITH(
                         [throwException]()
                         {
                             struct Data : public Base
                             {
                                 volatile int callCount = 0;
                             };

                             P<Data> pData = newObj<Data>();

                             StopWatch threadWatch;

                             {
                                 auto wrapped = wrapCallFromMainThread<int>([pData, throwException](int x)
                                                                        {
                                                                            Thread::sleepMillis(2000);
                                                                            pData->callCount++;
                                                                            if(throwException)
                                                                                throw InvalidArgumentError("hello");
                                                                            return x*2;
                                                                        } );


                                 // should NOT have been called yet.
                                 REQUIRE( pData->callCount==0 );

                                 // should not have waited
                                 REQUIRE( threadWatch.getMillis()<500 );

                                 Thread::sleepSeconds(2);

                                 // should STILL not have been called, since the wrapper was not executed yet
                                 REQUIRE( pData->callCount==0 );

                                 threadWatch.start();

                                 wrapped(42);

                                 // should NOT have been called immediately, since we are in a different thread.
                                 // Instead the call should have been deferred to the main thread.
                                 REQUIRE( pData->callCount==0 );

                                 // should not have waited
                                 REQUIRE( threadWatch.getMillis()<500 );


                                 // wait a little
                                 Thread::sleepMillis(3000);

                                 // NOW the function should have been called
                                 REQUIRE( pData->callCount==1 );

                             }

                             // the other thread's pData reference should have been released
                             REQUIRE( pData->getRefCount()==1 );
                         } );

        }
    }

#endif

}

TEST_CASE("wrapCallFromMainThread")
{
    SECTION("noException")
        testWrapCallFromMainThread(false);

    SECTION("exception")
        testWrapCallFromMainThread(true);
}







void testWrapAsyncCallFromMainThread(bool throwException)
{

    SECTION("mainThread")
    {
        struct Data : public Base
        {
            Thread::Id  threadId;
            int         callCount = 0;
        };
        P<Data> pData = newObj<Data>();

        StopWatch watch;

        auto wrapped = wrapAsyncCallFromMainThread<int>( [pData, throwException](int val)
                                                        {
                                                            pData->callCount++;
                                                            pData->threadId = Thread::getCurrentId();

                                                            if(throwException)
                                                                throw InvalidArgumentError("hello");

                                                            return val*2;
                                                        } );

        // should not have been called
        REQUIRE( pData->callCount==0 );

        wrapped(42);

        // should still not have been called (even though we are on the main thread).
        REQUIRE( pData->callCount==0 );

        // shoudl not have waited.
        REQUIRE( watch.getMillis()<500 );        
        
        CONTINUE_SECTION_AFTER_PENDING_EVENTS(pData)
        {
            Thread::sleepMillis(2000);

            // now the call should have happened.
            REQUIRE( pData->callCount==1 );

            // and it should have happened from the main thread.
            REQUIRE( pData->threadId==Thread::getMainId() );
        };
    }
    
#if BDN_HAVE_THREADS

    SECTION("otherThread")
    {
        CONTINUE_SECTION_IN_THREAD_WITH(
                     [throwException]()
                     {
                         volatile int   callCount = 0;
                         Thread::Id     threadId;

                         auto wrapped = wrapAsyncCallFromMainThread<int>([&callCount, throwException, &threadId](int x)
                                                                    {
                                                                        // sleep a little to ensure that we have time to check callCount
                                                                        Thread::sleepSeconds(1);
                                                                        threadId = Thread::getCurrentId();
                                                                        callCount++;
                                                                        if(throwException)
                                                                            throw InvalidArgumentError("hello");
                                                                        return x*2;
                                                                    } );

                         // should NOT have been called.
                         REQUIRE( callCount==0 );

                         Thread::sleepSeconds(2);

                         // should STILL not have been called, since the wrapper was not executed yet
                         REQUIRE( callCount==0 );

                         StopWatch threadWatch;

                         wrapped(42);

                         // should NOT have been called immediately, since we are in a different thread.
                         // Instead the call should have been deferred to the main thread.
                         REQUIRE( callCount==0 );

                         // should not have waited
                         REQUIRE( threadWatch.getMillis()<500 );

                         // sleep a while
                         Thread::sleepSeconds(3);

                         // now the call should have happened.
                         REQUIRE( callCount==1 );

                         REQUIRE( threadId==Thread::getMainId() );
                         REQUIRE( threadId!=Thread::getCurrentId() );
                     } );
    }
    
#endif

}

TEST_CASE("wrapAsyncCallFromMainThread")
{
    SECTION("noException")
        testWrapAsyncCallFromMainThread(false);

    SECTION("exception")
        testWrapAsyncCallFromMainThread(true);
}


class TestAsyncCallFromMainThreadAfterSeconds : public Base
{
public:
    TestAsyncCallFromMainThreadAfterSeconds(bool exception, double seconds)
    {
        _exception = exception;
        _seconds = seconds;
    }

    void runTest()
    {
        _pStopWatch = newObj<StopWatch>();

        P<TestAsyncCallFromMainThreadAfterSeconds> pThis = this;
        
        asyncCallFromMainThreadAfterSeconds(
            _seconds,
            [pThis]
            {
                pThis->onCalled();
            } );

        
        // should not have been called yet
        REQUIRE( !_called );

        CONTINUE_SECTION_AFTER_PENDING_EVENTS(pThis)
        {
            pThis->continueTest();
        };
    }

    void onCalled()
    {
        // should only be called once
        REQUIRE( !_called );

        REQUIRE_IN_MAIN_THREAD();

        _called = true;        

        if(_exception)
            throw InvalidArgumentError("hello");
    }

protected:
    void continueTest()
    {
        int64_t expectedMillis = (int64_t)(_seconds * 1000);
        int64_t maxMillis = expectedMillis + 500;
        
        int64_t elapsedMillis = _pStopWatch->getMillis();        

        if( _called )
        {
            REQUIRE( elapsedMillis>=expectedMillis-1 );
            REQUIRE( elapsedMillis <= maxMillis);

            // test successfully done.
        }
        else
        {
            // not yet called. Has the time expired yet?           

            REQUIRE( elapsedMillis <= maxMillis);

            // sleep a short time and then run another continuation
    
            Thread::sleepMillis(20);

            P<TestAsyncCallFromMainThreadAfterSeconds> pThis = this;

            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pThis)
            {
                pThis->continueTest();
            };
        }
    }

    bool            _called = false;

    bool            _exception;
    double          _seconds;
    P<StopWatch>    _pStopWatch;
};

void testAsyncCallFromMainThreadAfterSeconds(bool exception)
{
    double seconds;

    SECTION("zero")
        seconds = 0;

    SECTION("almostZero")
        seconds = 0.0000000001;

    SECTION("millis")
        seconds = 0.2;   

    SECTION("seconds")
        seconds = 2.5;    

    P<TestAsyncCallFromMainThreadAfterSeconds> pTest = newObj<TestAsyncCallFromMainThreadAfterSeconds>(exception, seconds);
    
    pTest->runTest();
}

TEST_CASE("asyncCallFromMainThreadAfterSeconds")
{
    SECTION("noException")
        testAsyncCallFromMainThreadAfterSeconds(false);

    SECTION("exception")
        testAsyncCallFromMainThreadAfterSeconds(true);
}


