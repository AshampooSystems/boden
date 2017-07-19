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

    SECTION("comparison")
    {
        SECTION("zero")
            testDipComparison(0, 0, 0);

        SECTION("zero almost zero")
            testDipComparison(0, 0.0001, 0);

        SECTION("zero significantly different")
            testDipComparison(0, 0.00011, -1);
        
        // verify that the insignificance boundary also works with big numbers
        SECTION("big equal")
            testDipComparison(12345678.9123, 12345678.9123, 0);
        SECTION("big tiny bit bigger")
            testDipComparison(12345678.9123, 12345678.9124, 0);
        SECTION("big tiny bit smaller")
            testDipComparison(12345678.9123, 12345678.9122, 0);

        SECTION("big significantly bigger")
            testDipComparison(12345678.9123, 12345678.91241, -1);

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
            
            REQUIRE( Dip::equal( Size(1.00009, 2), Size(1,2) ) );
            REQUIRE( !Dip::equal( Size(1.00011, 2), Size(1,2) ) );
            REQUIRE( Dip::equal( Size(std::numeric_limits<double>::infinity(), 2), Size(std::numeric_limits<double>::infinity(), 2) ) );
            REQUIRE( !Dip::equal( Size(std::numeric_limits<double>::infinity(), 2), Size(1, 2) ) );
            REQUIRE( !Dip::equal( Size(std::numeric_limits<double>::quiet_NaN(), 2), Size(1, 2) ) );
            
            REQUIRE( Dip::equal( Size(1, 2.00009), Size(1,2) ) );
            REQUIRE( !Dip::equal( Size(1, 2.00011), Size(1,2) ) );
            REQUIRE( Dip::equal( Size(1, std::numeric_limits<double>::infinity()), Size(1, std::numeric_limits<double>::infinity()) ) );
            REQUIRE( !Dip::equal( Size(1, std::numeric_limits<double>::infinity()), Size(1, 2) ) );            
            REQUIRE( !Dip::equal( Size(1, std::numeric_limits<double>::quiet_NaN()), Size(1, 2) ) );
        }

        SECTION("Point")
        {
            REQUIRE( Dip::equal( Point(1, 2), Point(1,2) ) );

            REQUIRE( Dip::equal( Point(1.00009, 2), Point(1,2) ) );
            REQUIRE( !Dip::equal( Point(1.00011, 2), Point(1,2) ) );
            REQUIRE( Dip::equal( Point(std::numeric_limits<double>::infinity(), 2), Point(std::numeric_limits<double>::infinity(), 2) ) );
            REQUIRE( !Dip::equal( Point(std::numeric_limits<double>::infinity(), 2), Point(1, 2) ) );
            REQUIRE( !Dip::equal( Point(std::numeric_limits<double>::quiet_NaN(), 2), Point(1, 2) ) );

            REQUIRE( Dip::equal( Point(1, 2.00009), Point(1,2) ) );
            REQUIRE( !Dip::equal( Point(1, 2.00011), Point(1,2) ) );            
            REQUIRE( Dip::equal( Point(1, std::numeric_limits<double>::infinity()), Point(1, std::numeric_limits<double>::infinity()) ) );
            REQUIRE( !Dip::equal( Point(1, std::numeric_limits<double>::infinity()), Point(1, 2) ) );            
            REQUIRE( !Dip::equal( Point(1, std::numeric_limits<double>::quiet_NaN()), Point(1, 2) ) );            
        }

        SECTION("Rect")
        {
            REQUIRE( Dip::equal( Rect(1, 2, 3, 4), Rect(1, 2, 3, 4) ) );
            
            REQUIRE( Dip::equal( Rect(1.00009, 2, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1.00011, 2, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( Dip::equal( Rect(std::numeric_limits<double>::infinity(), 2, 3, 4), Rect(std::numeric_limits<double>::infinity(), 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(std::numeric_limits<double>::infinity(), 2, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(std::numeric_limits<double>::quiet_NaN(), 2, 3, 4), Rect(1, 2, 3, 4) ) );

            REQUIRE( Dip::equal( Rect(1, 2.00009, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2.00011, 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( Dip::equal( Rect(1, std::numeric_limits<double>::infinity(), 3, 4), Rect(1, std::numeric_limits<double>::infinity(), 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, std::numeric_limits<double>::infinity(), 3, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, std::numeric_limits<double>::quiet_NaN(), 3, 4), Rect(1, 2, 3, 4) ) );

            REQUIRE( Dip::equal( Rect(1, 2, 3.00009, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2, 3.00011, 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( Dip::equal( Rect(1, 2, std::numeric_limits<double>::infinity(), 4), Rect(1, 2, std::numeric_limits<double>::infinity(), 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2, std::numeric_limits<double>::infinity(), 4), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2, std::numeric_limits<double>::quiet_NaN(), 4), Rect(1, 2, 3, 4) ) );

            REQUIRE( Dip::equal( Rect(1, 2, 3, 4.00009), Rect(1, 2, 3, 4) ) );
            REQUIRE( !Dip::equal( Rect(1, 2, 3, 4.00011), Rect(1, 2, 3, 4) ) );            
            REQUIRE( Dip::equal( Rect(1, 2, 3, std::numeric_limits<double>::infinity()), Rect(1, 2, 3, std::numeric_limits<double>::infinity()) ) );            
            REQUIRE( !Dip::equal( Rect(1, 2, 3, std::numeric_limits<double>::infinity()), Rect(1, 2, 3, 4) ) );            
            REQUIRE( !Dip::equal( Rect(1, 2, 3, std::numeric_limits<double>::quiet_NaN()), Rect(1, 2, 3, 4) ) );            
        }
    }
}

