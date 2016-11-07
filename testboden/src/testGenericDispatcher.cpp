#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/test/testDispatcher.h>

#include <bdn/GenericDispatcher.h>

using namespace bdn;



TEST_CASE("GenericDispatcher")
{
    static P<GenericDispatcher> pDispatcher = newObj<GenericDispatcher>();
    static P<Thread> pThread = newObj< Thread >( newObj<GenericDispatcher::ThreadRunnable>(pDispatcher) );
    
    bdn::test::testDispatcher( pDispatcher );

    SECTION("cleanup")
    {
        // this is a dummy section that is used to clean up the dispatcher and
        // thread we created.
        pThread->stop( Thread::ExceptionIgnore );
        pThread = nullptr;
        pDispatcher = nullptr;
    }
}


