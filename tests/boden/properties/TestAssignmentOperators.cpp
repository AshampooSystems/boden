#include <bdn/property/Property.h>
#include <gtest/gtest.h>

TEST(Old_Property, AssignmentOperators)
{
    bdn::Property<double> a;

    a = 10.;
    EXPECT_EQ(a, 10.);

    a = 20;
    EXPECT_EQ(a, 20.);

    a = true;
    EXPECT_EQ(a, 1.);

    bdn::Property<double> b = 5., c = 5.;

    EXPECT_EQ((b += c), 10.);
    EXPECT_EQ((b -= c), 5.);
    EXPECT_EQ((b *= c), 25.);
    EXPECT_EQ((b /= c), 5.);

    bdn::Property<int> y = 5, z = 2;

    EXPECT_EQ((y %= z), 1);
    EXPECT_EQ((y &= z), (1 & 2));

    y = 1;
    EXPECT_EQ((y |= z), (1 | 2));

    y = 1;
    EXPECT_EQ((y ^= z), (1 ^ 2));

    y = 1;
    EXPECT_EQ((y <<= z), (1 << 2));

    y = 1;
    EXPECT_EQ((y >>= z), (1 >> 2));

    bdn::Property<int> number = 1;

    EXPECT_EQ((number += 1), 2);
    EXPECT_EQ((number -= 1), 1);

    number = 2;

    EXPECT_EQ((number *= 2), 4);
    EXPECT_EQ((number /= 2), 2);
}
