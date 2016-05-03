#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Thread.h>

using namespace bdn;

void verifySleep( std::function<void()> func, int expectedSleepMillis)
{
	auto startTime = std::chrono::system_clock::now();

	func();

	auto	duration = std::chrono::system_clock::now() - startTime;
	int64_t durationMillis = std::chrono::duration_cast< std::chrono::milliseconds >( duration ).count();

	REQUIRE( durationMillis>=expectedSleepMillis);	
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

TEST_CASE("Thread")
{
	SECTION("sleepSeconds")
		testSleepSeconds();

	SECTION("sleepMillis")
		testSleepMillis();

	SECTION("yield")
		testYield();
}


