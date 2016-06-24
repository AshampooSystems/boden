#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Thread.h>
#include <bdn/StopWatch.h>

using namespace bdn;


class TestStopRunnable : public ThreadRunnableBase
{
public:
    TestStopRunnable(bool throwException = false)
    {
        _throwException = throwException;
    }

    volatile bool waiting = false;
    volatile bool done = false;

    bool _throwException;

    void run() override
    {
        waiting = true;

        while(!shouldStop())
            Thread::sleepMillis(20);

        done = true;

        if(_throwException)
            throw InvalidArgumentError("hello");
    }

};

#if BDN_HAVE_THREADS

void testSignalStop()
{
    P<TestStopRunnable> pRunnable = newObj<TestStopRunnable>();

    Thread thread( pRunnable );

    Thread::sleepSeconds(2);

    REQUIRE( pRunnable->waiting );
    REQUIRE( !pRunnable->done );

    thread.signalStop();

    Thread::sleepSeconds(2);

    REQUIRE( pRunnable->waiting );
    REQUIRE( pRunnable->done );
}

void testStop(bool throwException)
{
    P<TestStopRunnable> pRunnable = newObj<TestStopRunnable>(throwException);

    Thread thread( pRunnable );

    StopWatch watch;

    SECTION("rethrowException")
    {
        if(throwException)
            REQUIRE_THROWS( thread.stop( Thread::ExceptionThrow ) );
        else
            thread.stop( Thread::ExceptionThrow );

        REQUIRE( pRunnable->done );

        REQUIRE( watch.getMillis()<2000 );
    }

    SECTION("ignoreException")
    {
        thread.stop( Thread::ExceptionIgnore );

        REQUIRE( pRunnable->done );

        REQUIRE( watch.getMillis()<2000 );
    }

    SECTION("ignore-rethrow-ignore-exception")
    {
        thread.stop( Thread::ExceptionIgnore );

        REQUIRE( watch.getMillis()<2000 );

        watch.start();

        if(throwException)
            REQUIRE_THROWS( thread.stop( Thread::ExceptionThrow ) );
        else
            thread.stop( Thread::ExceptionThrow );

        REQUIRE( watch.getMillis()<1000 );

        watch.start();

        thread.stop( Thread::ExceptionIgnore );

        REQUIRE( watch.getMillis()<1000 );
    }
}

void testStop()
{
    SECTION("exception")
        testStop(true);

    SECTION("noException")
        testStop(false);
}


class TestJoinRunnable : public ThreadRunnableBase
{
public:
    TestJoinRunnable(bool throwException)
    {
        _throwException = throwException;
    }

    void run() override
    {
        Thread::sleepSeconds(5);

        done = true;

        if(_throwException)
           throw InvalidArgumentError("hello");
    }

    bool _throwException;
    volatile bool done = false;


};

void testJoin()
{
    SECTION("noException")
    {
        P<TestJoinRunnable> pRunnable = newObj<TestJoinRunnable>(false);

        Thread thread( pRunnable );

        StopWatch watch;

        thread.join( Thread::ExceptionThrow );

        REQUIRE( watch.getMillis()>=5000 );
        REQUIRE( watch.getMillis()<7000 );

        watch.start();

        thread.join( Thread::ExceptionThrow );

        // should return immediately
        REQUIRE( watch.getMillis()<1000 );
    }

    SECTION("exception")
    {
        P<TestJoinRunnable> pRunnable = newObj<TestJoinRunnable>(true);

        Thread thread( pRunnable );

        SECTION("throw")
            REQUIRE_THROWS( thread.join( Thread::ExceptionThrow ) );


        SECTION("noThrow")
            thread.join( Thread::ExceptionIgnore );


        SECTION("noThrowThrowNoThrow")
        {
            thread.join( Thread::ExceptionIgnore );

            REQUIRE_THROWS( thread.join( Thread::ExceptionThrow ) );

            thread.join( Thread::ExceptionIgnore );
        }
    }
}

void verifyDestructWithoutStopSupport(bool throwException)
{
    // should behave the same, no matter whether an exception is thrown or not.

    P<TestJoinRunnable> pRunnable = newObj<TestJoinRunnable>( throwException );

    StopWatch watch;

    {
        Thread thread( pRunnable );
    }

    // should have "stopped" the thread.
    REQUIRE( pRunnable->done );
    REQUIRE( watch.getMillis()>=5000 );
    REQUIRE( watch.getMillis()<7000 );
}

void verifyDestructWithStopSupport(bool throwException)
{
    // should behave the same, no matter whether an exception is thrown or not.

    P<TestStopRunnable> pRunnable = newObj<TestStopRunnable>( throwException );

    StopWatch watch;

    {
        Thread thread( pRunnable );
    }

    // should have "stopped" the thread.
    REQUIRE( pRunnable->done );
    REQUIRE( watch.getMillis()<1000 );
}

void verifyDestruct(bool throwException)
{
    SECTION("noStopSupport")
        verifyDestructWithoutStopSupport(throwException);

    SECTION("withStopSupport")
        verifyDestructWithStopSupport(throwException);
}

void testDestruct()
{
    SECTION("noException")
        verifyDestruct(false);

    SECTION("exception")
        verifyDestruct(true);
}


class TestIdRunnable : public ThreadRunnableBase
{
public:
    TestIdRunnable()
    {
    }

    void run() override
    {
        idFromThread = Thread::getCurrentId();

        Thread::sleepSeconds(5);
    }

    Thread::Id idFromThread;
};

void testId()
{
    P<TestIdRunnable> pRunnable1 = newObj<TestIdRunnable>();
    P<TestIdRunnable> pRunnable2 = newObj<TestIdRunnable>();

    Thread thread1( pRunnable1 );
    Thread thread2( pRunnable2 );

    Thread::sleepSeconds(2);

    REQUIRE( pRunnable1->idFromThread == thread1.getId() );
    REQUIRE( pRunnable2->idFromThread == thread2.getId() );

    REQUIRE( pRunnable1->idFromThread != pRunnable2->idFromThread );
    REQUIRE( pRunnable1->idFromThread != Thread::getMainId() );
    REQUIRE( pRunnable2->idFromThread != Thread::getMainId() );

    REQUIRE( pRunnable1->idFromThread != Thread::getCurrentId() );
    REQUIRE( pRunnable2->idFromThread != Thread::getCurrentId() );

    thread1.join( Thread::ExceptionThrow );
    thread2.join( Thread::ExceptionThrow );

    // id should still be set
    REQUIRE( thread1.getId() == pRunnable1->idFromThread );
    REQUIRE( thread2.getId() == pRunnable2->idFromThread );
}

void testRunnableReferences()
{
    P<TestJoinRunnable> pRunnable = newObj<TestJoinRunnable>(false);

    Thread thread( pRunnable );

    REQUIRE( pRunnable->getRefCount()==2 );

    thread.join( Thread::ExceptionThrow );

    REQUIRE( pRunnable->getRefCount()==1 );
}

void testDetach()
{
    P<TestJoinRunnable> pRunnable = newObj<TestJoinRunnable>(false);

    Thread thread( pRunnable );

    Thread::Id id = thread.getId();

    StopWatch watch;

    thread.detach();

    REQUIRE( watch.getMillis()<2000 );

    REQUIRE( thread.getId()==id );

    watch.start();

    SECTION("joinAfterDetach")
    {
        SECTION("rethrow")
            REQUIRE_THROWS_AS( thread.join(Thread::ExceptionThrow), ThreadDetachedError );

        SECTION("noThrow")
            REQUIRE_THROWS_AS( thread.join(Thread::ExceptionIgnore), ThreadDetachedError );
    }

    SECTION("stopAfterDetach")
    {
        SECTION("rethrow")
            REQUIRE_THROWS_AS( thread.stop(Thread::ExceptionThrow), ThreadDetachedError );

        SECTION("noThrow")
            REQUIRE_THROWS_AS( thread.stop(Thread::ExceptionIgnore), ThreadDetachedError );
    }

    SECTION("signalAfterDetach")
        REQUIRE_THROWS_AS( thread.signalStop(), ThreadDetachedError );

    REQUIRE( watch.getMillis()<2000 );


    Thread::sleepSeconds(5);

    REQUIRE( thread.getId()==id );
}


void verifyExec(bool throwException)
{
    StopWatch watch;

    SECTION("dontStoreResult")
    {
        Thread::exec( [throwException](int val){ Thread::sleepSeconds(2); if(throwException){ throw InvalidArgumentError("hello"); } return val; }, 42 );

        REQUIRE( watch.getMillis()<1000 );
    }

    SECTION("storeResultThenDestructImmediately")
    {
        {
            std::future<int> result = Thread::exec( [throwException](int val){ Thread::sleepSeconds(2); if(throwException){ throw InvalidArgumentError("hello"); } return val; }, 42 );
        }

        REQUIRE( watch.getMillis()<1000 );
    }

    SECTION("storeResult")
    {
        std::future<int> result = Thread::exec( [throwException](int val){ Thread::sleepSeconds(2); if(throwException){ throw InvalidArgumentError("hello"); } return val; }, 42 );

        REQUIRE( watch.getMillis()<1000 );

        SECTION("get")
        {
            if(throwException)
                REQUIRE_THROWS_AS( result.get(), InvalidArgumentError );
            else
            {
                int val = result.get();
                REQUIRE( val==42 );
            }

            REQUIRE( watch.getMillis()>=2000-10 );
            REQUIRE( watch.getMillis()<3000 );
        }

        SECTION("wait_for")
        {
            std::future_status waitStatus = result.wait_for( std::chrono::milliseconds(500) );
            REQUIRE( waitStatus == std::future_status::timeout );

            REQUIRE( watch.getMillis()>=500-10 );
            REQUIRE( watch.getMillis()<1500 );

            waitStatus = result.wait_for( std::chrono::milliseconds(2000) );
            REQUIRE( waitStatus == std::future_status::ready );

            REQUIRE( watch.getMillis()>=2000-10 );
            REQUIRE( watch.getMillis()<3000 );

            watch.start();

            if(throwException)
                REQUIRE_THROWS_AS( result.get(), InvalidArgumentError );
            else
            {
                int val = result.get();
                REQUIRE( val==42 );
            }

            // should have returned immediately.
            REQUIRE( watch.getMillis()<500 );
        }

        SECTION("wait_until")
        {
            std::future_status waitStatus = result.wait_until( std::chrono::system_clock::now() + std::chrono::milliseconds(500) );
            REQUIRE( waitStatus == std::future_status::timeout );

            REQUIRE( watch.getMillis()>=500-10 );
            REQUIRE( watch.getMillis()<1500 );

            waitStatus = result.wait_until( std::chrono::system_clock::now() + std::chrono::milliseconds(2000) );
            REQUIRE( waitStatus == std::future_status::ready );

            REQUIRE( watch.getMillis()>=2000-10 );
            REQUIRE( watch.getMillis()<3000 );

            watch.start();

            if(throwException)
                REQUIRE_THROWS_AS( result.get(), InvalidArgumentError );
            else
            {
                int val = result.get();
                REQUIRE( val==42 );
            }


            // should have returned immediately.
            REQUIRE( watch.getMillis()<500 );
        }

        SECTION("wait")
        {
            result.wait();

            REQUIRE( watch.getMillis()>=2000-10 );
            REQUIRE( watch.getMillis()<3000 );

            watch.start();

            if(throwException)
                REQUIRE_THROWS_AS( result.get(), InvalidArgumentError );
            else
            {
                int val = result.get();
                REQUIRE( val==42 );
            }

            // should have returned immediately.
            REQUIRE( watch.getMillis()<500 );
        }

        SECTION("assignDefaultConstructed")
        {
            result = std::future<int>();

            // should not have waited
            REQUIRE( watch.getMillis()<500 );
        }
    }
}

void testExec()
{
    SECTION("noException")
        verifyExec(false);

    SECTION("exception")
        verifyExec(true);
}


TEST_CASE( "Thread", "[.][long]" )
{
    SECTION("id")
        testId();

    SECTION("signalStop")
        testSignalStop();

    SECTION("join")
        testJoin();

    SECTION("destruct")
        testDestruct();

    SECTION("stop")
        testStop();

    SECTION("detach")
        testDetach();

    SECTION("runnableReferences")
        testRunnableReferences();

    SECTION("exec")
        testExec();

}

#else

// multi-threading is not supported
TEST_CASE("Thread")
{
    REQUIRE( Thread::getCurrentId()==0 );
    REQUIRE( Thread::getMainId()==0 );
    REQUIRE( Thread::isCurrentMain() );
}

#endif

void verifySleep( std::function<void()> func, int expectedSleepMillis)
{
	auto startTime = std::chrono::system_clock::now();

	func();

	auto	duration = std::chrono::system_clock::now() - startTime;
	int64_t durationMillis = std::chrono::duration_cast< std::chrono::milliseconds >( duration ).count();

	REQUIRE( durationMillis>=expectedSleepMillis-10);
	REQUIRE( durationMillis<=expectedSleepMillis + 500);
}

void testSleepSeconds()
{
	SECTION("<0")
		verifySleep( [](){ Thread::sleepSeconds(-1); }, 0 );

	SECTION("0")
		verifySleep( [](){ Thread::sleepSeconds(0); }, 0 );

	SECTION(">0")
		verifySleep( [](){ Thread::sleepSeconds(1.5); }, 1500 );
}

void testSleepMillis()
{
	SECTION("<0")
		verifySleep( [](){ Thread::sleepMillis(-1); }, 0 );

	SECTION("0")
		verifySleep( [](){ Thread::sleepMillis(0); }, 0 );

	SECTION(">0")
		verifySleep( [](){ Thread::sleepMillis(1500); }, 1500 );
}

void testYield()
{
	verifySleep( [](){ Thread::yield(); }, 0 );
}

TEST_CASE("Thread-Statics")
{
	SECTION("sleepSeconds")
		testSleepSeconds();

	SECTION("sleepMillis")
		testSleepMillis();

	SECTION("yield")
		testYield();
}


class ThreadLocalTestData : public Base
{
public:
    static std::atomic<int> constructedCount;
    static std::atomic<int> destructedCount;
    
    ThreadLocalTestData(int val=17)
    {
        _val = val;
        
        constructedCount++;
    }
    
    ~ThreadLocalTestData()
    {
        destructedCount++;
    }
    
    void dummy()
    {
    }
    
    int _val;
};

std::atomic<int> ThreadLocalTestData::constructedCount;
std::atomic<int> ThreadLocalTestData::destructedCount;


BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, pThreadLocal1 );


P<ThreadLocalTestData>& getFromFuncTestThreadLocal()
{
    BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, pThreadLocal );
    
    return pThreadLocal;
}
	
TEST_CASE("ThreadLocalStorage")
{
    SECTION( "initial value" )
    {
        SECTION("withConstructor")
        {
            BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, pThreadLocal );
    
            REQUIRE( pThreadLocal==nullptr );
        }
        
        SECTION("int")
        {
            BDN_STATIC_THREAD_LOCAL( int, threadLocal );
            
            REQUIRE( threadLocal==0 );
        }
        
        SECTION("bool")
        {
            BDN_STATIC_THREAD_LOCAL( bool, threadLocal );
            
            REQUIRE( threadLocal==false );
        }
        
        SECTION("void*")
        {
            BDN_STATIC_THREAD_LOCAL( void*, pThreadLocal );
            
            REQUIRE( pThreadLocal==nullptr );
        }
    }
    
    SECTION( "setGet" )
    {
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, pThreadLocal );
        
        P<ThreadLocalTestData> pData = newObj<ThreadLocalTestData>(42);
        
        pThreadLocal = pData;
        
        REQUIRE( pThreadLocal!=nullptr );
        
        REQUIRE( pThreadLocal==pData );
    }
    
    class SomeType
    {
    public:
        void dummy()
        {
        }
    };
    

    
    SECTION("convertToRef")
    {
        BDN_STATIC_THREAD_LOCAL( SomeType, myData );
        
        SomeType& ref = myData;
        
        ref.dummy();
    }
    
    
    SECTION("callFunc")
    {
        BDN_STATIC_THREAD_LOCAL( SomeType, myData );
        
        ((SomeType&)myData).dummy();
        
        SomeType& ref = myData;
        
        ref.dummy();        
    }
    
    SECTION( "fromFunc" )
    {
        P<ThreadLocalTestData>& pA = getFromFuncTestThreadLocal();
        P<ThreadLocalTestData>& pB = getFromFuncTestThreadLocal();
        
        // should get the same object each time
        REQUIRE( &pA == &pB );
        
        // should be null initially
        REQUIRE( pA==nullptr );
        REQUIRE( pB==nullptr );
        
        pA = newObj<ThreadLocalTestData>();
        
        REQUIRE( pA == pB);
        
#if BDN_HAVE_THREADS
        
        P<ThreadLocalTestData>* ppFromThread = Thread::exec(
            []()
            {
                P<ThreadLocalTestData>& pC = getFromFuncTestThreadLocal();
                
                REQUIRE( pC==nullptr );
                return &pC;
            } ).get();
    
        REQUIRE( ppFromThread != &pA );
        
        REQUIRE( pA != nullptr );
#endif
    }

#if BDN_HAVE_THREADS

    SECTION("setGetReleaseOtherThread")
    {
		REQUIRE( pThreadLocal1==nullptr );
        
        int constructedBefore = ThreadLocalTestData::constructedCount;
        int destructedBefore = ThreadLocalTestData::destructedCount;
        
        
        auto threadResult = Thread::exec(   []()
                        {
                            P<ThreadLocalTestData> pData = newObj<ThreadLocalTestData>( 143 );
                            pThreadLocal1 = pData;
                            
                            Thread::sleepMillis(3000);
                            
                            // should still have the same pointer
                            REQUIRE( pThreadLocal1==pData );
                        } );
        
        Thread::sleepMillis(1000);

        // a new instance should have been created (but not yet destroyed)
        REQUIRE( ThreadLocalTestData::constructedCount == constructedBefore+1);
        REQUIRE( ThreadLocalTestData::destructedCount == destructedBefore);
        
        // in this thread the pointer should still be null
        REQUIRE( pThreadLocal1==nullptr );
        
        P<ThreadLocalTestData> pData = newObj<ThreadLocalTestData>(42);
        pThreadLocal1 = pData;
        
        REQUIRE( pThreadLocal1!=nullptr );
        
        // one more object should exist now
        REQUIRE( ThreadLocalTestData::constructedCount == constructedBefore+2);
        REQUIRE( ThreadLocalTestData::destructedCount == destructedBefore);
        
        // wait for the thread to end
        threadResult.get();
        
        // wait a little bit to make sure that the thread cleanup had time to run
        Thread::sleepMillis(2000);
        
        // now the object from the other thread should have been released
        REQUIRE( ThreadLocalTestData::constructedCount == constructedBefore+2);
        REQUIRE( ThreadLocalTestData::destructedCount == destructedBefore+1);
        
        // and ours should still be there
        REQUIRE( pThreadLocal1==pData );
    }
    
#endif
    
    SECTION("Many objects")
    {
        // here we simply test that there is no low limit to the number of thread local objects one can have
        
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p1 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p2 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p3 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p4 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p5 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p6 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p7 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p8 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p9 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p10 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p11 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p12 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p13 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p14 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p15 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p16 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p17 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p18 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p19 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p20 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p21 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p22 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p23 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p24 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p25 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p26 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p27 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p28 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p29 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p30 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p31 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p32 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p33 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p34 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p35 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p36 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p37 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p38 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p39 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p40 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p41 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p42 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p43 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p44 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p45 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p46 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p47 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p48 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p49 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p50 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p51 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p52 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p53 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p54 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p55 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p56 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p57 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p58 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p59 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p60 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p61 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p62 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p63 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p64 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p65 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p66 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p67 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p68 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p69 );
        BDN_STATIC_THREAD_LOCAL( P<ThreadLocalTestData>, p70 );
        
        
    }
    
    
    
    
}

