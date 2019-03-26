
#include <bdn/test.h>

#include <bdn/AppRunnerBase.h>
#include <bdn/config.h>
#include <bdn/test/testDispatcher.h>

using namespace bdn;

TEST_CASE("mainDispatcher")
{
    bool enableTimingTests = false;
#ifdef BDN_TESTBODEN_ENABLE_TIMING_TESTS
    enableTimingTests = true;
#endif

    bdn::test::testDispatcher(getMainDispatcher(), AppRunnerBase::mainThreadId(), enableTimingTests);
}
