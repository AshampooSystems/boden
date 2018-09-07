#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Thread.h>
#include <bdn/Array.h>

using namespace bdn;

static std::atomic<int> constructedCount(0);

class SafeInitTestData : public Base
{
  public:
    SafeInitTestData()
    {
        val = 17;

        constructedCount++;
    }

    int val;
};

static SafeInitTestData *getTestData1()
{
    static SafeInit<SafeInitTestData> init;

    return init.get();
}

static SafeInitTestData *getTestData2()
{
    static SafeInit<SafeInitTestData> init;

    return init.get();
}

TEST_CASE("SafeInit")
{
    SECTION("initialized")
    {
        static SafeInit<SafeInitTestData> init;

        REQUIRE(init.get()->val == 17);
    }

    SECTION("fromFunc")
    {
        // should be initialized
        SafeInitTestData *pData = getTestData1();
        REQUIRE(pData->val == 17);

        // accessing again should give the same object
        REQUIRE(getTestData1() == pData);

        pData->val = 4;

        // should not be constructed again when accessing
        REQUIRE(getTestData1() == pData);
        REQUIRE(pData->val == 4);
    }

#if BDN_HAVE_THREADS

    SECTION("simultaneous access")
    {
        int constructedBefore = constructedCount;

        const int threadCount = 100;

        Array<std::future<void>> threadResults;
        Array<P<Thread>> threads;

        for (int i = 0; i < threadCount; i++)
            threadResults.emplace_back();

        for (int i = 0; i < threadCount; i++) {
            threadResults[i] = Thread::exec([]() { getTestData2(); });
        }

        // wait for all threads to finish
        for (std::future<void> &r : threadResults)
            r.get();

        // constructed should have executed exactly once
        REQUIRE(constructedCount == constructedBefore + 1);
    }

#endif
}
