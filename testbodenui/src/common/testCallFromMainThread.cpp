#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/mainThread.h>

#include <chrono>

using namespace bdn;

TEST_CASE("callFromMainThread")
{
	SECTION("mainThread")
	{
		int callCount = 0;

		std::future<int> result = callFromMainThread( [&callCount](int x){ callCount++; return x*2; }, 42 );

		// should have been called immediately, since we are currently in the main thread
		REQUIRE( callCount==1 );

		REQUIRE( result.wait_for( std::chrono::milliseconds::duration(0)) == std::future_status::ready  );

		REQUIRE( result.get()==84 );
	}

	SECTION("otherThreadNotStoringFuture")
	{
		auto startTime = std::chrono::system_clock::now();

		std::async(
			std::launch::async,
			[]()
			{
				volatile int callCount = 0;

				std::future<int> result = callFromMainThread(
					[&callCount](int x)
					{
						// sleep a little to ensure that we have time to check callCount
						Thread::sleepSeconds(1);
						callCount++;
						return x*2;
					},
					42 );


				// should NOT have been called immediately, since we are in a different thread.
				// Instead the call should have been deferred to the main thread.
				REQUIRE( callCount==0 );

				auto waitStart = std::chrono::system_clock::now();

				// REQUIRE( result.wait_for( std::chrono::milliseconds::duration(5000) ) == std::future_status::ready );
				// XXX
				result.wait();

				auto	waitDuration = std::chrono::system_clock::now() - waitStart;
				int64_t waitMillis = std::chrono::duration_cast<std::chrono::milliseconds>( waitDuration ).count();

				REQUIRE( waitMillis>=500 );
				REQUIRE( waitMillis<=5500 );

				REQUIRE( callCount==1 );

				REQUIRE( result.get()==84 );			
			} );

		auto	threadStartDuration = std::chrono::system_clock::now() - startTime;
		int64_t threadStartMillis = std::chrono::duration_cast<std::chrono::milliseconds>( threadStartDuration ).count();

		REQUIRE( threadStartMillis<=2000 );
		
		MAKE_ASYNC_TEST(10);
	}
}

