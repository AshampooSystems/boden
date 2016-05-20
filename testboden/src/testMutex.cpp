#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Thread.h>
#include <bdn/Mutex.h>
#include <bdn/StopWatch.h>

using namespace bdn;


void testMutex(bool recursive)
{
	Mutex mutex;

	bool  threadLocked = false;
	bool  threadUnlocked = false;

	mutex.lock();

	if(recursive)
	{
		// lock and unlock again
		mutex.lock();
		mutex.unlock();

		// should still be locked here.
	}

	Thread::exec(	[&mutex, &threadLocked, &threadUnlocked]()
					{
						mutex.lock();
						threadLocked = true;

						Thread::sleepMillis(2000);

						mutex.unlock();

						threadUnlocked = true;
					
					} );

	Thread::sleepMillis(1000);

	REQUIRE( !threadLocked );

	mutex.unlock();

	Thread::sleepMillis(1000);

	REQUIRE( threadLocked );
		
	StopWatch watch;

	mutex.lock();

	Thread::sleepMillis(500);

	REQUIRE( threadUnlocked );
	REQUIRE( watch.getMillis()>=800 );
	REQUIRE( watch.getMillis()<2000 );

	mutex.unlock();
		
}

TEST_CASE("Mutex")
{
	SECTION("lockUnlock")
		testMutex(false);

	SECTION("recursive")
		testMutex(true);

}


TEST_CASE("MutexLock")
{
	Mutex mutex;
	bool  threadLocked = false;

	{
		MutexLock lock(mutex);

		Thread::exec(	[&threadLocked, &mutex]()
						{
							mutex.lock();

							threadLocked = true;

							mutex.unlock();
						} );

		Thread::sleepMillis(1000);

		REQUIRE(!threadLocked);
	}

	Thread::sleepMillis(1000);

	REQUIRE( threadLocked );

	StopWatch watch;

	mutex.lock();

	REQUIRE( watch.getMillis()<1000 );
	mutex.unlock();
}


TEST_CASE("MutexUnlock")
{
	Mutex mutex;
	bool  threadLocked = false;
	bool  threadLocked2 = false;

	{
		MutexLock lock(mutex);

		Thread::exec(	[&threadLocked, &mutex]()
						{
							mutex.lock();

							threadLocked = true;

							mutex.unlock();
						} );

		Thread::sleepMillis(1000);

		REQUIRE(!threadLocked);

		StopWatch watch;

		{
			MutexUnlock unlock(mutex);

			Thread::sleepMillis(1000);

			REQUIRE( threadLocked );

			watch.start();
		}		

		REQUIRE( watch.getMillis()<1000 );

		// verify that the mutex has really been re-locked		
		
		Thread::exec(	[&threadLocked2, &mutex]()
						{
							mutex.lock();

							threadLocked2 = true;

							mutex.unlock();
						} );

		Thread::sleepMillis(1000);

		REQUIRE(!threadLocked2);		
	}	

	Thread::sleepMillis(1000);

	REQUIRE(threadLocked2);	

}

