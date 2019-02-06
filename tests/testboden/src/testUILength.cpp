

#include <bdn/UILength.h>

#include <bdn/test.h>

#include <sstream>

using namespace bdn;

static void _verifyUILengthToString(double value, UILength::Unit unit, String expectedString)
{
    UILength len(value, unit);

    std::ostringstream str;
    str << len;

    REQUIRE(str.str() == expectedString);
}

TEST_CASE("UILength")
{
    SECTION("defaultConstruct")
    {
        UILength a;

        REQUIRE(a.unit == UILength::Unit::none);
        REQUIRE(a.value == 0.0);

        REQUIRE(a.isNone());
    }

    SECTION("isNone")
    {
        UILength a;

        REQUIRE(a.isNone());

        a.unit = UILength::Unit::dip;
        REQUIRE(!a.isNone());

        a.unit = UILength::Unit::sem;
        REQUIRE(!a.isNone());

        a.unit = UILength::Unit::em;
        REQUIRE(!a.isNone());

        a.unit = UILength::Unit::none;
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
        UILength a(12.3456);

        REQUIRE(a.unit == UILength::Unit::dip);
        REQUIRE(a.value == 12.3456);
    }

    SECTION("construct(int)")
    {
        UILength a(12);

        REQUIRE(a.unit == UILength::Unit::dip);
        REQUIRE(a.value == 12);
    }

    SECTION("construct(value, unit)")
    {
        UILength a(12.3456, UILength::Unit::sem);

        REQUIRE(a.unit == UILength::Unit::sem);
        REQUIRE(a.value == 12.3456);
    }

    SECTION("static construct helpers")
    {
        SECTION("none")
        {
            UILength a = UILength::none();

            REQUIRE(a.unit == UILength::Unit::none);
            REQUIRE(a.value == 0);

            REQUIRE(a.isNone());
        }

        SECTION("dip")
        {
            UILength a = UILength::dip(1.234);

            REQUIRE(a.unit == UILength::Unit::dip);
            REQUIRE(a.value == 1.234);

            REQUIRE(!a.isNone());
        }

        SECTION("sem")
        {
            UILength a = UILength::sem(1.234);

            REQUIRE(a.unit == UILength::Unit::sem);
            REQUIRE(a.value == 1.234);

            REQUIRE(!a.isNone());
        }

        SECTION("em")
        {
            UILength a = UILength::em(1.234);

            REQUIRE(a.unit == UILength::Unit::em);
            REQUIRE(a.value == 1.234);

            REQUIRE(!a.isNone());
        }
    }

    SECTION("equality")
    {
        UILength a(12.3456, UILength::Unit::dip);

        checkEquality(UILength(), UILength(), true);
        checkEquality(a, UILength(), false);
        checkEquality(a, UILength::dip(12.3456), true);
        checkEquality(a, UILength::dip(17), false);

        checkEquality(a, UILength::sem(12.3456), false);
        checkEquality(a, UILength::em(12.3456), false);
    }

    SECTION("toString")
    {
        _verifyUILengthToString(1.25, UILength::Unit::none, "none");
        _verifyUILengthToString(1.25, UILength::Unit::dip, "1.25 dip");
        _verifyUILengthToString(1.25, UILength::Unit::sem, "1.25 sem");
        _verifyUILengthToString(1.25, UILength::Unit::em, "1.25 em");
    }
}
