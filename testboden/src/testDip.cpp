#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Dip.h>

using namespace bdn;


template<class AType, class BType>
static void verifyDipRelationImpl(AType a, BType b, int expectedRelation)
{
    int rel = Dip::compare(a, b);
    REQUIRE( rel == expectedRelation );

    REQUIRE( (a < b) == (rel==-1) );
    REQUIRE( (a > b) == (rel==1) );
    REQUIRE( (a == b) == (rel==0) );
    REQUIRE( (a != b) == (rel!=0) );
    REQUIRE( (a <= b) == (rel<=0) );
    REQUIRE( (a >= b) == (rel>=0) );
}


template<class AType, class BType>
static void verifyDipRelation(AType a, BType b, int expectedRelation)
{
    SECTION("normal")
        verifyDipRelationImpl<AType, BType>(a, b, expectedRelation);

    SECTION("inverse")
        verifyDipRelationImpl<BType, AType>(b, a, expectedRelation*-1 );    
};

void testDipComparison(double a, double b, int expectedRelation)
{
    // note that verifyDipRelation also automatically tests the inverse
    // comparison.
    Dip adip(a);

    SECTION("with double")
        verifyDipRelation<Dip, double>( adip, b, expectedRelation);

    SECTION("with Dip")
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
    }
}

