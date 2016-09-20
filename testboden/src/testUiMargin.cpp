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
		UiMargin a( UiLength(UiLength::Unit::dip, 1.1),
					UiLength(UiLength::Unit::sem, 2.2),
					UiLength(UiLength::Unit::sem, 3.3),
					UiLength(UiLength::Unit::dip, 4.4) );
			
		REQUIRE( a.top == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.right == UiLength(UiLength::Unit::sem, 2.2));
		REQUIRE( a.bottom == UiLength(UiLength::Unit::sem, 3.3));
		REQUIRE( a.left == UiLength(UiLength::Unit::dip, 4.4));
	}

	SECTION("constructAllIndividuallyWithUnit")
	{
		UiMargin a( UiLength::Unit::dip, 1.1, 2.2, 3.3, 4.4 );

		REQUIRE( a.top == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.right == UiLength(UiLength::Unit::dip, 2.2));
		REQUIRE( a.bottom == UiLength(UiLength::Unit::dip, 3.3));
		REQUIRE( a.left == UiLength(UiLength::Unit::dip, 4.4));
	}

	SECTION("constructTwoValuesWithUnit")
	{
		UiMargin a( UiLength::Unit::dip, 1.1, 2.2 );

		REQUIRE( a.top == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.right == UiLength(UiLength::Unit::dip, 2.2));
		REQUIRE( a.bottom == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.left == UiLength(UiLength::Unit::dip, 2.2));
	}

	SECTION("constructOneValueWithUnit")
	{
		UiMargin a( UiLength::Unit::dip, 1.1 );

		REQUIRE( a.top == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.right == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.bottom == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.left == UiLength(UiLength::Unit::dip, 1.1));
	}
	
	SECTION("equality")
	{
		UiMargin a( UiLength::Unit::dip, 1.1, 2.2, 3.3, 4.4 );
		
		checkEquality( UiMargin(), UiMargin(), true );
		checkEquality( a, UiMargin(), false );
		checkEquality( a, UiMargin(UiLength::Unit::dip, 1.1, 2.2, 3.3, 4.4 ), true );
		checkEquality( a, UiMargin(UiLength::Unit::dip, 10.1, 2.2, 3.3, 4.4 ), false );
		checkEquality( a, UiMargin(UiLength::Unit::dip, 1.1, 20.2, 3.3, 4.4 ), false );
		checkEquality( a, UiMargin(UiLength::Unit::dip, 1.1, 2.2, 30.3, 4.4 ), false );
		checkEquality( a, UiMargin(UiLength::Unit::dip, 1.1, 2.2, 3.3, 40.4 ), false );
	}

}


