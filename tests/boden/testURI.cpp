#include <gtest/gtest.h>

#include <bdn/Uri.h>
#include <string>

using namespace std::string_literals;

namespace bdn
{
    TEST(URI, no_change) { EXPECT_EQ("HelloWorld", Uri::unescape("HelloWorld")); }
    TEST(URI, space_change) { EXPECT_EQ("Hello World", Uri::unescape("Hello%20World")); }
    TEST(URI, two_in_a_row_change) { EXPECT_EQ("Hello /World", Uri::unescape("Hello%20%2fWorld")); }
    TEST(URI, uppercase_change) { EXPECT_EQ("Hello /World", Uri::unescape("Hello%20%2FWorld")); }
}
