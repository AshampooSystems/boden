#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/testDispatcher.h>

#include <bdn/GenericDispatcher.h>

using namespace bdn;

// the generic dispatcher has to run in its own thread for our tests to work.
// So we cannot do this if threading is not supported.
#if BDN_HAVE_THREADS

TEST_CASE("GenericDispatcher")
{
    static P<GenericDispatcher> dispatcher = newObj<GenericDispatcher>();
    static P<Thread> thread = newObj<Thread>(newObj<GenericDispatcher::ThreadRunnable>(dispatcher));

    bool enableTimingTests = false;
#ifdef BDN_ENABLE_TIMING_TESTS
    enableTimingTests = true;
#endif

    bdn::test::testDispatcher(dispatcher, thread->getId(), enableTimingTests);

    SECTION("cleanup")
    {
        // this is a dummy section that is used to clean up the dispatcher and
        // thread we created.
        thread->stop(Thread::ExceptionIgnore);
        thread = nullptr;
        dispatcher = nullptr;
    }
}

#endif
