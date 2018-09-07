#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/ThreadLocalStorage.h>

#include <bdn/Thread.h>

using namespace bdn;
using namespace bdn::win32;

class TlsTestData
{
  public:
    TlsTestData(int v = 17) { val = v; }

    int val;
};

class TlsTestData2 : public Base
{
  public:
    static int &getConstructed()
    {
        static int constructed = 0;

        return constructed;
    }

    static int &getDestructed()
    {
        static int destructed = 0;

        return destructed;
    }

    TlsTestData2(int v = 17)
    {
        val = v;
        getConstructed()++;
    }

    ~TlsTestData2() { getDestructed()++; }

    int val;
};

static ThreadLocalStorage<P<TlsTestData2>> pGlobalThreadLocal;

TEST_CASE("win32::ThreadLocalStorage")
{

    SECTION("initial value")
    {
        SECTION("withConstructor")
        {
            ThreadLocalStorage<TlsTestData> data;

            TlsTestData &ref = data;

            REQUIRE(ref.val == 17);
        }

        SECTION("int")
        {
            ThreadLocalStorage<int> data;

            REQUIRE(data == 0);
        }

        SECTION("bool")
        {
            ThreadLocalStorage<bool> data;

            REQUIRE(data == false);
        }

        SECTION("void*")
        {
            ThreadLocalStorage<void *> data;

            REQUIRE(data == nullptr);
        }
    }

    SECTION("setGet")
    {
        ThreadLocalStorage<TlsTestData> data;

        REQUIRE(((TlsTestData &)data).val == 17);

        data = TlsTestData(19);

        REQUIRE(((TlsTestData &)data).val == 19);
    }

#if BDN_HAVE_THREADS

    SECTION("setGetReleaseOtherThread")
    {
        int constructedBefore = TlsTestData2::getConstructed();
        int destructedBefore = TlsTestData2::getDestructed();

        auto threadResult = Thread::exec([]() {
            REQUIRE(pGlobalThreadLocal == nullptr);

            P<TlsTestData2> pData = newObj<TlsTestData2>(143);
            pGlobalThreadLocal = pData;

            Thread::sleepMillis(3000);

            // should still have the same pointer
            REQUIRE(pGlobalThreadLocal == pData);
        });

        Thread::sleepMillis(1000);

        // a new instance should have been created (but not yet destroyed)
        REQUIRE(TlsTestData2::getConstructed() == constructedBefore + 1);
        REQUIRE(TlsTestData2::getDestructed() == destructedBefore);

        // in this thread the pointer should still be null for this thread
        REQUIRE(pGlobalThreadLocal == nullptr);

        P<TlsTestData2> pData = newObj<TlsTestData2>(42);
        pGlobalThreadLocal = pData;

        REQUIRE(pGlobalThreadLocal != nullptr);

        // one more object should exist now
        REQUIRE(TlsTestData2::getConstructed() == constructedBefore + 2);
        REQUIRE(TlsTestData2::getDestructed() == destructedBefore);

        // wait for the thread to end
        threadResult.get();

        // wait a little bit to make sure that the thread cleanup had time to
        // run
        Thread::sleepMillis(2000);

        // now the object from the other thread should have been released
        REQUIRE(TlsTestData2::getConstructed() == constructedBefore + 2);
        REQUIRE(TlsTestData2::getDestructed() == destructedBefore + 1);

        // and ours should still be there
        REQUIRE(pGlobalThreadLocal == pData);
    }

#endif
}
