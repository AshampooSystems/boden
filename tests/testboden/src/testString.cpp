#include <bdn/init.h>
#include <bdn/test.h>

#include "testStringImpl.h"

using namespace bdn;

template <class LeftType, class RightType>
inline void verifyGlobalConcatenation()
{
    String leftObj("hello");
    String rightObj("world");

    LeftType left = (LeftType)leftObj;
    RightType right = (RightType)rightObj;

    String result = left + right;

    verifyContents(result, U"helloworld");
}

template <class LeftType, class RightType>
inline void verifyGlobalConcatenationMoveFromLeft()
{
    String leftObj("hello");
    String rightObj("world");

    LeftType left = (LeftType)leftObj;
    RightType right = (RightType)rightObj;

    String result = std::move(left) + right;

    verifyContents(result, U"helloworld");

    verifyContents(left, U"");
    verifyContents(rightObj, U"world");
}

inline void testGlobalConcatenation()
{
    SECTION("string + string")
    verifyGlobalConcatenation<String, String>();

    SECTION("string + std::string")
    verifyGlobalConcatenation<String, std::string>();

    SECTION("string + std::wstring")
    verifyGlobalConcatenation<String, std::wstring>();

    SECTION("string + std::u16string")
    verifyGlobalConcatenation<String, std::u16string>();

    SECTION("string + std::u32string")
    verifyGlobalConcatenation<String, std::u16string>();

    SECTION("string + const char*")
    verifyGlobalConcatenation<String, const char *>();

    SECTION("string + const wchar_t*")
    verifyGlobalConcatenation<String, const wchar_t *>();

    SECTION("string + const char16_t*")
    verifyGlobalConcatenation<String, const char16_t *>();

    SECTION("string + const char32_t*")
    verifyGlobalConcatenation<String, const char32_t *>();

    SECTION("string&& + string")
    verifyGlobalConcatenationMoveFromLeft<String, String>();

    SECTION("string&& + std::string")
    verifyGlobalConcatenationMoveFromLeft<String, std::string>();

    SECTION("string&& + std::wstring")
    verifyGlobalConcatenationMoveFromLeft<String, std::wstring>();

    SECTION("string&& + std::u16string")
    verifyGlobalConcatenationMoveFromLeft<String, std::u16string>();

    SECTION("string&& + std::u32string")
    verifyGlobalConcatenationMoveFromLeft<String, std::u16string>();

    SECTION("string&& + const char*")
    verifyGlobalConcatenationMoveFromLeft<String, const char *>();

    SECTION("string&& + const wchar_t*")
    verifyGlobalConcatenationMoveFromLeft<String, const wchar_t *>();

    SECTION("string&& + const char16_t*")
    verifyGlobalConcatenationMoveFromLeft<String, const char16_t *>();

    SECTION("string&& + const char32_t*")
    verifyGlobalConcatenationMoveFromLeft<String, const char32_t *>();

    SECTION("std::string + string")
    verifyGlobalConcatenation<std::string, String>();

    SECTION("std::wstring + string")
    verifyGlobalConcatenation<std::wstring, String>();

    SECTION("std::u16string + string")
    verifyGlobalConcatenation<std::u16string, String>();

    SECTION("std::u32string + string")
    verifyGlobalConcatenation<std::u32string, String>();

    SECTION("const char* + string")
    verifyGlobalConcatenation<const char *, String>();

    SECTION("const wchar_t* + string")
    verifyGlobalConcatenation<const wchar_t *, String>();

    SECTION("const char16_t* + string")
    verifyGlobalConcatenation<const char16_t *, String>();

    SECTION("const char32_t* + string")
    verifyGlobalConcatenation<const char32_t *, String>();
}

template <class LeftType, class RightType> inline void verifyGlobalComparison()
{
    String hello("hello");
    String hello2("hello");
    String Hello("Hello");
    String world("world");

    REQUIRE((((LeftType)hello) < ((RightType)world)));
    REQUIRE((((LeftType)hello) <= ((RightType)world)));
    REQUIRE(!(((LeftType)hello) == ((RightType)world)));
    REQUIRE((((LeftType)hello) != ((RightType)world)));
    REQUIRE(!(((LeftType)hello) > ((RightType)world)));
    REQUIRE(!(((LeftType)hello) >= ((RightType)world)));

    REQUIRE(!(((LeftType)hello) < ((RightType)hello2)));
    REQUIRE((((LeftType)hello) <= ((RightType)hello2)));
    REQUIRE((((LeftType)hello) == ((RightType)hello2)));
    REQUIRE(!(((LeftType)hello) != ((RightType)hello2)));
    REQUIRE(!(((LeftType)hello) > ((RightType)hello2)));
    REQUIRE((((LeftType)hello) >= ((RightType)hello2)));

    REQUIRE(!(((LeftType)world) < ((RightType)hello)));
    REQUIRE(!(((LeftType)world) <= ((RightType)hello)));
    REQUIRE(!(((LeftType)world) == ((RightType)hello)));
    REQUIRE((((LeftType)world) != ((RightType)hello)));
    REQUIRE((((LeftType)world) > ((RightType)hello)));
    REQUIRE((((LeftType)world) >= ((RightType)hello)));

    REQUIRE((((LeftType)Hello) < ((RightType)hello)));
    REQUIRE((((LeftType)Hello) <= ((RightType)hello)));
    REQUIRE(!(((LeftType)Hello) == ((RightType)hello)));
    REQUIRE((((LeftType)Hello) != ((RightType)hello)));
    REQUIRE(!(((LeftType)Hello) > ((RightType)hello)));
    REQUIRE(!(((LeftType)Hello) >= ((RightType)hello)));
}

inline void testGlobalComparison()
{
    SECTION("string + string")
    verifyGlobalComparison<String, String>();

    SECTION("string + std::string")
    verifyGlobalComparison<String, std::string>();

    SECTION("string + std::wstring")
    verifyGlobalComparison<String, std::wstring>();

    SECTION("string + std::u16string")
    verifyGlobalComparison<String, std::u16string>();

    SECTION("string + std::u32string")
    verifyGlobalComparison<String, std::u16string>();

    SECTION("string + const char*")
    verifyGlobalComparison<String, const char *>();

    SECTION("string + const wchar_t*")
    verifyGlobalComparison<String, const wchar_t *>();

    SECTION("string + const char16_t*")
    verifyGlobalComparison<String, const char16_t *>();

    SECTION("string + const char32_t*")
    verifyGlobalComparison<String, const char32_t *>();

    SECTION("std::string + string")
    verifyGlobalComparison<std::string, String>();

    SECTION("std::wstring + string")
    verifyGlobalComparison<std::wstring, String>();

    SECTION("std::u16string + string")
    verifyGlobalComparison<std::u16string, String>();

    SECTION("std::u32string + string")
    verifyGlobalComparison<std::u32string, String>();

    SECTION("const char* + string")
    verifyGlobalComparison<const char *, String>();

    SECTION("const wchar_t* + string")
    verifyGlobalComparison<const wchar_t *, String>();

    SECTION("const char16_t* + string")
    verifyGlobalComparison<const char16_t *, String>();

    SECTION("const char32_t* + string")
    verifyGlobalComparison<const char32_t *, String>();
}

template <class CharType>
String fromStreamData(const std::basic_string<CharType> &data,
                      const std::locale &loc)
{
    return String(data);
}

template <>
String fromStreamData<char>(const std::basic_string<char> &data,
                            const std::locale &loc)
{
    return String::fromLocaleEncoding(data, loc);
}

template <class CharType>
std::basic_string<CharType> toStreamData(const String &s,
                                         const std::locale &loc)
{
    return (const std::basic_string<CharType> &)s;
}

template <>
std::basic_string<char> toStreamData<char>(const String &s,
                                           const std::locale &loc)
{
    // special version for char. This uses the locale encoding.
    return s.toLocaleEncoding(loc);
}

template <class CharType>
void verifyStringFromStream(const String &s, const String &expectedValue,
                            const std::locale &loc)
{
    REQUIRE(s == expectedValue);
}

template <>
void verifyStringFromStream<char>(const String &s, const String &expectedValue,
                                  const std::locale &loc)
{
    std::string streamData = toStreamData<char>(expectedValue, loc);

    String expectedValueAfterLocaleRoundTrip =
        fromStreamData<char>(streamData, loc);

    // sanity check
    REQUIRE(expectedValueAfterLocaleRoundTrip.getLength() >=
            expectedValue.getLength());

    REQUIRE(s == expectedValueAfterLocaleRoundTrip);
}

template <class CharType>
std::basic_string<CharType> makeInStreamData(const String &in)
{
    std::basic_string<CharType> streamData = toStreamData<CharType>(
        in, std::basic_ostringstream<CharType>().getloc());

    return streamData;
}

template <class CharType> inline void verifyStreamIntegration()
{
    SECTION("output")
    {
        std::basic_ostringstream<CharType> stream;
        String s(U"\U00012345hello");

        stream << s;

        std::basic_string<CharType> streamData = stream.str();

        REQUIRE(streamData == toStreamData<CharType>(s, stream.getloc()));

        String fromStream = fromStreamData(streamData, stream.getloc());
        verifyStringFromStream<CharType>(fromStream, s, stream.getloc());
    }

    SECTION("input")
    {
        String in(U"\U00012345hello world");

        std::basic_istringstream<CharType> stream(
            makeInStreamData<CharType>(in));

        String s;

        stream >> s;

        verifyStringFromStream<CharType>(s, U"\U00012345hello",
                                         stream.getloc());

        stream >> s;

        verifyStringFromStream<CharType>(s, U"world", stream.getloc());
    }

    SECTION("getLineFromStream-noDelim")
    {
        String in(U"\U00012345hello world\nbla gubbel");
        std::basic_istringstream<CharType> stream(
            makeInStreamData<CharType>(in));

        String s;

        getLineFromStream(stream, s);

        verifyStringFromStream<CharType>(s, U"\U00012345hello world",
                                         stream.getloc());

        getLineFromStream(stream, s);

        verifyStringFromStream<CharType>(s, U"bla gubbel", stream.getloc());
    }

    SECTION("getLineFromStream-delim")
    {
        String in(U"\U00012345hello world!bla gubbel");
        std::basic_istringstream<CharType> stream(
            makeInStreamData<CharType>(in));

        String s;

        getLineFromStream(stream, s, '!');

        verifyStringFromStream<CharType>(s, U"\U00012345hello world",
                                         stream.getloc());

        getLineFromStream(stream, s, '!');

        verifyStringFromStream<CharType>(s, U"bla gubbel", stream.getloc());
    }
}

inline void testStreamIntegration()
{
    SECTION("char")
    verifyStreamIntegration<char>();

    SECTION("wchar_t")
    verifyStreamIntegration<wchar_t>();
}

void testStdSwap()
{
    String a(U"\U00012345hello");
    String b(U"world");

    std::swap(a, b);

    REQUIRE(a == U"world");
    REQUIRE(b == U"\U00012345hello");
}

void testStdHash()
{
    String stringA(U"hello\U00012345world");
    String stringB(U"hello\U00012345worlX");

    size_t hashA = std::hash<String>()(stringA);
    size_t hashB = std::hash<String>()(stringB);

    REQUIRE(hashA != hashB);

    // verify that the top bytes of the hash are actually used.
    // Note that this can theoretically fail if the top two bytes
    // happen to be 0 by accident - but that is very unlikely.
    // And since the hashing is deterministic, we can adapt the test
    // and use a different input string if that happens.
    size_t top2Bytes = hashA;
    top2Bytes >>= (sizeof(hashA) - 2) * 8;
    top2Bytes &= 0xffff;

    REQUIRE(top2Bytes != 0);

    String stringC(U"xyzhello\U00012345worldxyz");
    String sliceStringA = stringC.subString(3, stringC.length() - 6);

    // sanity check
    REQUIRE(sliceStringA == stringA);

    size_t hashAFromSlice = std::hash<String>()(sliceStringA);

    REQUIRE(hashAFromSlice == hashA);
}

struct StringShiftOperatorTest_
{
};

template <typename CHAR_TYPE, typename TRAITS>
std::basic_ostream<CHAR_TYPE, TRAITS> &
operator<<(std::basic_ostream<CHAR_TYPE, TRAITS> &os,
           const StringShiftOperatorTest_ &o)
{
    return os << "shiftoptest";
}

TEST_CASE("String", "[String]")
{
    SECTION("globalConcatenation")
    testGlobalConcatenation();

    SECTION("globalComparison")
    testGlobalComparison();

    SECTION("streamIntegration")
    testStreamIntegration();

    SECTION("std::swap")
    testStdSwap();

    SECTION("std::hash")
    testStdHash();
}
