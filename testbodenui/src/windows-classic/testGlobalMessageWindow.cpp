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

		if(throwException)
			throw std::exception("test");
	}

	int		callCount = 0;
	bool	throwException = false;
};

void testPostCall()
{
	GlobalMessageWindow window;

	P<TestCallable> pCallable = newObj<TestCallable>();

	window.postCall(pCallable);

	REQUIRE( pCallable->callCount==0 );

	BEGIN_UI_TEST(pCallable);
}

TEST_CASE("GlobalMessageWindow")
{
	SECTION("postCall")
		testPostCall();
}