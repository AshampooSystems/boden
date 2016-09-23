#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Rounder.h>
#include <bdn/RoundUp.h>
#include <bdn/RoundDown.h>
#include <bdn/RoundNearest.h>

using namespace bdn;

template<class RounderType, typename InType, typename OutType>
void verifyRound(const RounderType& r, InType in, OutType expectedOut, OutType maxDeviation)
{
    REQUIRE_ALMOST_EQUAL( r.round(in), expectedOut, maxDeviation );
    REQUIRE_ALMOST_EQUAL( r(in), expectedOut, maxDeviation );

    // rounding the rounded value again should yield the same result
    REQUIRE( r.round(in) == r.round( r.round(in) ) );
    REQUIRE( r(in) == r( r(in) ) );
}

template<class RounderType>
void verifyRoundDouble(const RounderType& r, double in, double expectedOut)
{
    verifyRound(r, in, expectedOut, 0.0000001);
}

template<class RounderType>
void verifyRoundSize(const RounderType& r, Size in, Size expectedOut)
{
    verifyRound(r, in, expectedOut, Size(0.0000001, 0.0000001) );
}

template<class RounderType>
void verifyRoundPoint(const RounderType& r, Point in, Point expectedOut)
{
    verifyRound(r, in, expectedOut, Point(0.0000001, 0.0000001) );
}

template<class RounderType>
void verifyRoundRect(const RounderType& r, Rect in, Rect expectedOut)
{
    {
        Rect out = r.round(in);

        REQUIRE_ALMOST_EQUAL( out.x, expectedOut.x, 0.0000001 );
        REQUIRE_ALMOST_EQUAL( out.y, expectedOut.y, 0.0000001 );
        REQUIRE_ALMOST_EQUAL( out.getSize(), expectedOut.getSize(), Size(0.0000001, 0.0000001) );
    }

    {
        Rect out = r(in);

        REQUIRE_ALMOST_EQUAL( out.x, expectedOut.x, 0.0000001 );
        REQUIRE_ALMOST_EQUAL( out.y, expectedOut.y, 0.0000001 );
        REQUIRE_ALMOST_EQUAL( out.getSize(), expectedOut.getSize(), Size(0.0000001, 0.0000001) );
    }
}



template<class RounderType>
void verifyRoundMargin(const RounderType& r, Margin in, Margin expectedOut)
{
    verifyRound(r, in, expectedOut, Margin(0.0000001));
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

        verifyRoundDouble( r, 0, 0);
        verifyRoundDouble( r, 1, 1);
        verifyRoundDouble( r, -1, -1);

        verifyRoundDouble( r, 1.1, 2);
        verifyRoundDouble( r, 1.9, 2);

        verifyRoundDouble( r, -1.1, -1);
        verifyRoundDouble( r, -1.9, -1);
    }

    SECTION("unit=0.4")
    {
        Rounder<std::ceil> r(0.4);

        verifyRoundDouble( r, 0, 0);
        verifyRoundDouble( r, 1, 1.2);
        verifyRoundDouble( r, -1, -0.8);

        verifyRoundDouble( r, 1.12, 1.2);
        verifyRoundDouble( r, 1.19, 1.2);

        verifyRoundDouble( r, -1.12, -0.8);
        verifyRoundDouble( r, -1.19, -0.8);
    }
    
    SECTION("usesRoundFunction")
    {
        Rounder<dummyRound> r(0.4);

        verifyRoundDouble( r, 0, 0);
        verifyRoundDouble( r, 1, 2*1.2);
        verifyRoundDouble( r, -1, 2*-0.8);

        verifyRoundDouble( r, 1.12, 2*1.2);
        verifyRoundDouble( r, 1.19, 2*1.2);

        verifyRoundDouble( r, -1.12, 2*-0.8);
        verifyRoundDouble( r, -1.19, 2*-0.8);
    }

    SECTION("Size")
    {
        Rounder<std::ceil> r(0.4);

        verifyRoundSize(r, Size(0.9, -1.9), Size(1.2, -1.6) );
    }

    SECTION("Point")
    {
        Rounder<std::ceil> r(0.4);

        verifyRoundPoint(r, Point(0.9, -1.9), Point(1.2, -1.6) );
    }

    SECTION("Rect")
    {
        Rounder<std::ceil> r(0.4);

        verifyRoundRect(r, Rect(0.9, -1.9, -3.12, 0.3), Rect(1.2, -1.6, -2.8, 0.4) );
    }

    SECTION("Margin")
    {
        Rounder<std::ceil> r(0.4);

        verifyRoundMargin(r, Margin(0.9, -1.9, -3.12, 0.3), Margin(1.2, -1.6, -2.8, 0.4) );
    }

}


TEST_CASE("RoundUp")
{
    verifyRoundDouble( RoundUp(0.4), 1, 1.2);
}

TEST_CASE("RoundDown")
{
    verifyRoundDouble( RoundDown(0.4), 1, 0.8);
}

TEST_CASE("RoundNearest")
{
    verifyRoundDouble( RoundNearest(0.4), 1, 1.2);
    verifyRoundDouble( RoundNearest(0.4), 0.99, 0.8);
}

