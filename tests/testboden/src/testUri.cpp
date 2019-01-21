
#include <bdn/test.h>

#include <bdn/Uri.h>
#include <bdn/hex.h>

using namespace bdn;

void testUnescape()
{
    SECTION("empty")
    REQUIRE(Uri::unescape("") == "");

    SECTION("percent")
    REQUIRE(Uri::unescape("%") == "%");

    SECTION("notEscaped")
    REQUIRE(Uri::unescape(bdn::fromUtf32(U"he\U00012345lloworld")) == bdn::fromUtf32(U"he\U00012345lloworld"));

    SECTION("asciiEscaped")
    REQUIRE(Uri::unescape(bdn::fromUtf32((U"he\U00012345llo%40w%31%20%55orld"))) ==
            bdn::fromUtf32(U"he\U00012345llo@w1 Uorld"));

    SECTION("withEscapedPercent")
    REQUIRE(Uri::unescape(bdn::fromUtf32(U"he\U00012345llow%2531%25%25orld")) ==
            bdn::fromUtf32(U"he\U00012345llow%31%%orld"));

    SECTION("escapedUtf8")
    {
        String orig = bdn::fromUtf32(U"he\U00012345\U00002345lloworld");

        String escaped;
        for (char utf8Char : orig) {
            uint8_t el = (uint8_t)utf8Char;

            if (el >= 0x80) {
                escaped += "%";
                escaped += encodeHexDigit(el >> 4);
                escaped += encodeHexDigit(el & 0xf);
            } else
                escaped += (char32_t)el;
        }

        REQUIRE(Uri::unescape(escaped) == orig);
    }

    SECTION("invalidSequence") { REQUIRE(Uri::unescape("hell%1go") == "hell%1go"); }

    SECTION("invalidSequenceFollowedByNormalSequence") { REQUIRE(Uri::unescape("hell%1g%20o") == "hell%1g o"); }

    SECTION("invalidPercentFollowedByNormalSequence") { REQUIRE(Uri::unescape("hell%%20o") == "hell% o"); }

    SECTION("invalidPercentWithDigitFollowedByNormalSequence") { REQUIRE(Uri::unescape("hell%1%20o") == "hell%1 o"); }

    SECTION("sequenceAtEnd") { REQUIRE(Uri::unescape("hello%20") == "hello "); }

    SECTION("unfinishedSequenceAtEnd")
    {
        SECTION("noDigit")
        REQUIRE(Uri::unescape("hello%") == "hello%");

        SECTION("oneDigit")
        REQUIRE(Uri::unescape("hello%2") == "hello%2");
    }

    /*
     * TODO: Whats this supposed to do ?
    SECTION("invalidUtf8")
    {

        String s = Uri::unescape("hell%ff%feo");
        String sDec = bdn::fromUtf32(U"hell\ufffd\ufffdo");

        REQUIRE(s == sDec);
    }

    SECTION("invalidUtf8AtEnd") { REQUIRE(Uri::unescape("hell%ff") == bdn::fromUtf32(U"hell\ufffd")); }

    SECTION("unfinishedUtf8AtEnd") { REQUIRE(Uri::unescape("hell%e1") == bdn::fromUtf32(U"hell\ufffd")); }
    */
}

TEST_CASE("Uri")
{
    SECTION("unescape")
    testUnescape();
}
