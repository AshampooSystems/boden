#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Point.h>

using namespace bdn;

TEST_CASE("Point")
{
	SECTION("defaultConstruct")
	{
		Point p;

		REQUIRE( p.x == 0);
		REQUIRE( p.y == 0);
	}

	SECTION("constructXY")
	{
		Point p(10, 20);
        
        REQUIRE( p.x == 10);
		REQUIRE( p.y == 20);
	}

	SECTION("equality")
	{
		Point p(10, 20);
		
		checkEquality( Point(), Point(), true );
		checkEquality( p, Point(), false );
		checkEquality( p, Point(10, 20), true );
		checkEquality( p, Point(10, 19), false );
		checkEquality( p, Point(8, 20), false );
	}

}


