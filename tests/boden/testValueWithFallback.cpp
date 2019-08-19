#include <gtest/gtest.h>

#include <bdn/ValueWithFallback.h>
#include <string>

using namespace std::string_literals;

namespace bdn
{
    TEST(ValueWithFallback, Empty)
    {
        ValueWithFallback<std::string> value;
        EXPECT_EQ(value.get(), std::string());
    }

    TEST(ValueWithFallback, fallback)
    {
        ValueWithFallback<std::string> value;
        value.setFallback("Hello");
        EXPECT_EQ(value.get(), std::string("Hello"));
    }

    TEST(ValueWithFallback, Own)
    {
        ValueWithFallback<std::string> value;
        value.set("Hello");
        EXPECT_EQ(value.get(), std::string("Hello"));
    }

    TEST(ValueWithFallback, OwnOverridesFallback)
    {
        ValueWithFallback<std::string> value;
        value.setFallback("Test");
        value.set("Hello");
        EXPECT_EQ(value.get(), std::string("Hello"));

        value.setFallback("Still?");
        EXPECT_EQ(value.get(), std::string("Hello"));
    }

    TEST(ValueWithFallback, FallBackFromOwn)
    {
        ValueWithFallback<std::string> value;
        value.setFallback("Test");
        value.set("Hello");
        EXPECT_EQ(value.get(), std::string("Hello"));

        value.unset();
        EXPECT_EQ(value.get(), std::string("Test"));
    }
}
