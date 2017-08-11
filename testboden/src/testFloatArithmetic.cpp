#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;


template<class Type>
static void testFloatArithmethic()
{
    // here we verify that floating point arithmetic works as expected.

    Type inf = std::numeric_limits<Type>::infinity();
    Type negInf = -std::numeric_limits<Type>::infinity();
    Type nan = std::numeric_limits<Type>::quiet_NaN();

    REQUIRE( inf == inf );
    REQUIRE( inf != negInf );
    REQUIRE( inf != 2 );

    REQUIRE( negInf == negInf );
    REQUIRE( negInf != inf  );
    REQUIRE( negInf != 2  );
    
    REQUIRE( inf - 2 == inf);
    REQUIRE( inf + 2 == inf);

    REQUIRE( negInf - 2 == negInf);
    REQUIRE( negInf + 2 == negInf);

    REQUIRE( inf + inf == inf);
    REQUIRE( std::isnan(inf - inf) );

    REQUIRE( negInf + negInf == negInf);
    REQUIRE( std::isnan(negInf - negInf) );

    REQUIRE( std::isnan(inf + negInf) );
    REQUIRE( inf - negInf == inf);

    REQUIRE( std::isnan(negInf + inf) );
    REQUIRE( negInf - inf == negInf);


    // relative comparison should work as expected with the infinities
    REQUIRE( 17.2 < inf );
    REQUIRE( 17.2 <= inf );
    REQUIRE( ! (17.2 > inf) );
    REQUIRE( ! (17.2 >= inf) );

    REQUIRE( -17.2 < inf );
    REQUIRE( -17.2 <= inf );
    REQUIRE( ! (-17.2 > inf) );
    REQUIRE( ! (-17.2 >= inf) );


    REQUIRE( ! (17.2 < negInf) );
    REQUIRE( ! (17.2 <= negInf) );
    REQUIRE( 17.2 > negInf );
    REQUIRE( 17.2 >= negInf );

    REQUIRE( ! (-17.2 < negInf) );
    REQUIRE( ! (-17.2 <= negInf) );
    REQUIRE( -17.2 > negInf );
    REQUIRE( -17.2 >= negInf );

    // relative comparison of infinities to infinities should always yield false
    REQUIRE( ! (inf < inf) );
    REQUIRE( ! (inf > inf) );

    // since inf==inf inf<=inf must also be true
    REQUIRE( inf <= inf );    
    REQUIRE( inf >= inf );

    REQUIRE( ! (negInf < negInf) );    
    REQUIRE( ! (negInf > negInf) );
    REQUIRE( negInf <= negInf );
    REQUIRE( negInf >= negInf );

    // we can compare positive and negative infinities with each other
    REQUIRE( negInf < inf );
    REQUIRE( negInf <= inf );
    REQUIRE( ! (negInf > inf) );
    REQUIRE( ! (negInf >= inf) );

    REQUIRE( ! (inf < negInf) );
    REQUIRE( ! (inf <= negInf) );
    REQUIRE( inf > negInf );
    REQUIRE( inf >= negInf );

    
    // NaN should not compare equal to anything, not even itself
    REQUIRE( std::isnan(nan) );
    REQUIRE( nan!=nan );
    REQUIRE( nan!=inf );
    REQUIRE( nan!=negInf );
    REQUIRE( nan!=17.2 );
    REQUIRE( nan!=(int)17 );

    // relative comparisons with nan should always yield false
    REQUIRE( ! (nan<17) );
    REQUIRE( ! (nan<=17) );
    REQUIRE( ! (nan>17) );
    REQUIRE( ! (nan>=17) );

}

TEST_CASE("floatArith")
{
    SECTION("float")
        testFloatArithmethic<float>();

    SECTION("double")
        testFloatArithmethic<double>();
}

