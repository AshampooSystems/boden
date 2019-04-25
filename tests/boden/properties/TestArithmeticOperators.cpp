#include <bdn/property/Property.h>
#include <gtest/gtest.h>

TEST(Old_Property, ArithmeticOperators)
{
    bdn::Property<double> a;
    const bdn::Property<double> b = 5., c = 5.;

    EXPECT_EQ((a = -b), -5.);
    EXPECT_EQ((a = +b), 5.);
    EXPECT_EQ((a = b + c), 10.);
    EXPECT_EQ((a = b - c), 0.);
    EXPECT_EQ((a = b * c), 25.);
    EXPECT_EQ((a = b / c), 1.);

    bdn::Property<int> x;
    const bdn::Property<int> y = 5, z = 2;

    EXPECT_EQ((x = y % z), 1);
    EXPECT_EQ((x = ~y), ~5);
    EXPECT_EQ((x = y & z), (5 & 2));
    EXPECT_EQ((x = y | z), (5 | 2));
    EXPECT_EQ((x = y ^ z), (5 ^ 2));
    EXPECT_EQ((x = y << z), (5 << 2));
    EXPECT_EQ((x = y >> z), (5 >> 2));

    x = 10;

    EXPECT_EQ((x + 2), 12);
    EXPECT_EQ((x - 2), 8);
    EXPECT_EQ((x * 2), 20);
    EXPECT_EQ((x / 2), 5);
}
