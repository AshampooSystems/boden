#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/testDispatcher.h>

using namespace bdn;

TEST_CASE("mainDispatcher")
{

    bool canKeepRunningAfterUnhandledException = true;

    // iOS always exits after unhandled exceptions
#ifdef BDN_PLATFORM_IOS
    canKeepRunningAfterUnhandledException = false;
#endif

    bool enableTimingTests = false;
#ifdef BDN_TESTBODEN_ENABLE_TIMING_TESTS
    enableTimingTests = true;
#endif

    bdn::test::testDispatcher(getMainDispatcher(), Thread::getMainId(), enableTimingTests,
                              canKeepRunningAfterUnhandledException);
}
