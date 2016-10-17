#include <bdn/init.h>

#include <bdn/UiSize.h>

#include <bdn/test.h>

using namespace bdn;

TEST_CASE("UiSize")
{
	SECTION("defaultConstruct")
	{
		UiSize a;

		REQUIRE( a.width == UiLength() );
        REQUIRE( a.height == UiLength() );
	}

	SECTION("constructUiLength")
	{
		UiSize a( UiLength(UiLength::Unit::dip, 1.1),
					UiLength(UiLength::Unit::sem, 2.2) );
			
		REQUIRE( a.width == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.height == UiLength(UiLength::Unit::sem, 2.2));
	}

    SECTION("constructSize")
	{
		UiSize a( Size(1.1, 2.2) );
			
		REQUIRE( a.width == UiLength(UiLength::Unit::dip, 1.1));
		REQUIRE( a.height == UiLength(UiLength::Unit::dip, 2.2));
	}

    SECTION("constructUnitWidthHeight")
	{
		UiSize a(UiLength::sem, 1.1, 2.2 );
			
		REQUIRE( a.width == UiLength(UiLength::Unit::sem, 1.1));
		REQUIRE( a.height == UiLength(UiLength::Unit::sem, 2.2));
	}

	
	SECTION("equality")
	{
		UiSize a( UiLength::Unit::dip, 1.1, 2.2 );
		
		checkEquality( UiSize(), UiSize(), true );
		checkEquality( a, UiSize(), false );
		checkEquality( a, UiSize(UiLength::Unit::dip, 1.1, 2.2 ), true );
		checkEquality( a, UiSize(UiLength::Unit::dip, 10.1, 2.2), false );
		checkEquality( a, UiSize(UiLength::Unit::dip, 1.1, 20.2 ), false );
		checkEquality( a, UiSize(UiLength::Unit::sem, 1.1, 2.2 ), false );
	}

}


