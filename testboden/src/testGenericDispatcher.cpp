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
    static P<GenericDispatcher> pDispatcher = newObj<GenericDispatcher>();
    static P<Thread> pThread = newObj< Thread >( newObj<GenericDispatcher::ThreadRunnable>(pDispatcher) );
    
    bdn::test::testDispatcher( pDispatcher, pThread->getId() );

    SECTION("cleanup")
    {
        // this is a dummy section that is used to clean up the dispatcher and
        // thread we created.
        pThread->stop( Thread::ExceptionIgnore );
        pThread = nullptr;
        pDispatcher = nullptr;
    }
}

#endif


