#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/GlobalMessageWindow.h>
#include <bdn/mainThread.h>

#include <bdn/StopWatch.h>

using namespace bdn;
using namespace bdn::win32;


class TestCallable : public Base, BDN_IMPLEMENTS ISimpleCallable
{
public:
	void call()
	{
		callCount++;
	}

	int		callCount = 0;
};

void continueTestPostCall(P<TestCallable> pCallable, P<GlobalMessageWindow> pWindow);

void testPostCall(P<GlobalMessageWindow> pWindow)
{
	P<TestCallable>			pCallable = newObj<TestCallable>();		
		
	pWindow->postCall( pCallable );

    if( Thread::isCurrentMain() )
    {
	    REQUIRE( pCallable->callCount==0 );
    }

    continueTestPostCall(pCallable, pWindow);
}

void continueTestPostCall(P<TestCallable> pCallable, P<GlobalMessageWindow> pWindow)
{
    if(pCallable->callCount==0)
    {
        CONTINUE_SECTION_WHEN_IDLE_WITH( [pCallable, pWindow](){ continueTestPostCall(pCallable, pWindow); } );
    }
}




void testCallOnceWhenIdle(P<GlobalMessageWindow> pWindow)
{
	P<TestCallable>			pCallable = newObj<TestCallable>();		
    P<TestCallable>			pCallable2 = newObj<TestCallable>();		
		
	pWindow->callOnceWhenIdle( pCallable );
    pWindow->callOnceWhenIdle( pCallable2 );

    CONTINUE_SECTION_IN_THREAD(pCallable, pCallable2, pWindow)
    {
        callFromMainThread(
            [pCallable, pCallable2, pWindow]()
            {
                // should not have been called yet
                REQUIRE( pCallable->callCount==0 );
                REQUIRE( pCallable2->callCount==0 );

                pWindow->notifyIdleBegun();

                // now each should have been called once

                REQUIRE( pCallable->callCount==1 );
                REQUIRE( pCallable2->callCount==1 );

                // notifying again should have no effect

                pWindow->notifyIdleBegun();

                REQUIRE( pCallable->callCount==1 );
                REQUIRE( pCallable2->callCount==1 );
            } ).get();
    };
}


void testGlobalInstance()
{
	// we simply test that we get a valid global instance
	P<GlobalMessageWindow> pWindow = &GlobalMessageWindow::get();

	REQUIRE( pWindow!=nullptr );

	// must get the same object each time
	REQUIRE( &GlobalMessageWindow::get() == pWindow);
}




TEST_CASE("win32.GlobalMessageWindow")
{
    P<GlobalMessageWindow>	            pWindow = newObj<GlobalMessageWindow>();

	SECTION("postCall")
    {
        SECTION("mainThread")
            testPostCall(pWindow);

        SECTION("otherThread")
        {
            CONTINUE_SECTION_IN_THREAD(pWindow)
            {
                testPostCall(pWindow);
            };
        }
    }

    SECTION("callOnceWhenIdle")
    {
        SECTION("mainThread")
            testCallOnceWhenIdle(pWindow);

        SECTION("otherThread")
        {
            CONTINUE_SECTION_IN_THREAD(pWindow)
            {
                testCallOnceWhenIdle(pWindow);
            };
        }
        
    }

	SECTION("globalInstance")
		testGlobalInstance();
}

