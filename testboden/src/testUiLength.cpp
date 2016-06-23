#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/UiLength.h>

using namespace bdn;

TEST_CASE("UiLength")
{
	SECTION("defaultConstruct")
	{
		UiLength a;

		REQUIRE( a.unit == UiLength::Unit::sem );
		REQUIRE( a.value == 0.0 );
	}

	SECTION("constructUnitValue")
	{
		UiLength a(UiLength::Unit::pixel96, 12.3456 );

		REQUIRE( a.unit == UiLength::Unit::pixel96 );
		REQUIRE( a.value == 12.3456);
	}

	SECTION("equality")
	{
		UiLength a(UiLength::Unit::pixel96, 12.3456 );
		
		checkEquality( UiLength(), UiLength(), true );
		checkEquality( a, UiLength(), false );
		checkEquality( a, UiLength(UiLength::Unit::pixel96, 12.3456), true );
		checkEquality( a, UiLength(UiLength::Unit::realPixel, 12.3456), false );
		checkEquality( a, UiLength(UiLength::Unit::pixel96, 17), false );
	}

}


