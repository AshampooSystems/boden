#include <bdn/init.h>

#include <bdn/UiSize.h>

#include <bdn/test.h>

using namespace bdn;

TEST_CASE("UiSize")
{
    SECTION("defaultConstruct")
    {
        UiSize a;

        REQUIRE(a.width == UiLength());
        REQUIRE(a.height == UiLength());
    }

    SECTION("constructUiLength")
    {
        UiSize a(UiLength::em(1.1), UiLength::sem(2.2));

        REQUIRE(a.width == UiLength::em(1.1));
        REQUIRE(a.height == UiLength::sem(2.2));
    }

    SECTION("constructDouble")
    {
        UiSize a(1.1, 2.2);

        REQUIRE(a.width == UiLength(1.1));
        REQUIRE(a.height == UiLength(2.2));
    }

    SECTION("constructInt")
    {
        UiSize a(1, 2);

        REQUIRE(a.width == UiLength(1));
        REQUIRE(a.height == UiLength(2));
    }

    SECTION("constructSize")
    {
        UiSize a(Size(1.1, 2.2));

        REQUIRE(a.width == UiLength(1.1));
        REQUIRE(a.height == UiLength(2.2));
    }

    SECTION("constructUnitWidthHeight")
    {
        UiSize a(UiLength::em(1.1), UiLength::sem(2.2));

        REQUIRE(a.width == UiLength::em(1.1));
        REQUIRE(a.height == UiLength::sem(2.2));
    }

    SECTION("equality")
    {
        UiSize a(1.1, 2.2);

        checkEquality(UiSize(), UiSize(), true);
        checkEquality(a, UiSize(), false);
        checkEquality(a, UiSize(1.1, 2.2), true);
        checkEquality(a, UiSize(10.1, 2.2), false);
        checkEquality(a, UiSize(1.1, 20.2), false);
        checkEquality(a, UiSize(UiLength::sem(1.1), 2.2), false);
        checkEquality(a, UiSize(1.1, UiLength::sem(2.2)), false);
    }

    SECTION("stream <<")
    {
        UiSize m(1.125, -345.125);

        REQUIRE(toString(m) == "(1.125 dip x -345.125 dip)");
    }
}
