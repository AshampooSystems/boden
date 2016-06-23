#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Margin.h>

using namespace bdn;

TEST_CASE("Margin")
{
	SECTION("defaultConstruct")
	{
		Margin a;

		REQUIRE( a.top == 0);
		REQUIRE( a.right == 0);
		REQUIRE( a.bottom == 0);
		REQUIRE( a.left == 0);
	}

	SECTION("constructAllIndividually")
	{
		Margin a(1,2,3,4);

		REQUIRE( a.top == 1);
		REQUIRE( a.right == 2);
		REQUIRE( a.bottom == 3);
		REQUIRE( a.left == 4);
	}

	SECTION("constructSingleVal")
	{
		Margin a(3);

		REQUIRE( a.top == 3);
		REQUIRE( a.right == 3);
		REQUIRE( a.bottom == 3);
		REQUIRE( a.left == 3);
	}

	SECTION("constructTwoVals")
	{
		Margin a(3, 4);

		REQUIRE( a.top == 3);
		REQUIRE( a.right == 4);
		REQUIRE( a.bottom == 3);
		REQUIRE( a.left == 4);
	}

	SECTION("equality")
	{
		Margin a(1,2,3,4);
		
		checkEquality( Margin(), Margin(), true );
		checkEquality( a, Margin(), false );
		checkEquality( a, Margin(1,2,3,4), true );
		checkEquality( a, Margin(10,2,3,4), false );
		checkEquality( a, Margin(1,20,3,4), false );
		checkEquality( a, Margin(1,2,30,4), false );
		checkEquality( a, Margin(1,2,3,40), false );
	}

}


