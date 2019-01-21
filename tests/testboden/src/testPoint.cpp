
#include <bdn/test.h>

#include <bdn/Point.h>

using namespace bdn;

TEST_CASE("Point")
{
    SECTION("defaultConstruct")
    {
        Point p;

        REQUIRE(p.x == 0);
        REQUIRE(p.y == 0);
    }

    SECTION("constructXY")
    {
        Point p(10, 20);

        REQUIRE(p.x == 10);
        REQUIRE(p.y == 20);
    }

    SECTION("equality")
    {
        Point p(10, 20);

        checkEquality(Point(), Point(), true);
        checkEquality(p, Point(), false);
        checkEquality(p, Point(10, 20), true);
        checkEquality(p, Point(10, 19), false);
        checkEquality(p, Point(8, 20), false);
    }

    SECTION("+-Point")
    {
        Point a(10, 20);

        SECTION("+")
        {
            REQUIRE(a + Point(1, 2) == Point(11, 22));
            REQUIRE(a == Point(10, 20));
        }

        SECTION("-")
        {
            REQUIRE(a - Point(1, 2) == Point(9, 18));
            REQUIRE(a == Point(10, 20));
        }

        SECTION("+neg")
        {
            REQUIRE(a + Point(-5, -6) == Point(5, 14));
            REQUIRE(a == Point(10, 20));
        }

        SECTION("-neg")
        {
            REQUIRE(a - Point(-5, -6) == Point(15, 26));
            REQUIRE(a == Point(10, 20));
        }

        SECTION("+=")
        {
            a += Point(1, 2);
            REQUIRE(a == Point(11, 22));
        }

        SECTION("-=")
        {
            a -= Point(1, 2);
            REQUIRE(a == Point(9, 18));
        }

        SECTION("+=neg")
        {
            a += Point(-5, -6);
            REQUIRE(a == Point(5, 14));
        }

        SECTION("-=neg")
        {
            a -= Point(-5, -6);
            REQUIRE(a == Point(15, 26));
        }
    }

    SECTION("operator<")
    {
        Point a(10, 20);

        REQUIRE(!(a < Point(9, 19)));
        REQUIRE(!(a < Point(9, 20)));
        REQUIRE(!(a < Point(9, 21)));

        REQUIRE(!(a < Point(10, 19)));
        REQUIRE(!(a < Point(10, 20)));
        REQUIRE(!(a < Point(10, 21)));

        REQUIRE(!(a < Point(11, 19)));
        REQUIRE(!(a < Point(11, 20)));
        REQUIRE(a < Point(11, 21));
    }

    SECTION("operator<=")
    {
        Point a(10, 20);

        REQUIRE(!(a <= Point(9, 19)));
        REQUIRE(!(a <= Point(9, 20)));
        REQUIRE(!(a <= Point(9, 21)));

        REQUIRE(!(a <= Point(10, 19)));
        REQUIRE(a <= Point(10, 20));
        REQUIRE(a <= Point(10, 21));

        REQUIRE(!(a <= Point(11, 19)));
        REQUIRE(a <= Point(11, 20));
        REQUIRE(a <= Point(11, 21));
    }

    SECTION("operator>")
    {
        Point a(10, 20);

        REQUIRE(a > Point(9, 19));
        REQUIRE(!(a > Point(9, 20)));
        REQUIRE(!(a > Point(9, 21)));

        REQUIRE(!(a > Point(10, 19)));
        REQUIRE(!(a > Point(10, 20)));
        REQUIRE(!(a > Point(10, 21)));

        REQUIRE(!(a > Point(11, 19)));
        REQUIRE(!(a > Point(11, 20)));
        REQUIRE(!(a > Point(11, 21)));
    }

    SECTION("operator>=")
    {
        Point a(10, 20);

        REQUIRE(a >= Point(9, 19));
        REQUIRE(a >= Point(9, 20));
        REQUIRE(!(a >= Point(9, 21)));

        REQUIRE(a >= Point(10, 19));
        REQUIRE(a >= Point(10, 20));
        REQUIRE(!(a >= Point(10, 21)));

        REQUIRE(!(a >= Point(11, 19)));
        REQUIRE(!(a >= Point(11, 20)));
        REQUIRE(!(a >= Point(11, 21)));
    }

    /*
     * TODO
     * SECTION("toString")
    {
        Point p(1.125, -345.125);

        REQUIRE(toString(p) == "(1.125, -345.125)");
    }*/
}
