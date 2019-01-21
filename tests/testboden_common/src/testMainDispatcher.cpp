
#include <bdn/test.h>

#include <bdn/test/testDispatcher.h>
#include <bdn/AppRunnerBase.h>
#include <bdn/config.h>

using namespace bdn;

TEST_CASE("mainDispatcher")
{
    bool canKeepRunningAfterUnhandledException = true;

    // iOS always exits after unhandled exceptions

    if (bdn::config::is_ios) {
        canKeepRunningAfterUnhandledException = false;
    }

    bool enableTimingTests = false;
#ifdef BDN_TESTBODEN_ENABLE_TIMING_TESTS
    enableTimingTests = true;
#endif

    bdn::test::testDispatcher(getMainDispatcher(), AppRunnerBase::mainThreadId(), enableTimingTests,
                              canKeepRunningAfterUnhandledException);
}
