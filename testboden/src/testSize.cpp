#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Size.h>

using namespace bdn;

TEST_CASE("Size")
{
	SECTION("defaultConstruct")
	{
		Size s;

		REQUIRE( s.width == 0);
		REQUIRE( s.height == 0);
	}

	SECTION("constructWidthHeight")
	{
		Size s(10, 20);

		REQUIRE( s.width == 10);
		REQUIRE( s.height == 20);
	}

	SECTION("equality")
	{
		Size a(10, 20);
		
		checkEquality( Size(), Size(), true );
		checkEquality( a, Size(), false );
		checkEquality( a, Size(10, 20), true );
		checkEquality( a, Size(10, 19), false );
		checkEquality( a, Size(8, 20), false );
	}

	SECTION("+-Size")
	{
		Size a(10, 20);

		SECTION("+")
		{
			REQUIRE( a+Size(1,2) == Size(11, 22) );
			REQUIRE( a == Size(10, 20) );
		}

		SECTION("-")
		{
			REQUIRE( a-Size(1,2) == Size(9, 18) );
			REQUIRE( a == Size(10, 20) );
		}

		SECTION("+neg")
		{
			REQUIRE( a+Size(-5, -6) == Size(5, 14) );
			REQUIRE( a == Size(10, 20) );
		}

		SECTION("-neg")
		{
			REQUIRE( a-Size(-5, -6) == Size(15, 26) );
			REQUIRE( a == Size(10, 20) );
		}

		SECTION("+=")
		{
			a+=Size(1, 2);
			REQUIRE( a == Size(11, 22) );
		}

		SECTION("-=")
		{
			a-=Size(1, 2);
			REQUIRE( a == Size(9, 18) );
		}

		SECTION("+=neg")
		{
			a+=Size(-5, -6);
			REQUIRE( a == Size(5, 14) );
		}

		SECTION("-=neg")
		{
			a-=Size(-5, -6);
			REQUIRE( a == Size(15, 26) );
		}
	}


	SECTION("+-Margin")
	{
		Size a(10, 20);

		SECTION("+")
		{
			REQUIRE( a+Margin(1,2,3,4) == Size(16, 24) );
			REQUIRE( a == Size(10, 20) );
		}

		SECTION("+=")
		{
			a+=Margin(1,2,3,4);
			REQUIRE( a == Size(16, 24) );
		}

		SECTION("-")
		{
			REQUIRE( a-Margin(1,2,3,4) == Size(4, 16) );
			REQUIRE( a == Size(10, 20) );
		}

		SECTION("-=")
		{
			a-=Margin(1,2,3,4);
			REQUIRE( a == Size(4, 16) );
		}

		SECTION("+neg")
		{
			REQUIRE( a+Margin(-1,-2,-3,-4) == Size(4, 16) );
			REQUIRE( a == Size(10, 20) );
		}

		SECTION("+=neg")
		{
			a+=Margin(-1,-2,-3,-4);
			REQUIRE( a == Size(4, 16) );
		}

		SECTION("-neg")
		{
			REQUIRE( a-Margin(-1,-2,-3,-4) == Size(16, 24) );
			REQUIRE( a == Size(10, 20) );
		}
		
		SECTION("-=neg")
		{
			a-=Margin(-1,-2,-3,-4);
			REQUIRE( a == Size(16, 24) );
		}
	}

    SECTION("operator<")
    {
        Size a(10, 20);

        REQUIRE( ! (a < Size(9, 19)) );
        REQUIRE( ! (a < Size(9, 20)) );
        REQUIRE( ! (a < Size(9, 21)) );
        
        REQUIRE( ! (a < Size(10, 19)) );
        REQUIRE( ! (a < Size(10, 20)) );
        REQUIRE( ! (a < Size(10, 21)) );

        REQUIRE( ! (a < Size(11, 19)) );
        REQUIRE( ! (a < Size(11, 20)) );
        REQUIRE( a < Size(11, 21) );
    }

    SECTION("operator<=")
    {
        Size a(10, 20);

        REQUIRE( ! (a <= Size(9, 19)) );
        REQUIRE( ! (a <= Size(9, 20)) );
        REQUIRE( ! (a <= Size(9, 21)) );
        
        REQUIRE( ! (a <= Size(10, 19)) );
        REQUIRE( a <= Size(10, 20) );
        REQUIRE( a <= Size(10, 21) );

        REQUIRE( ! (a <= Size(11, 19)) );
        REQUIRE( a <= Size(11, 20) );
        REQUIRE( a <= Size(11, 21) );
    }

    SECTION("operator>")
    {
        Size a(10, 20);

        REQUIRE( a > Size(9, 19) );
        REQUIRE( ! (a > Size(9, 20)) );
        REQUIRE( ! (a > Size(9, 21)) );
        
        REQUIRE( ! (a > Size(10, 19)) );
        REQUIRE( ! (a > Size(10, 20)) );
        REQUIRE( ! (a > Size(10, 21)) );

        REQUIRE( ! (a > Size(11, 19)) );
        REQUIRE( ! (a > Size(11, 20)) );
        REQUIRE( ! (a > Size(11, 21)) );
    }

    SECTION("operator>=")
    {
        Size a(10, 20);

        REQUIRE( a >= Size(9, 19) );
        REQUIRE( a >= Size(9, 20) );
        REQUIRE( ! (a >= Size(9, 21)) );
        
        REQUIRE( a >= Size(10, 19) );
        REQUIRE( a >= Size(10, 20) );
        REQUIRE( ! (a >= Size(10, 21)) );

        REQUIRE( ! (a >= Size(11, 19)) );
        REQUIRE( ! (a >= Size(11, 20)) );
        REQUIRE( ! (a >= Size(11, 21)) );
    }
}


