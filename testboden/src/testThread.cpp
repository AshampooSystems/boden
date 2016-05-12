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


