#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/testDispatcher.h>

using namespace bdn;


TEST_CASE("mainDispatcher")
{

    bool canKeepRunningAfterUnhandledException = true;
    
    // iOS always exits after unhandled exceptions
#if BDN_PLATFORM_IOS
    canKeepRunningAfterUnhandledException = false;
#endif
    

    bdn::test::testDispatcher( getMainDispatcher(), Thread::getMainId(), canKeepRunningAfterUnhandledException );
}




