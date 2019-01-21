
#include <bdn/test.h>
#include <bdn/test/testDispatcher.h>
#include <bdn/GenericDispatcher.h>
#include <bdn/config.h>

#include <thread>
#include <iostream>

using namespace bdn;

// the generic dispatcher has to run in its own thread for our tests to work.
// So we cannot do this if threading is not supported.
#if BDN_HAVE_THREADS

TEST_CASE("GenericDispatcher")
{
    static std::shared_ptr<GenericDispatcher> dispatcher = std::make_shared<GenericDispatcher>();
    static std::shared_ptr<GenericDispatcher::ThreadRunnable> dispatcherRunnable =
        std::make_shared<GenericDispatcher::ThreadRunnable>(dispatcher);

    std::shared_ptr<GenericDispatcher::ThreadRunnable> ptr = dispatcherRunnable;

    static std::thread thread([ptr]() { ptr->run(); });

    bool enableTimingTests = false;
#ifdef BDN_ENABLE_TIMING_TESTS
    enableTimingTests = true;
#endif

    bdn::test::testDispatcher(dispatcher, thread.get_id(), enableTimingTests);

    SECTION("cleanup")
    {
        // this is a dummy section that is used to clean up the dispatcher and
        // thread we created.
        dispatcherRunnable->signalStop();
        thread.join();
        dispatcher = nullptr;
    }
}

#endif
