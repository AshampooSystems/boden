#include <gtest/gtest.h>

#include <bdn/OfferedValue.h>
#include <string>

using namespace std::string_literals;

namespace bdn
{
    TEST(OfferedValue, Empty)
    {
        OfferedValue<std::string> value;
        EXPECT_EQ(value.get(), std::string());
    }

    TEST(OfferedValue, offered)
    {
        OfferedValue<std::string> value;
        value.setOffered("Hello");
        EXPECT_EQ(value.get(), std::string("Hello"));
    }

    TEST(OfferedValue, Own)
    {
        OfferedValue<std::string> value;
        value.set("Hello");
        EXPECT_EQ(value.get(), std::string("Hello"));
    }

    TEST(OfferedValue, OwnOverridesOffered)
    {
        OfferedValue<std::string> value;
        value.setOffered("Test");
        value.set("Hello");
        EXPECT_EQ(value.get(), std::string("Hello"));

        value.setOffered("Still?");
        EXPECT_EQ(value.get(), std::string("Hello"));
    }

    TEST(OfferedValue, FallBackToOffered)
    {
        OfferedValue<std::string> value;
        value.setOffered("Test");
        value.set("Hello");
        EXPECT_EQ(value.get(), std::string("Hello"));

        value.unset();
        EXPECT_EQ(value.get(), std::string("Test"));
    }
}
