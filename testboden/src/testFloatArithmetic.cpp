#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;


template<class Type>
static void testFloatArithmethic()
{
    Type inf = std::numeric_limits<Type>::infinity();
    Type negInf = -std::numeric_limits<Type>::infinity();

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
    REQUIRE( inf - inf == inf);

    REQUIRE( negInf + negInf == negInf);
    REQUIRE( negInf - negInf == negInf);

    REQUIRE( inf + negInf == inf);
    REQUIRE( inf - negInf == inf);

    REQUIRE( negInf + inf == negInf);
    REQUIRE( negInf - inf == negInf);
}

TEST_CASE("floating point arithmetic")
{
    SECTION("float")
        testFloatArithmethic<float>();

    SECTION("double")
        testFloatArithmethic<double>();
}

