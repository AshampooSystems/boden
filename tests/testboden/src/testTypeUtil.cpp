#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/typeUtil.h>

using namespace bdn;

static void testTypeSupportsShiftLeftWith()
{
    REQUIRE((typeSupportsShiftLeftWith<std::ostream, int>()));

    REQUIRE((typeSupportsShiftLeftWith<std::ostream, double>()));

    REQUIRE((typeSupportsShiftLeftWith<std::ostream, std::string>()));

    REQUIRE((typeSupportsShiftLeftWith<std::ostream, String>()));

    struct OStreamLeftShiftNotSupported
    {
    };

    REQUIRE(!(typeSupportsShiftLeftWith<std::ostream,
                                        OStreamLeftShiftNotSupported>()));

    // the following verifies that the function can be used in compile time
    // statements.
    static_assert(
        typeSupportsShiftLeftWith<std::ostream, int>(),
        "typeSupportsShiftLeftWith must be usable as a compile expression");
}

static void testTypeHasCustomArrowOperator()
{
    REQUIRE(typeHasCustomArrowOperator<std::shared_ptr<std::string>>());

    REQUIRE(typeHasCustomArrowOperator<P<Base>>());

    REQUIRE(!typeHasCustomArrowOperator<int>());

    REQUIRE(!typeHasCustomArrowOperator<void *>());

    REQUIRE(!typeHasCustomArrowOperator<Size>());

    // the following verifies that the function can be used in compile time
    // statements.
    static_assert(
        typeHasCustomArrowOperator<P<Base>>(),
        "typeHasCustomArrowOperator must be usable as a compile expression");
}

TEST_CASE("typeUtil")
{
    SECTION("typeSupportsShiftLeftWith")
    testTypeSupportsShiftLeftWith();

    SECTION("typeHasCustomArrowOperator")
    testTypeHasCustomArrowOperator();
}
