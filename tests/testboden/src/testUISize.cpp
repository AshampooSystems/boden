

#include <bdn/UISize.h>

#include <bdn/test.h>

using namespace bdn;

TEST_CASE("UISize")
{
    SECTION("defaultConstruct")
    {
        UISize a;

        REQUIRE(a.width == UILength());
        REQUIRE(a.height == UILength());
    }

    SECTION("constructUILength")
    {
        UISize a(UILength::em(1.1), UILength::sem(2.2));

        REQUIRE(a.width == UILength::em(1.1));
        REQUIRE(a.height == UILength::sem(2.2));
    }

    SECTION("constructDouble")
    {
        UISize a(1.1, 2.2);

        REQUIRE(a.width == UILength(1.1));
        REQUIRE(a.height == UILength(2.2));
    }

    SECTION("constructInt")
    {
        UISize a(1, 2);

        REQUIRE(a.width == UILength(1));
        REQUIRE(a.height == UILength(2));
    }

    SECTION("constructSize")
    {
        UISize a(Size(1.1, 2.2));

        REQUIRE(a.width == UILength(1.1));
        REQUIRE(a.height == UILength(2.2));
    }

    SECTION("constructUnitWidthHeight")
    {
        UISize a(UILength::em(1.1), UILength::sem(2.2));

        REQUIRE(a.width == UILength::em(1.1));
        REQUIRE(a.height == UILength::sem(2.2));
    }

    SECTION("equality")
    {
        UISize a(1.1, 2.2);

        checkEquality(UISize(), UISize(), true);
        checkEquality(a, UISize(), false);
        checkEquality(a, UISize(1.1, 2.2), true);
        checkEquality(a, UISize(10.1, 2.2), false);
        checkEquality(a, UISize(1.1, 20.2), false);
        checkEquality(a, UISize(UILength::sem(1.1), 2.2), false);
        checkEquality(a, UISize(1.1, UILength::sem(2.2)), false);
    }

    SECTION("stream <<")
    {
        UISize m(1.125, -345.125);

        std::ostringstream str;
        str << m;

        REQUIRE(str.str() == "(1.125 dip x -345.125 dip)");
    }
}
