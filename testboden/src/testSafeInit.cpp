#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Thread.h>

using namespace bdn;

static std::atomic<int> constructedCount(0);

class TestData : public Base
{
public:
	TestData()
	{
		val = 17;

		constructedCount++;
	}

	int val;
};

static TestData* getTestData1()
{
	static SafeInit<TestData> init;

	return init.get();
}

static TestData* getTestData2()
{
	static SafeInit<TestData> init;

	return init.get();
}

TEST_CASE("SafeInit")
{
	SECTION("initialized")
	{
		static SafeInit<TestData> init;

		REQUIRE( init.get()->val == 17);
	}

	SECTION("fromFunc")
	{
		// should be initialized
		TestData* pData = getTestData1();
		REQUIRE(pData->val == 17);

		// accessing again should give the same object
		REQUIRE( getTestData1() == pData );

		pData->val = 4;

		// should not be constructed again when accessing
		REQUIRE( getTestData1() == pData );
		REQUIRE(pData->val == 4);
	}	

#if BDN_HAVE_THREADS

	SECTION("simultaneous access")
	{
		int constructedBefore = constructedCount;

		const int threadCount = 100;

		std::vector< std::future<void> > threadResults;
		std::vector< P<Thread> > threads;

		for(int i=0;i<threadCount; i++)
			threadResults.emplace_back();

		for(int i=0;i<threadCount; i++)
		{
			threadResults[i] = Thread::exec(
					[]()
					{
						getTestData2();
					} );
		}

		// wait for all threads to finish
		for(std::future<void>& r: threadResults)
			r.get();

		// constructed should have executed exactly once
		REQUIRE( constructedCount == constructedBefore+1 );		
	}

#endif

}


