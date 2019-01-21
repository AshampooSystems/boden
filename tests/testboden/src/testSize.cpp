
#include <bdn/test.h>

#include <bdn/Size.h>

using namespace bdn;

TEST_CASE("Size")
{
    SECTION("defaultConstruct")
    {
        Size s;

        REQUIRE(s.width == 0);
        REQUIRE(s.height == 0);
    }

    SECTION("constructWidthHeight")
    {
        Size s(10, 20);

        REQUIRE(s.width == 10);
        REQUIRE(s.height == 20);
    }

    SECTION("equality")
    {
        Size a(10, 20);

        checkEquality(Size(), Size(), true);
        checkEquality(a, Size(), false);
        checkEquality(a, Size(10, 20), true);
        checkEquality(a, Size(10, 19), false);
        checkEquality(a, Size(8, 20), false);
    }

    SECTION("+-Size")
    {
        Size a(10, 20);

        SECTION("+")
        {
            REQUIRE(a + Size(1, 2) == Size(11, 22));
            REQUIRE(a == Size(10, 20));
        }

        SECTION("-")
        {
            REQUIRE(a - Size(1, 2) == Size(9, 18));
            REQUIRE(a == Size(10, 20));
        }

        SECTION("+neg")
        {
            REQUIRE(a + Size(-5, -6) == Size(5, 14));
            REQUIRE(a == Size(10, 20));
        }

        SECTION("-neg")
        {
            REQUIRE(a - Size(-5, -6) == Size(15, 26));
            REQUIRE(a == Size(10, 20));
        }

        SECTION("+=")
        {
            a += Size(1, 2);
            REQUIRE(a == Size(11, 22));
        }

        SECTION("-=")
        {
            a -= Size(1, 2);
            REQUIRE(a == Size(9, 18));
        }

        SECTION("+=neg")
        {
            a += Size(-5, -6);
            REQUIRE(a == Size(5, 14));
        }

        SECTION("-=neg")
        {
            a -= Size(-5, -6);
            REQUIRE(a == Size(15, 26));
        }
    }

    SECTION("+-Margin")
    {
        Size a(10, 20);

        SECTION("+")
        {
            REQUIRE(a + Margin(1, 2, 3, 4) == Size(16, 24));
            REQUIRE(a == Size(10, 20));
        }

        SECTION("+=")
        {
            a += Margin(1, 2, 3, 4);
            REQUIRE(a == Size(16, 24));
        }

        SECTION("-")
        {
            REQUIRE(a - Margin(1, 2, 3, 4) == Size(4, 16));
            REQUIRE(a == Size(10, 20));
        }

        SECTION("-=")
        {
            a -= Margin(1, 2, 3, 4);
            REQUIRE(a == Size(4, 16));
        }

        SECTION("+neg")
        {
            REQUIRE(a + Margin(-1, -2, -3, -4) == Size(4, 16));
            REQUIRE(a == Size(10, 20));
        }

        SECTION("+=neg")
        {
            a += Margin(-1, -2, -3, -4);
            REQUIRE(a == Size(4, 16));
        }

        SECTION("-neg")
        {
            REQUIRE(a - Margin(-1, -2, -3, -4) == Size(16, 24));
            REQUIRE(a == Size(10, 20));
        }

        SECTION("-=neg")
        {
            a -= Margin(-1, -2, -3, -4);
            REQUIRE(a == Size(16, 24));
        }
    }

    SECTION("operator<")
    {
        Size a(10, 20);

        REQUIRE(!(a < Size(9, 19)));
        REQUIRE(!(a < Size(9, 20)));
        REQUIRE(!(a < Size(9, 21)));

        REQUIRE(!(a < Size(10, 19)));
        REQUIRE(!(a < Size(10, 20)));
        REQUIRE(!(a < Size(10, 21)));

        REQUIRE(!(a < Size(11, 19)));
        REQUIRE(!(a < Size(11, 20)));
        REQUIRE(a < Size(11, 21));
    }

    SECTION("operator<=")
    {
        Size a(10, 20);

        REQUIRE(!(a <= Size(9, 19)));
        REQUIRE(!(a <= Size(9, 20)));
        REQUIRE(!(a <= Size(9, 21)));

        REQUIRE(!(a <= Size(10, 19)));
        REQUIRE(a <= Size(10, 20));
        REQUIRE(a <= Size(10, 21));

        REQUIRE(!(a <= Size(11, 19)));
        REQUIRE(a <= Size(11, 20));
        REQUIRE(a <= Size(11, 21));
    }

    SECTION("operator>")
    {
        Size a(10, 20);

        REQUIRE(a > Size(9, 19));
        REQUIRE(!(a > Size(9, 20)));
        REQUIRE(!(a > Size(9, 21)));

        REQUIRE(!(a > Size(10, 19)));
        REQUIRE(!(a > Size(10, 20)));
        REQUIRE(!(a > Size(10, 21)));

        REQUIRE(!(a > Size(11, 19)));
        REQUIRE(!(a > Size(11, 20)));
        REQUIRE(!(a > Size(11, 21)));
    }

    SECTION("operator>=")
    {
        Size a(10, 20);

        REQUIRE(a >= Size(9, 19));
        REQUIRE(a >= Size(9, 20));
        REQUIRE(!(a >= Size(9, 21)));

        REQUIRE(a >= Size(10, 19));
        REQUIRE(a >= Size(10, 20));
        REQUIRE(!(a >= Size(10, 21)));

        REQUIRE(!(a >= Size(11, 19)));
        REQUIRE(!(a >= Size(11, 20)));
        REQUIRE(!(a >= Size(11, 21)));
    }

    SECTION("applyMinimum")
    {
        SECTION("to normal size")
        {
            Size a(10, 20);

            SECTION("width unlimited, height unlimited")
            {
                a.applyMinimum(Size::none());

                // should have no effect
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width bigger, height unlimited")
            {
                a.applyMinimum(Size(11, Size::componentNone()));
                REQUIRE(a == Size(11, 20));
            }

            SECTION("width equal, height unlimited")
            {
                a.applyMinimum(Size(10, Size::componentNone()));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width smaller, height unlimited")
            {
                a.applyMinimum(Size(9, Size::componentNone()));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width unlimited, height bigger")
            {
                a.applyMinimum(Size(Size::componentNone(), 21));
                REQUIRE(a == Size(10, 21));
            }

            SECTION("width unlimited, height equal")
            {
                a.applyMinimum(Size(Size::componentNone(), 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width unlimited, height smaller")
            {
                a.applyMinimum(Size(Size::componentNone(), 19));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width smaller, height bigger")
            {
                a.applyMinimum(Size(9, 21));
                REQUIRE(a == Size(10, 21));
            }

            SECTION("width bigger, height smaller")
            {
                a.applyMinimum(Size(11, 19));
                REQUIRE(a == Size(11, 20));
            }
        }

        SECTION("to unlimited width, normal height")
        {
            Size a(Size::componentNone(), 20);

            SECTION("width unlimited, height unlimited")
            {
                a.applyMinimum(Size::none());

                // should have no effect
                REQUIRE(a == Size(Size::componentNone(), 20));
            }

            SECTION("width normal, height unlimited")
            {
                a.applyMinimum(Size(10, Size::componentNone()));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width normal, height smaller")
            {
                a.applyMinimum(Size(10, 19));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width normal, height equal")
            {
                a.applyMinimum(Size(10, 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width normal, height bigger")
            {
                a.applyMinimum(Size(10, 21));
                REQUIRE(a == Size(10, 21));
            }
        }

        SECTION("to normal width, unlimited height")
        {
            Size a(10, Size::componentNone());

            SECTION("width unlimited, height unlimited")
            {
                a.applyMinimum(Size::none());

                // should have no effect
                REQUIRE(a == Size(10, Size::componentNone()));
            }

            SECTION("width unlimited, height normal")
            {
                a.applyMinimum(Size(Size::componentNone(), 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width smaller, height normal")
            {
                a.applyMinimum(Size(9, 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width equal, height normal")
            {
                a.applyMinimum(Size(10, 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width bigger, height normal")
            {
                a.applyMinimum(Size(11, 20));
                REQUIRE(a == Size(11, 20));
            }
        }
    }

    SECTION("applyMaximum")
    {
        SECTION("to normal size")
        {
            Size a(10, 20);

            SECTION("width unlimited, height unlimited")
            {
                a.applyMaximum(Size::none());

                // should have no effect
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width bigger, height unlimited")
            {
                a.applyMaximum(Size(11, Size::componentNone()));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width equal, height unlimited")
            {
                a.applyMaximum(Size(10, Size::componentNone()));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width smaller, height unlimited")
            {
                a.applyMaximum(Size(9, Size::componentNone()));
                REQUIRE(a == Size(9, 20));
            }

            SECTION("width unlimited, height bigger")
            {
                a.applyMaximum(Size(Size::componentNone(), 21));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width unlimited, height equal")
            {
                a.applyMaximum(Size(Size::componentNone(), 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width unlimited, height smaller")
            {
                a.applyMaximum(Size(Size::componentNone(), 19));
                REQUIRE(a == Size(10, 19));
            }

            SECTION("width smaller, height bigger")
            {
                a.applyMaximum(Size(9, 21));
                REQUIRE(a == Size(9, 20));
            }

            SECTION("width bigger, height smaller")
            {
                a.applyMaximum(Size(11, 19));
                REQUIRE(a == Size(10, 19));
            }
        }

        SECTION("to unlimited width, normal height")
        {
            Size a(Size::componentNone(), 20);

            SECTION("width unlimited, height unlimited")
            {
                a.applyMaximum(Size::none());

                // should have no effect
                REQUIRE(a == Size(Size::componentNone(), 20));
            }

            SECTION("width normal, height unlimited")
            {
                a.applyMaximum(Size(10, Size::componentNone()));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width normal, height smaller")
            {
                a.applyMaximum(Size(10, 19));
                REQUIRE(a == Size(10, 19));
            }

            SECTION("width normal, height equal")
            {
                a.applyMaximum(Size(10, 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width normal, height bigger")
            {
                a.applyMaximum(Size(10, 21));
                REQUIRE(a == Size(10, 20));
            }
        }

        SECTION("to normal width, unlimited height")
        {
            Size a(10, Size::componentNone());

            SECTION("width unlimited, height unlimited")
            {
                a.applyMaximum(Size::none());

                // should have no effect
                REQUIRE(a == Size(10, Size::componentNone()));
            }

            SECTION("width unlimited, height normal")
            {
                a.applyMaximum(Size(Size::componentNone(), 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width smaller, height normal")
            {
                a.applyMaximum(Size(9, 20));
                REQUIRE(a == Size(9, 20));
            }

            SECTION("width equal, height normal")
            {
                a.applyMaximum(Size(10, 20));
                REQUIRE(a == Size(10, 20));
            }

            SECTION("width bigger, height normal")
            {
                a.applyMaximum(Size(11, 20));
                REQUIRE(a == Size(10, 20));
            }
        }
    }
}
