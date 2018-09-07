#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/sort.h>

using namespace bdn;

TEST_CASE("sort")
{
    SECTION("ascending")
    {
        SECTION("int")
        {
            REQUIRE(ascending(1, 2) == true);
            REQUIRE(ascending(1, 1) == false);
            REQUIRE(ascending(2, 1) == false);
        }

        SECTION("String")
        {
            REQUIRE(ascending(String("a"), String("b")) == true);
            REQUIRE(ascending(String("a"), String("a")) == false);
            REQUIRE(ascending(String("b"), String("a")) == false);
        }
    }

    SECTION("descending")
    {
        SECTION("int")
        {
            REQUIRE(descending(1, 2) == false);
            REQUIRE(descending(1, 1) == false);
            REQUIRE(descending(2, 1) == true);
        }

        SECTION("String")
        {
            REQUIRE(descending(String("a"), String("b")) == false);
            REQUIRE(descending(String("a"), String("a")) == false);
            REQUIRE(descending(String("b"), String("a")) == true);
        }
    }
}
