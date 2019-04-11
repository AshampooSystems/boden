#include <bdn/String.h>
#include <chrono>
#include <gtest/gtest.h>

using namespace std::chrono_literals;

namespace bdn
{
    TEST(String, ToDuration)
    {
        EXPECT_EQ(string_to_duration("10ns"), 10ns);
        EXPECT_EQ(string_to_duration("10us"), 10us);
        EXPECT_EQ(string_to_duration("10ms"), 10ms);
        EXPECT_EQ(string_to_duration("10s"), 10s);
        EXPECT_EQ(string_to_duration("10min"), 10min);
        EXPECT_EQ(string_to_duration("10h"), 10h);
    }

    TEST(String, FromDuration)
    {
        EXPECT_EQ(duration_to_string(10ns), "10ns");
        EXPECT_EQ(duration_to_string(10us), "10\xC2\xB5s");
        EXPECT_EQ(duration_to_string(10ms), "10ms");
        EXPECT_EQ(duration_to_string(10s), "10s");
        EXPECT_EQ(duration_to_string(10min), "10min");
        EXPECT_EQ(duration_to_string(10h), "10h");
    }
}
