
#include <bdn/test.h>

#include <bdn/round.h>

using namespace bdn;

static void testRound(RoundType roundType, double in, double scaleFactor, double expectedOut)
{
    SECTION("roundType: " + std::to_string((int)roundType) + ", value: " + std::to_string(in) +
            ", scaleFactor: " + std::to_string(scaleFactor))
    {
        REQUIRE(stableScaledRound(roundType, in, scaleFactor) == expectedOut);

        // rounding the round result must yield the same result (stable)
        REQUIRE(stableScaledRound(roundType, expectedOut, scaleFactor) == expectedOut);

        if (roundType == RoundType::up) {
            REQUIRE(stableScaledRoundUp(in, scaleFactor) == expectedOut);
            REQUIRE(stableScaledRoundUp(expectedOut, scaleFactor) == expectedOut);
        } else if (roundType == RoundType::down) {
            REQUIRE(stableScaledRoundDown(in, scaleFactor) == expectedOut);
            REQUIRE(stableScaledRoundDown(expectedOut, scaleFactor) == expectedOut);
        } else if (roundType == RoundType::nearest) {
            REQUIRE(stableScaledRoundNearest(in, scaleFactor) == expectedOut);
            REQUIRE(stableScaledRoundNearest(expectedOut, scaleFactor) == expectedOut);
        } else {
            // invalid round type
            REQUIRE(false);
        }
    }
}

TEST_CASE("stableScaledRound")
{
    SECTION("factor=0")
    {
        REQUIRE_THROWS_AS(stableScaledRoundDown(1, 0), InvalidArgumentError);
        REQUIRE_THROWS_AS(stableScaledRoundUp(1, 0), InvalidArgumentError);
        REQUIRE_THROWS_AS(stableScaledRoundNearest(1, 0), InvalidArgumentError);

        REQUIRE_THROWS_AS(stableScaledRound(RoundType::down, 1, 0), InvalidArgumentError);
        REQUIRE_THROWS_AS(stableScaledRound(RoundType::up, 1, 0), InvalidArgumentError);
        REQUIRE_THROWS_AS(stableScaledRound(RoundType::nearest, 1, 0), InvalidArgumentError);
    }

    SECTION("factor=1")
    {
        testRound(RoundType::up, 1.4, 1, 2);
        testRound(RoundType::down, 1.6, 1, 1);
        testRound(RoundType::nearest, 1.4, 1, 1);
        testRound(RoundType::nearest, 1.6, 1, 2);
        testRound(RoundType::nearest, 1.5, 1, 2);

        testRound(RoundType::up, -1.6, 1, -1);
        testRound(RoundType::down, -1.4, 1, -2);
        testRound(RoundType::nearest, -1.4, 1, -1);
        testRound(RoundType::nearest, -1.6, 1, -2);
        testRound(RoundType::nearest, -1.5, 1, -2);
    }

    SECTION("factor=3")
    {
        testRound(RoundType::up, 1.4, 3, 5.0 / 3);
        testRound(RoundType::down, 1.6, 3, 4.0 / 3);
        testRound(RoundType::nearest, 1.4, 3, 4.0 / 3);
        testRound(RoundType::nearest, 1.6, 3, 5.0 / 3);
        // halfway point
        testRound(RoundType::nearest, (5.0 / 3 + 4.0 / 3) / 2, 3, 5.0 / 3);

        testRound(RoundType::up, -1.6, 3, -4.0 / 3);
        testRound(RoundType::down, -1.4, 3, -5.0 / 3);
        testRound(RoundType::nearest, -1.4, 3, -4.0 / 3);
        testRound(RoundType::nearest, -1.6, 3, -5.0 / 3);
        // halfway point
        testRound(RoundType::nearest, (-5.0 / 3 + -4.0 / 3) / 2, 3, -5.0 / 3);
    }
}
