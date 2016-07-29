#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/mainThread.h>

#include <chrono>

using namespace bdn;

class TimeoutChecker : public Base
{
public:
	TimeoutChecker()
	{		
		_pAsyncData = newObj<AsyncData>();

		_pAsyncData->startTime = std::chrono::system_clock::now();
	}

	~TimeoutChecker()
	{
		// we got destructed. I.e. the async test was ended. Abort any pending
		// async calls that might still be queued.
		_pAsyncData->aborted = true;
	}

	void scheduleNextCheck()
	{
		scheduleNextCheckInternal(_pAsyncData);
	}
	

private:
	struct AsyncData : public Base
	{
		std::chrono::time_point<std::chrono::system_clock> startTime;
		mutable bool aborted = false;
	};

	static void scheduleNextCheckInternal(AsyncData* pAsyncData)
	{
		// we want the lambda to hold a reference to the async data
		// to ensure that it is not destroyed in the meantime.
		P<AsyncData> pAsyncDataPtr = pAsyncData;

		asyncCallFromMainThread( [pAsyncDataPtr](){ TimeoutChecker::doCheck(pAsyncDataPtr); } );
	}

	static void doCheck(AsyncData* pAsyncData)
	{
		if(pAsyncData->aborted)
			return;

		auto	now = std::chrono::system_clock::now();;

		auto	elapsed = now-pAsyncData->startTime;
		double	elapsedSeconds = static_cast<double>( std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() );

		
		// If elapsedSeconds is longer than 15 then this is an error.
		// We should have had a timeout at 5 seconds!
		if(elapsedSeconds>=15)
		{
			// the test "failed". However, it is marked as "shouldFail", since we expect a timeout.
			// I.e. the result will be inverted. So we should just end here. That will mark it "passed"
			// at first, which will be inverted to failed when the shouldFail tag is processed. So the 
			// end result is fail.
			END_ASYNC_TEST();			
		}
		else
		{
			// always sleep a little to ensure that we do not hog all processing time
			Thread::sleepSeconds(1);

			scheduleNextCheckInternal(pAsyncData);
		}
	}
	
	P<AsyncData> _pAsyncData;
};


TEST_CASE("AsyncTest Timeout", "[!shouldfail]")
{
	// we test here that the async test timeout works.

	P<TimeoutChecker> pChecker = newObj<TimeoutChecker>();

	pChecker->scheduleNextCheck();
	
	MAKE_ASYNC_TEST(5, pChecker);
}



