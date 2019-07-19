#include <gtest/gtest.h>

#include <bdn/AttributedString.h>

namespace bdn
{
    TEST(AttributedString, Instantiate)
    {
        auto attributedString = std::make_shared<AttributedString>();
        EXPECT_NE(attributedString, nullptr);
    }

    TEST(AttributedString, Html)
    {
        auto attributedString = std::make_shared<AttributedString>();

        bool result = attributedString->fromHTML("<b>Hello World</b>");

        EXPECT_TRUE(result);
    }
}
