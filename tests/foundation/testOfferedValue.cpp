#include <gtest/gtest.h>

#include <bdn/OfferedValue.h>
#include <bdn/String.h>

using namespace std::string_literals;

namespace bdn
{
    TEST(OfferedValue, Empty)
    {
        OfferedValue<String> value;
        EXPECT_EQ(value.get(), String());
    }

    TEST(OfferedValue, offered)
    {
        OfferedValue<String> value;
        value.setOffered("Hello");
        EXPECT_EQ(value.get(), String("Hello"));
    }

    TEST(OfferedValue, Own)
    {
        OfferedValue<String> value;
        value.set("Hello");
        EXPECT_EQ(value.get(), String("Hello"));
    }

    TEST(OfferedValue, OwnOverridesOffered)
    {
        OfferedValue<String> value;
        value.setOffered("Test");
        value.set("Hello");
        EXPECT_EQ(value.get(), String("Hello"));

        value.setOffered("Still?");
        EXPECT_EQ(value.get(), String("Hello"));
    }

    TEST(OfferedValue, FallBackToOffered)
    {
        OfferedValue<String> value;
        value.setOffered("Test");
        value.set("Hello");
        EXPECT_EQ(value.get(), String("Hello"));

        value.unset();
        EXPECT_EQ(value.get(), String("Test"));
    }
}
