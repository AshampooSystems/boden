#include <gtest/gtest.h>

#include <bdn/Color.h>
#include <bdn/Json.h>

namespace bdn::ui
{
    TEST(Color, InitWithStringWithAlpha)
    {
        Color c("#0080FFAB");

        auto x = std::array<uint8_t, 4>{0x00, 0x80, 0xFF, 0xAB};

        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }

    TEST(Color, InitWithString)
    {
        Color c("#0080FF");

        auto x = std::array<uint8_t, 4>{0x00, 0x80, 0xFF, 0xFF};

        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }

    TEST(Color, InitWithStringMissingAlpha)
    {
        Color c("#0080FFA");

        auto x = std::array<uint8_t, 4>{0x00, 0x00, 0x00, 0xFF};

        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }

    TEST(Color, InitWith3LetterStringWithAlpha)
    {
        Color c("#08F7");

        auto x = std::array<uint8_t, 4>{0x00, 0x88, 0xFF, 0x77};
        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }

    TEST(Color, InitWith3LetterString)
    {
        Color c("#08F");

        auto x = std::array<uint8_t, 4>{0x00, 0x88, 0xFF, 0xFF};
        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }

    TEST(Color, Named)
    {
        Color c("BurlyWood");

        auto x = std::array<uint8_t, 4>{0xDE, 0xB8, 0x87, 0xFF};
        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }

    TEST(Color, JsonNamed)
    {
        bdn::json j = JsonStringify("BurlyWood");

        Color c = j;

        auto x = std::array<uint8_t, 4>{0xDE, 0xB8, 0x87, 0xFF};
        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }

    TEST(Color, JsonWithString)
    {
        bdn::json j = JsonStringify("#007fffaa");

        Color c = j;

        auto x = std::array<uint8_t, 4>{0x00, 0x7f, 0xff, 0xaa};
        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }

    TEST(Color, JsonWithArray)
    {
        bdn::json j = JsonStringify([ 0.0, 0.5, 1.0, 1.0 ]);

        Color c = j;

        auto x = std::array<uint8_t, 4>{0x00, 0x7f, 0xff, 0xff};
        EXPECT_EQ(c.asIntArray<uint8_t>(), x);
    }
}
