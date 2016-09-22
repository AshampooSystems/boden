#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Rounder.h>

using namespace bdn;

template<class RounderType, typename InType, typename OutType>
void verifyRound(const RounderType& r, InType in, OutType expectedOut)
{
    REQUIRE( r.round(in)==out );
    REQUIRE( r(in)==out );
}


double dummyRound(double x)
{
    return std::ceil(x)*2;
}


TEST_CASE("Rounder")
{
    SECTION("unit=0")
    {
        REQUIRE_THROWS_AS( Rounder<std::ceil>(0), InvalidArgumentError);
    }

    SECTION("unit=1")
    {
        Rounder<std::ceil> r(1);

        verifyRound( r, 0, 0);
        verifyRound( r, 1, 1);
        verifyRound( r, -1, -1);

        verifyRound( r, 1.1, 2);
        verifyRound( r, 1.9, 2);

        verifyRound( r, -1.1, -2);
        verifyRound( r, -1.9, -2);
    }

    SECTION("unit=0.4")
    {
        Rounder<std::ceil> r(0.4);

        verifyRound( r, 0, 0);
        verifyRound( r, 1, 1.2);
        verifyRound( r, -1, -1.2);

        verifyRound( r, 1.12, 1.6);
        verifyRound( r, 1.19, 1.6);

        verifyRound( r, -1.12, -1.6);
        verifyRound( r, -1.19, -1.6);
    }
    
    SECTION("usesRoundFunction")
    {
        Rounder<dummyRound> r(0.4);

        verifyRound( r, 0, 0);
        verifyRound( r, 1, 2*1.2);
        verifyRound( r, -1, 2*-1.2);

        verifyRound( r, 1.12, 2*1.6);
        verifyRound( r, 1.19, 2*1.6);

        verifyRound( r, -1.12, 2*-1.6);
        verifyRound( r, -1.19, 2*-1.6);
    }

    SECTION("Size")
    {
        Rounder<std::ceil> r(0.4);

        verifyRound(r, Size(0.9, -1.9), Size(1.2, -2.0) );
    }

    SECTION("Point")
    {
        Rounder<std::ceil> r(0.4);

        verifyRound(r, Point(0.9, -1.9), Point(1.2, -2.0) );
    }

    SECTION("Rect")
    {
        Rounder<std::ceil> r(0.4);

        verifyRound(r, Rect(0.9, -1.9, -3.12, 0.3), Rect(1.2, -2.0, -3.2, 0.4) );
    }

    SECTION("Margin")
    {
        Rounder<std::ceil> r(0.4);

        verifyRound(r, Margin(0.9, -1.9, -3.12, 0.3), Margin(1.2, -2.0, -3.2, 0.4) );
    }

}


TEST_CASE("RoundUp")
{
    verifyRound( RoundUp(0.4), 1, 1.2);
}

TEST_CASE("RoundDown")
{
    verifyRound( RoundDown(0.4), 1, 0.8);
}

TEST_CASE("RoundNearest")
{
    verifyRound( RoundNearest(0.4), 1, 1.2);
    verifyRound( RoundNearest(0.4), 0.99, 0.8);
}

