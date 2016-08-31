#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/GlobalMessageWindow.h>

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
        CONTINUE_SECTION_AFTER_PENDING_EVENTS_WITH( [pCallable, pWindow](){ continueTestPostCall(pCallable, pWindow); } );
    }
}

void testGlobalInstance()
{
	// we simply test that we get a valid global instance
	P<GlobalMessageWindow> pWindow = &GlobalMessageWindow::get();

	REQUIRE( pWindow!=nullptr );

	// must get the same object each time
	REQUIRE( &GlobalMessageWindow::get() == pWindow);
}




TEST_CASE("GlobalMessageWindow")
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

	SECTION("globalInstance")
		testGlobalInstance();
}