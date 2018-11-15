#include <bdn/init.h>
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
    REQUIRE(Uri::unescape(U"he\U00012345lloworld") == U"he\U00012345lloworld");

    SECTION("asciiEscaped")
    REQUIRE(Uri::unescape(U"he\U00012345llo%40w%31%20%55orld") == U"he\U00012345llo@w1 Uorld");

    SECTION("withEscapedPercent")
    REQUIRE(Uri::unescape(U"he\U00012345llow%2531%25%25orld") == U"he\U00012345llow%31%%orld");

    SECTION("escapedUtf8")
    {
        String orig = U"he\U00012345\U00002345lloworld";
        std::string origUtf8 = orig.asUtf8();

        String escaped;
        for (char utf8Char : origUtf8) {
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

    SECTION("invalidUtf8") { REQUIRE(Uri::unescape("hell%ff%feo") == U"hell\ufffd\ufffdo"); }

    SECTION("invalidUtf8AtEnd") { REQUIRE(Uri::unescape("hell%ff") == U"hell\ufffd"); }

    SECTION("unfinishedUtf8AtEnd") { REQUIRE(Uri::unescape("hell%e1") == U"hell\ufffd"); }
}

TEST_CASE("Uri")
{
    SECTION("unescape")
    testUnescape();
}
