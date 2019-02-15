
#include <bdn/config.h>
#include <bdn/test.h>

using namespace bdn;

template <class Type> static void testFloatArithmethic()
{
    // here we verify that floating point arithmetic works as expected.

    SECTION("floating point standard compliance") { REQUIRE(std::numeric_limits<Type>::is_iec559); }

    SECTION("has NaN") { REQUIRE(std::numeric_limits<Type>::has_quiet_NaN); }

    SECTION("has infinity") { REQUIRE(std::numeric_limits<Type>::has_infinity); }

    Type inf = std::numeric_limits<Type>::infinity();
    Type negInf = -std::numeric_limits<Type>::infinity();
    Type nan = std::numeric_limits<Type>::quiet_NaN();
    Type normal = static_cast<Type>(17.45);

    SECTION("isfinite")
    {
        SECTION("infinity")
        REQUIRE(!std::isfinite(inf));

        SECTION("negative infinity")
        REQUIRE(!std::isfinite(negInf));

        SECTION("nan")
        REQUIRE(!std::isfinite(nan));

        SECTION("normal")
        REQUIRE(std::isfinite(normal));
    }

    SECTION("isinf")
    {
        SECTION("infinity")
        REQUIRE(std::isinf(inf));

        SECTION("negative infinity")
        REQUIRE(std::isinf(negInf));

        SECTION("nan")
        REQUIRE(!std::isinf(nan));

        SECTION("normal")
        REQUIRE(!std::isinf(normal));
    }

    SECTION("isnan")
    {
        SECTION("infinity")
        REQUIRE(!std::isnan(inf));

        SECTION("negative infinity")
        REQUIRE(!std::isnan(negInf));

        SECTION("nan")
        REQUIRE(std::isnan(nan));

        SECTION("normal")
        REQUIRE(!std::isnan(normal));
    }

    // Operations involving infinite and nan as operands can produce incorrect
    // results if aggressive floating point optimizations are enabled
    // (-ffast-math or -ffinite-math-only)

    // However, these flags are used quite a bit, so we, as a framework,
    // want to work properly when they are enabled. So we do not assume that nan
    // and infinity operations are standards compliant.

    // We do still perform these tests when we do not detect the fast-math
    // option to be on.

    if (!bdn::config::aggressive_float_optimization_enabled) {

        SECTION("infinity comparisons")
        {
            SECTION("with infinity")
            {
                SECTION("equality")
                REQUIRE(inf == inf);

                SECTION("relative")
                {
                    // relative comparison of infinities to infinities should always
                    // yield false
                    REQUIRE(!(inf < inf));
                    REQUIRE(!(inf > inf));

                    // since inf==inf inf<=inf must also be true
                    REQUIRE(inf <= inf);
                    REQUIRE(inf >= inf);
                }
            }

            SECTION("with negative infinity")
            {
                SECTION("equality")
                REQUIRE(inf != negInf);

                SECTION("relative")
                {
                    REQUIRE(!(inf < negInf));
                    REQUIRE(!(inf <= negInf));
                    REQUIRE(inf > negInf);
                    REQUIRE(inf >= negInf);
                }
            }

            SECTION("with normal number")
            REQUIRE(inf != 2);

            SECTION("number with infinity")
            {
                // relative comparison should work as expected with the infinities
                REQUIRE(17.2 < inf);
                REQUIRE(17.2 <= inf);
                REQUIRE(!(17.2 > inf));
                REQUIRE(!(17.2 >= inf));

                REQUIRE(-17.2 < inf);
                REQUIRE(-17.2 <= inf);
                REQUIRE(!(-17.2 > inf));
                REQUIRE(!(-17.2 >= inf));
            }
        }

        SECTION("negative infinity comparisons")
        {
            SECTION("with negative infinity")
            {
                SECTION("equality")
                REQUIRE(negInf == negInf);

                SECTION("relative")
                {
                    REQUIRE(!(negInf < negInf));
                    REQUIRE(!(negInf > negInf));
                    REQUIRE(negInf <= negInf);
                    REQUIRE(negInf >= negInf);
                }
            }

            SECTION("with infinity")
            {
                SECTION("equality")
                REQUIRE(negInf != inf);

                SECTION("relative")
                {
                    // we can compare positive and negative infinities with each
                    // other
                    REQUIRE(negInf < inf);
                    REQUIRE(negInf <= inf);
                    REQUIRE(!(negInf > inf));
                    REQUIRE(!(negInf >= inf));
                }
            }

            SECTION("with normal number")
            REQUIRE(negInf != 2);

            SECTION("number with negative infinity")
            {
                REQUIRE(!(17.2 < negInf));
                REQUIRE(!(17.2 <= negInf));
                REQUIRE(17.2 > negInf);
                REQUIRE(17.2 >= negInf);

                REQUIRE(!(-17.2 < negInf));
                REQUIRE(!(-17.2 <= negInf));
                REQUIRE(-17.2 > negInf);
                REQUIRE(-17.2 >= negInf);
            }
        }

        SECTION("infinity arithmetic")
        {
            SECTION("normal number")
            {
                REQUIRE(inf - 2 == inf);
                REQUIRE(inf + 2 == inf);
            }

            SECTION("add infinity")
            REQUIRE(inf + inf == inf);

            SECTION("add negative infinity")
            REQUIRE(std::isnan(inf + negInf));

            SECTION("subtract infinity")
            REQUIRE(std::isnan(inf - inf));

            SECTION("subtract negative infinity")
            REQUIRE(inf - negInf == inf);
        }

        SECTION("negative infinity arithmetic")
        {
            SECTION("normal number")
            {
                REQUIRE(negInf - 2 == negInf);
                REQUIRE(negInf + 2 == negInf);
            }

            SECTION("add negative infinity")
            REQUIRE(negInf + negInf == negInf);

            SECTION("add infinity")
            REQUIRE(std::isnan(negInf + inf));

            SECTION("subtract negative infinity")
            REQUIRE(std::isnan(negInf - negInf));

            SECTION("subtract infinity")
            REQUIRE(negInf - inf == negInf);
        }

        SECTION("NaN comparisons")
        {
            SECTION("not equal to anything")
            {
                // NaN should not compare equal to anything, not even itself
                REQUIRE(std::isnan(nan));
                REQUIRE(nan != inf);
                REQUIRE(nan != negInf);
                REQUIRE(nan != 17.2);
                REQUIRE(nan != (int)17);
                REQUIRE(nan != nan);
            }

            SECTION("relative comparisons all false")
            {
                // relative comparisons with nan should always yield false
                REQUIRE(!(nan < 17));
                REQUIRE(!(nan <= 17));
                REQUIRE(!(nan > 17));
                REQUIRE(!(nan >= 17));
                REQUIRE(!(nan >= nan));
                REQUIRE(!(nan > nan));
                REQUIRE(!(nan <= nan));
                REQUIRE(!(nan < nan));
            }
        }
    }
}

TEST_CASE("floatingPoint")
{
    SECTION("float")
    testFloatArithmethic<float>();

    SECTION("double")
    testFloatArithmethic<double>();
}
