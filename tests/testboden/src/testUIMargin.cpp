

#include <bdn/UIMargin.h>

#include <bdn/test.h>

using namespace bdn;

TEST_CASE("UIMargin")
{
    SECTION("defaultConstruct")
    {
        UIMargin a;

        REQUIRE(a.top == UILength());
        REQUIRE(a.right == UILength());
        REQUIRE(a.bottom == UILength());
        REQUIRE(a.left == UILength());
    }

    SECTION("constructAllIndividuallyUILength")
    {
        UIMargin a(UILength::dip(1.1), UILength::sem(2.2), UILength::em(3.3), UILength::dip(4.4));

        REQUIRE(a.top == UILength::dip(1.1));
        REQUIRE(a.right == UILength::sem(2.2));
        REQUIRE(a.bottom == UILength::em(3.3));
        REQUIRE(a.left == UILength::dip(4.4));
    }

    SECTION("constructAllIndividuallyDouble")
    {
        UIMargin a(1.1, 2.2, 3.3, 4.4);

        REQUIRE(a.top == UILength::dip(1.1));
        REQUIRE(a.right == UILength::dip(2.2));
        REQUIRE(a.bottom == UILength::dip(3.3));
        REQUIRE(a.left == UILength::dip(4.4));
    }

    SECTION("constructAllIndividuallyInt")
    {
        UIMargin a(1, 2, 3, 4);

        REQUIRE(a.top == UILength::dip(1));
        REQUIRE(a.right == UILength::dip(2));
        REQUIRE(a.bottom == UILength::dip(3));
        REQUIRE(a.left == UILength::dip(4));
    }

    SECTION("constructTwoValues")
    {
        UIMargin a(1.1, 2.2);

        REQUIRE(a.top == UILength(1.1));
        REQUIRE(a.right == UILength(2.2));
        REQUIRE(a.bottom == UILength(1.1));
        REQUIRE(a.left == UILength(2.2));
    }

    SECTION("constructOneValueWithUnit")
    {
        UIMargin a(1.1);

        REQUIRE(a.top == UILength(1.1));
        REQUIRE(a.right == UILength(1.1));
        REQUIRE(a.bottom == UILength(1.1));
        REQUIRE(a.left == UILength(1.1));
    }

    SECTION("equality")
    {
        UIMargin a(1.1, 2.2, 3.3, 4.4);

        checkEquality(UIMargin(), UIMargin(), true);
        checkEquality(a, UIMargin(), false);
        checkEquality(a, UIMargin(1.1, 2.2, 3.3, 4.4), true);
        checkEquality(a, UIMargin(10.1, 2.2, 3.3, 4.4), false);
        checkEquality(a, UIMargin(1.1, 20.2, 3.3, 4.4), false);
        checkEquality(a, UIMargin(1.1, 2.2, 30.3, 4.4), false);
        checkEquality(a, UIMargin(1.1, 2.2, 3.3, 40.4), false);
    }

    SECTION("stream <<")
    {
        UIMargin m(1.125, -345.125, 2.775, 3.5);

        std::ostringstream str;
        str << m;

        REQUIRE(str.str() == "(1.125 dip, -345.125 dip, 2.775 dip, 3.5 dip)");
    }
}
