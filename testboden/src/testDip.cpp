#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Dip.h>

using namespace bdn;


template<class AType, class BType>
static void verifyDipRelationImpl(AType a, BType b, double expectedRelation)
{
    double rel = Dip::compare(a, b);

    // nan does not compare equal to itself. So we have to check with isnan
    if(std::isnan(expectedRelation))
    {
        REQUIRE( std::isnan(rel) );
    }
    else
        REQUIRE( rel == expectedRelation );

    REQUIRE( (a < b) == (rel==-1) );
    REQUIRE( (a > b) == (rel==1) );
    REQUIRE( (a == b) == (rel==0) );
    REQUIRE( (a != b) == (rel!=0) );
    REQUIRE( (a <= b) == (rel<=0) );
    REQUIRE( (a >= b) == (rel>=0) );

    REQUIRE( Dip::equal(a,b) == (rel==0) );
}


template<class AType, class BType>
static void verifyDipRelation(AType a, BType b, double expectedRelation)
{
    SECTION("normal")
        verifyDipRelationImpl<AType, BType>(a, b, expectedRelation);

    SECTION("inverse")
    {
        if(std::isnan(expectedRelation))
        {
            // if the expected result is NaN then the order of the operands does not matter.
            verifyDipRelationImpl<BType, AType>(b, a, expectedRelation );    
        }
        else
            verifyDipRelationImpl<BType, AType>(b, a, expectedRelation*-1 );    
    }
};

void testDipComparison(double a, double b, double expectedRelation)
{
    // note that verifyDipRelation also automatically tests the inverse
    // comparison.
    Dip adip(a);

    SECTION("second type is double")
        verifyDipRelation<Dip, double>( adip, b, expectedRelation);

    SECTION("second type is Dip")
        verifyDipRelation<Dip, Dip>( adip, Dip(b), expectedRelation);
}




static void _verifyDipPixelAlignImpl(double inVal, double pixelsPerDip, RoundType roundType, double expectedOutVal)
{
    // to make sure that the pixel aligner uses the correct roundType for position and size we
    // specify another round type for the unused value
    RoundType otherRoundType = (roundType==RoundType::nearest) ? RoundType::down : RoundType::nearest;

     SECTION("pixelAlign")
    {
        double outVal = Dip(inVal).pixelAlign( pixelsPerDip, roundType);
        REQUIRE( outVal == expectedOutVal );
    }

    SECTION("static pixelAlign")
    {
        double outVal = Dip::pixelAlign( inVal, pixelsPerDip, roundType);
        REQUIRE( outVal == expectedOutVal );
    }

    SECTION("static pixelAlign(Rect)")
    {
        SECTION("x")
        {
            double outVal = Dip::pixelAlign( Rect( inVal, 0, 0, 0 ), pixelsPerDip, roundType, otherRoundType).x;

            REQUIRE( outVal == expectedOutVal );
        }

        SECTION("y")
        {
            double outVal = Dip::pixelAlign( Rect( 0, inVal, 0, 0 ), pixelsPerDip, roundType, otherRoundType).y;

            REQUIRE( outVal == expectedOutVal );
        }

        SECTION("width")
        {
            double outVal = Dip::pixelAlign( Rect( 0, 0, inVal, 0 ), pixelsPerDip, otherRoundType, roundType).width;

            REQUIRE( outVal == expectedOutVal );
        }

        SECTION("height")
        {
            double outVal = Dip::pixelAlign( Rect( 0, 0, 0, inVal ), pixelsPerDip, otherRoundType, roundType).height;

            REQUIRE( outVal == expectedOutVal );
        }
    }
}

static void verifyDipPixelAlign(double inVal, double pixelsPerDip, RoundType roundType, double expectedOutVal)
{
    SECTION("positive")
        _verifyDipPixelAlignImpl( inVal, pixelsPerDip, roundType, expectedOutVal);

    SECTION("negative with inverted roundType")
    {
        // "down" always rounds to the next smaller value. I.e. for negative values
        // it rounds away from zero and for positive values it rounds towards zero.
        // For the expectedOutVal to match the actual result we must invert the rounding
        // direction.
        if(roundType==RoundType::up)
            roundType=RoundType::down;
        else if(roundType==RoundType::down)
            roundType=RoundType::up;

        _verifyDipPixelAlignImpl( -inVal, pixelsPerDip, roundType, -expectedOutVal);
    }
}

TEST_CASE("Dip")
{
    SECTION("construct")
    {
        SECTION("default")
        {
            Dip d;
            REQUIRE( d.getValue() == 0);
        }

        SECTION("value")
        {
            Dip d(17.45);
            REQUIRE( d.getValue() == 17.45);
        }

        SECTION("copy")
        {
            Dip s(17.45);
            Dip d( s );
            REQUIRE( d.getValue() == 17.45);
        }
    }

    SECTION("implicit conversion")
    {
        Dip d(17.45);

        double v = d;
        REQUIRE( v == 17.45 );
    }

    SECTION("assign")
    {
        Dip d(17.45);

        d = -7.89;

        REQUIRE( d.getValue() == -7.89 );
    }

    const double visibilityBoundary = Dip::visibilityBoundary();
    // we also want to test with big numbers, so we need to make sure that
    // epsilon can be represented even when added to a bigger number.
    // So we do not use numeric_limits::epsilon, but rather a generic "small number"
    const double epsilon = 0.00001;

    SECTION("comparison")
    {
        SECTION("zero")
            testDipComparison(0, 0, 0);

        SECTION("zero almost zero")
            testDipComparison(0, 0.0001, 0);

        SECTION("zero significantly different")
            testDipComparison(0, visibilityBoundary+0.0001, -1);
        
        // verify that the insignificance boundary also works with big numbers
        SECTION("big equal")
            testDipComparison(12345678.9123, 12345678.9123, 0);
        SECTION("big tiny bit bigger")
            testDipComparison(12345678.9123, 12345678.9123 + epsilon, 0);
        SECTION("big tiny bit smaller")
            testDipComparison(12345678.9123, 12345678.9123 - epsilon, 0);

        SECTION("big significantly bigger")
            testDipComparison(12345678.9123, 12345678.9123 + visibilityBoundary + epsilon, -1);

        SECTION("sign significant")
            testDipComparison( -1.23456 , 1.23456, -1);

        SECTION("infinity")
        {
            SECTION("to infinity")
                testDipComparison(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), 0);

            SECTION("to -infinity")
                testDipComparison(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), 1);

            SECTION("to non-infinity")
                testDipComparison(std::numeric_limits<double>::infinity(), 1, 1);

            SECTION("to nan")
                testDipComparison(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() );
        }

        SECTION("-infinity")
        {
            SECTION("to infinity")
                testDipComparison( - std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), -1);

            SECTION("to -infinity")
                testDipComparison( - std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), 0);

            SECTION("to non-infinity")
                testDipComparison( - std::numeric_limits<double>::infinity(), 1, -1);

            SECTION("to nan")
                testDipComparison( - std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() );
        }

        SECTION("nan")
        {
            SECTION("to infinity")
                testDipComparison(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity(),  std::numeric_limits<double>::quiet_NaN());

            SECTION("to -infinity")
                testDipComparison( std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN());

            SECTION("to non-infinity")
                testDipComparison( std::numeric_limits<double>::quiet_NaN(), 1, std::numeric_limits<double>::quiet_NaN());

            SECTION("to nan")
            {
                // nan is not equal to anything, not even itself
                testDipComparison( std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() );
            }
        }
    }

    SECTION("equal")
    {
        SECTION("Size")
        {
            REQUIRE( Dip::equal( Size(1, 2), Size(1,2) ) );
            
            REQUIRE( Dip::equal( Size(1 + visibilityBoundary - epsilon, 2), Size(1,2) ) );
            REQUIRE( !Dip::equal( Size(1 + visibilityBoundary + epsilon, 2), Size(1,2) ) );
            REQUIRE( Dip::equal( Size(std::numeric_limits<double>::infinity(), 2), Size(std::numeric_limits<double>::infinity(), 2) ) );
            REQUIRE( !Dip::equal( Size(std::numeric_limits<double>::infinity(), 2), Size(1, 2) ) );
            REQUIRE( !Dip::equal( Size(std::numeric_limits<double>::quiet_NaN(), 2), Size(1, 2) ) );
            
            REQUIRE( Dip::equal( Size(1, 2 + visibilityBoundary - epsilon), Size(1,2) ) );
            REQUIRE( !Dip::equal( Size(1, 2  + visibilityBoundary + epsilon), Size(1,2) ) );
            REQUIRE( Dip::equal( Size(1, std::numeric_limits<double>::infinity()), Size(1, std::numeric_limits<double>::infinity()) ) );
            REQUIRE( !Dip::equal( Size(1, std::numeric_limits<double>::infinity()), Size(1, 2) ) );            
            REQUIRE( !Dip::equal( Size(1, std::numeric_limits<double>::quiet_NaN()), Size(1, 2) ) );
        }

        SECTION("Point")
        {
            REQUIRE( Dip::equal( Point(1, 2), Point(1,2) ) );

            REQUIRE( Dip::equal( Point(1 + visibilityBoundary - epsilon, 2), Point(1,2) ) );
            REQUIRE( !Dip::equal( Point(1 + visibilityBoundary + epsilon, 2), Point(1,2) ) );
            REQUIRE( Dip::equal( Point(std::numeric_limits<double>::infinity(), 2), Point(std::numeric_limits<double>::infinity(), 2) ) );
            REQUIRE( !Dip::equal( Point(std::numeric_limits<double>::infinity(), 2), Point(1, 2) ) );
            REQUIRE( !Dip::equal( Point(std::numeric_limits<double>::quiet_NaN(), 2), Point(1, 2) ) );

            REQUIRE( Dip::equal( Point(1, 2 + visibilityBoundary - epsilon), Point(1,2) ) );
            REQUIRE( !Dip::equal( Point(1, 2 + visibilityBoundary + epsilon), Point(1,2) ) );            
            REQUIRE( Dip::equal( Point(1, std::numeric_limits<double>::infinity()), Point(1, std::numeric_limits<double>::infinity()) ) );
            REQUIRE( !Dip::equal( Point(1, std::numeric_limits<double>::infinity()), Point(1, 2) ) );            
            REQUIRE( !Dip::equal( Point(1, std::numeric_limits<double>::quiet_NaN()), Point(1, 2) ) );            
        }

        SECTION("Rect")
        {
            REQUIRE( Dip::equal( Rect(1, 2, 3, 4), Rect(1, 2, 3, 4) ) );
            
            REQUIRE( Dip::equal( Rect(1 + visibilityBoundary - epsilon, 2, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1 + visibilityBoundary + epsilon, 2, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( Dip::equal( Rect(std::numeric_limits<double>::infinity(), 2, 3, 4), Rect(std::numeric_limits<double>::infinity(), 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(std::numeric_limits<double>::infinity(), 2, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(std::numeric_limits<double>::quiet_NaN(), 2, 3, 4), Rect(1, 2, 3, 4) ) );

            REQUIRE( Dip::equal( Rect(1, 2 + visibilityBoundary - epsilon, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2 + visibilityBoundary + epsilon, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( Dip::equal( Rect(1, std::numeric_limits<double>::infinity(), 3, 4), Rect(1, std::numeric_limits<double>::infinity(), 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, std::numeric_limits<double>::infinity(), 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, std::numeric_limits<double>::quiet_NaN(), 3, 4), Rect(1, 2, 3, 4) ) );

            REQUIRE( Dip::equal( Rect(1, 2, 3 + visibilityBoundary - epsilon, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2, 3 + visibilityBoundary + epsilon, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( Dip::equal( Rect(1, 2, std::numeric_limits<double>::infinity(), 4), Rect(1, 2, std::numeric_limits<double>::infinity(), 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2, std::numeric_limits<double>::infinity(), 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2, std::numeric_limits<double>::quiet_NaN(), 4), Rect(1, 2, 3, 4) ) );

            REQUIRE( Dip::equal( Rect(1, 2, 3, 4 + visibilityBoundary - epsilon), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2, 3, 4 + visibilityBoundary + epsilon), Rect(1, 2, 3, 4) ) );            
            REQUIRE( Dip::equal( Rect(1, 2, 3, std::numeric_limits<double>::infinity()), Rect(1, 2, 3, std::numeric_limits<double>::infinity()) ) );            
            REQUIRE( !Dip::equal( Rect(1, 2, 3, std::numeric_limits<double>::infinity()), Rect(1, 2, 3, 4) ) );            
            REQUIRE( !Dip::equal( Rect(1, 2, 3, std::numeric_limits<double>::quiet_NaN()), Rect(1, 2, 3, 4) ) );            
        }
    }

    SECTION("pixel alignment")
    {
        RoundType roundTypes[] = {RoundType::nearest, RoundType::down, RoundType::up};

        const double visibilityBoundary = Dip::visibilityBoundary();
        const double tiny = std::numeric_limits<double>::epsilon()*2;

        const double pixelsPerDip = 1.25;

        for( RoundType roundType: roundTypes)
        {
            SECTION("roundType=" + std::to_string((int)roundType) )
            {
                // note that since there are 1.25 pixels per DIP in our example that 
                // one pixel has a size of 0.8 DIPs. So pixel boundaries are at 0.8, 1.6, 2.4, etc.

                SECTION("exact")
                    verifyDipPixelAlign( 1.6, pixelsPerDip, roundType, 1.6 );

                SECTION("invisibly more")
                {
                    // should not matter, since the difference is "invisible"
                    verifyDipPixelAlign(
                        1.6 + visibilityBoundary - tiny,
                        pixelsPerDip,
                        roundType,
                        1.6 );
                }

                SECTION("invisibly less")
                {
                    // should not matter, since the difference is "invisible"
                    verifyDipPixelAlign(
                        1.6 - visibilityBoundary + tiny,
                        pixelsPerDip,
                        roundType,
                        1.6 );
                }

                SECTION("visibly more")
                {
                    verifyDipPixelAlign(
                        1.6 + visibilityBoundary + tiny,
                        pixelsPerDip,
                        roundType,
                        (roundType==RoundType::up) ? 2.4 : 1.6 );
                }

                SECTION("visibly less")
                {                
                    verifyDipPixelAlign(
                        1.6 - visibilityBoundary - tiny,
                        pixelsPerDip,
                        roundType,
                        (roundType==RoundType::down) ? 0.8 : 1.6 );
                }

                SECTION("halfway")
                {
                    verifyDipPixelAlign(
                        1.2,
                        pixelsPerDip,
                        roundType,
                        (roundType==RoundType::down) ? 0.8 : 1.6  );
                }

                SECTION("invisibly less than halfway")
                {
                    verifyDipPixelAlign(
                        1.2 - visibilityBoundary + tiny,
                        pixelsPerDip,
                        roundType,
                        (roundType==RoundType::down) ? 0.8 : 1.6  );
                }

                SECTION("invisibly more than halfway")
                {
                    verifyDipPixelAlign(
                        1.2 + visibilityBoundary - tiny,
                        pixelsPerDip,
                        roundType,
                        (roundType==RoundType::down) ? 0.8 : 1.6  );
                }

                SECTION("visibly less than halfway")
                {
                    // nearest and down should both round down
                    verifyDipPixelAlign(
                        1.2 - visibilityBoundary - tiny,
                        pixelsPerDip,
                        roundType,
                        (roundType==RoundType::up) ? 1.6 : 0.8  );
                }

                SECTION("visibly more than halfway")
                {
                    // nearest and up should both round up
                    verifyDipPixelAlign(
                        1.2 + visibilityBoundary + tiny,
                        pixelsPerDip,
                        roundType,
                        (roundType==RoundType::down) ? 0.8 : 1.6  );
                }
            }
        }
    }    
}

