#include <bdn/init.h>

#include <bdn/UiMargin.h>

#include <bdn/test.h>

using namespace bdn;

TEST_CASE("UiMargin")
{
	SECTION("defaultConstruct")
	{
		UiMargin a;

		REQUIRE( a.top == UiLength() );
		REQUIRE( a.right == UiLength() );
		REQUIRE( a.bottom ==  UiLength());
		REQUIRE( a.left ==  UiLength());
	}

	SECTION("constructAllIndividuallyUiLength")
	{
		UiMargin a( UiLength::dip(1.1),
					UiLength::sem(2.2),
					UiLength::em(3.3),
					UiLength::dip(4.4) );
			
		REQUIRE( a.top == UiLength::dip(1.1));
		REQUIRE( a.right == UiLength::sem(2.2));
		REQUIRE( a.bottom == UiLength::em(3.3));
		REQUIRE( a.left == UiLength::dip(4.4));
	}

    SECTION("constructAllIndividuallyDouble")
	{
		UiMargin a( 1.1,
					2.2,
					3.3,
					4.4 );
			
		REQUIRE( a.top == UiLength::dip(1.1));
		REQUIRE( a.right == UiLength::dip(2.2));
		REQUIRE( a.bottom == UiLength::dip(3.3));
		REQUIRE( a.left == UiLength::dip(4.4));
    }
	

    SECTION("constructAllIndividuallyInt")
	{
		UiMargin a( 1,
					2,
					3,
					4 );
			
		REQUIRE( a.top == UiLength::dip(1));
		REQUIRE( a.right == UiLength::dip(2));
		REQUIRE( a.bottom == UiLength::dip(3));
		REQUIRE( a.left == UiLength::dip(4));
	}

	SECTION("constructTwoValues")
	{
		UiMargin a( 1.1, 2.2 );

		REQUIRE( a.top == UiLength(1.1));
		REQUIRE( a.right == UiLength(2.2));
		REQUIRE( a.bottom == UiLength(1.1));
		REQUIRE( a.left == UiLength(2.2));
	}

	SECTION("constructOneValueWithUnit")
	{
		UiMargin a( 1.1 );

		REQUIRE( a.top == UiLength(1.1));
		REQUIRE( a.right == UiLength(1.1));
		REQUIRE( a.bottom == UiLength(1.1));
		REQUIRE( a.left == UiLength(1.1));
	}
	
	SECTION("equality")
	{
		UiMargin a( 1.1, 2.2, 3.3, 4.4 );
		
		checkEquality( UiMargin(), UiMargin(), true );
		checkEquality( a, UiMargin(), false );
		checkEquality( a, UiMargin(1.1, 2.2, 3.3, 4.4 ), true );
		checkEquality( a, UiMargin(10.1, 2.2, 3.3, 4.4 ), false );
		checkEquality( a, UiMargin(1.1, 20.2, 3.3, 4.4 ), false );
		checkEquality( a, UiMargin(1.1, 2.2, 30.3, 4.4 ), false );
		checkEquality( a, UiMargin(1.1, 2.2, 3.3, 40.4 ), false );
	}


	
	SECTION("toString")
	{
		UiMargin m( 1.125, -345.125, 2.775, 3.5 );

		SECTION("method")
			REQUIRE( m.toString() == "1.125 dip, -345.125 dip, 2.775 dip, 3.5 dip" );

		SECTION("global function")
			REQUIRE( toString(m) == "1.125 dip, -345.125 dip, 2.775 dip, 3.5 dip" );
	}

}


