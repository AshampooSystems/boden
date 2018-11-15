#include <bdn/init.h>

#include <bdn/UiLength.h>

#include <bdn/test.h>

using namespace bdn;

static void _verifyUiLengthToString(double value, UiLength::Unit unit, String expectedString)
{
    UiLength len(value, unit);

    REQUIRE(toString(len) == expectedString);
}

TEST_CASE("UiLength")
{
    SECTION("defaultConstruct")
    {
        UiLength a;

        REQUIRE(a.unit == UiLength::Unit::none);
        REQUIRE(a.value == 0.0);

        REQUIRE(a.isNone());
    }

    SECTION("isNone")
    {
        UiLength a;

        REQUIRE(a.isNone());

        a.unit = UiLength::Unit::dip;
        REQUIRE(!a.isNone());

        a.unit = UiLength::Unit::sem;
        REQUIRE(!a.isNone());

        a.unit = UiLength::Unit::em;
        REQUIRE(!a.isNone());

        a.unit = UiLength::Unit::none;
        REQUIRE(a.isNone());

        // value should not matter.

        a.value = 1;
        REQUIRE(a.isNone());

        a.value = -1;
        REQUIRE(a.isNone());

        a.value = 0;
        REQUIRE(a.isNone());
    }

    SECTION("construct(double)")
    {
        UiLength a(12.3456);

        REQUIRE(a.unit == UiLength::Unit::dip);
        REQUIRE(a.value == 12.3456);
    }

    SECTION("construct(int)")
    {
        UiLength a(12);

        REQUIRE(a.unit == UiLength::Unit::dip);
        REQUIRE(a.value == 12);
    }

    SECTION("construct(value, unit)")
    {
        UiLength a(12.3456, UiLength::Unit::sem);

        REQUIRE(a.unit == UiLength::Unit::sem);
        REQUIRE(a.value == 12.3456);
    }

    SECTION("static construct helpers")
    {
        SECTION("none")
        {
            UiLength a = UiLength::none();

            REQUIRE(a.unit == UiLength::Unit::none);
            REQUIRE(a.value == 0);

            REQUIRE(a.isNone());
        }

        SECTION("dip")
        {
            UiLength a = UiLength::dip(1.234);

            REQUIRE(a.unit == UiLength::Unit::dip);
            REQUIRE(a.value == 1.234);

            REQUIRE(!a.isNone());
        }

        SECTION("sem")
        {
            UiLength a = UiLength::sem(1.234);

            REQUIRE(a.unit == UiLength::Unit::sem);
            REQUIRE(a.value == 1.234);

            REQUIRE(!a.isNone());
        }

        SECTION("em")
        {
            UiLength a = UiLength::em(1.234);

            REQUIRE(a.unit == UiLength::Unit::em);
            REQUIRE(a.value == 1.234);

            REQUIRE(!a.isNone());
        }
    }

    SECTION("equality")
    {
        UiLength a(12.3456, UiLength::Unit::dip);

        checkEquality(UiLength(), UiLength(), true);
        checkEquality(a, UiLength(), false);
        checkEquality(a, UiLength::dip(12.3456), true);
        checkEquality(a, UiLength::dip(17), false);

        checkEquality(a, UiLength::sem(12.3456), false);
        checkEquality(a, UiLength::em(12.3456), false);
    }

    SECTION("toString")
    {
        _verifyUiLengthToString(1.25, UiLength::Unit::none, "none");
        _verifyUiLengthToString(1.25, UiLength::Unit::dip, "1.25 dip");
        _verifyUiLengthToString(1.25, UiLength::Unit::sem, "1.25 sem");
        _verifyUiLengthToString(1.25, UiLength::Unit::em, "1.25 em");
    }
}
