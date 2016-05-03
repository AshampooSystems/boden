#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/GlobalMessageWindow.h>

using namespace bdn;


class TestCallable : public Base, BDN_IMPLEMENTS ISimpleCallable
{
public:
	void call()
	{
		callCount++;

		END_ASYNC_TEST();
	}

	int		callCount = 0;
};

void testPostCall()
{
	struct TestObjects : public Base	
	{
		GlobalMessageWindow window;
		P<TestCallable>		pCallable = newObj<TestCallable>();		
	};

	P<TestObjects> pObjects = newObj<TestObjects>();
	
	pObjects->window.postCall( pObjects->pCallable );

	REQUIRE( pObjects->pCallable->callCount==0 );

	MAKE_ASYNC_TEST(pObjects);
}


void testGlobalInstance()
{
	// we simply test that we get a valid global instance
	P<GlobalMessageWindow> pWindow = GlobalMessageWindow::get();

	REQUIRE( pWindow!=nullptr );

	// must get the same object each time
	REQUIRE( GlobalMessageWindow::get() == pWindow);
}

TEST_CASE("GlobalMessageWindow")
{
	SECTION("postCall")
		testPostCall();

	SECTION("globalInstance")
		testGlobalInstance();
}