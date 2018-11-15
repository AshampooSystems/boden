#include <bdn/init.h>

#include <bdn/test.h>

#include <algorithm>

using namespace bdn;

TEST_CASE("getStringEndPtr")
{
    const char *s = "hello";

    REQUIRE(getStringEndPtr(s) == s + 5);
    REQUIRE(getStringEndPtr(s, 0) == s);
    REQUIRE(getStringEndPtr(s, 5) == s + 5);
    REQUIRE(getStringEndPtr(s, 4) == s + 4);
}

template <class CODEC> inline void verifyContents(StringData<CODEC> &data, const std::u32string &expectedResult)
{
    REQUIRE(checkEquality(data.begin(), data.end(), expectedResult.empty()));

    std::u32string result(data.begin(), data.end());
    REQUIRE(result == expectedResult);

    // also test reverse iteration
    std::u32string reverseResult;
    auto it = data.end();

    while (it != data.begin()) {
        it--;
        reverseResult += *it;
    }

    std::u32string expectedReverseResult = expectedResult;
    std::reverse(expectedReverseResult.begin(), expectedReverseResult.end());

    REQUIRE(reverseResult == expectedReverseResult);
}

template <class CODEC> void testConstruct()
{
    SECTION("empty")
    {
        StringData<CODEC> data;

        verifyContents<CODEC>(data, U"");
    }

    SECTION("fromUTF8-noLength")
    {
        StringData<CODEC> data("hello");

        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUTF8-withLength")
    {
        StringData<CODEC> data("helloxyz", 5);
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUTF8-stdString")
    {
        StringData<CODEC> data(std::string("hello"));
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUTF16-noLength")
    {
        StringData<CODEC> data(u"hello");
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUTF16-withLength")
    {
        StringData<CODEC> data(u"helloxyz", 5);
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUTF16-stdString")
    {
        StringData<CODEC> data(std::u16string(u"hello"));
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUTF32-noLength")
    {
        StringData<CODEC> data(U"hello");
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUTF32-withLength")
    {
        StringData<CODEC> data(U"helloxyz", 5);
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUTF32-stdString")
    {
        StringData<CODEC> data(std::u32string(U"hello"));
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromUnicodeIterators")
    {
        std::u32string s = U"hello";

        StringData<CODEC> data(s.begin(), s.end());
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromEncodedIterators-Utf8")
    {
        std::string s = "hello";

        StringData<CODEC> data(Utf8Codec(), s.begin(), s.end());
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromEncodedIterators-Utf16")
    {
        std::u16string s = u"hello";

        StringData<CODEC> data(Utf16Codec(), s.begin(), s.end());
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromEncodedIterators-Utf32")
    {
        std::u32string s = U"hello";

        StringData<CODEC> data(Utf32Codec(), s.begin(), s.end());
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromEncodedPtrs-Utf8")
    {
        const char *s = "hello";

        StringData<CODEC> data(Utf8Codec(), s, s + 5);
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromEncodedPtrs-Utf16")
    {
        const char16_t *s = u"hello";

        StringData<CODEC> data(Utf16Codec(), s, s + 5);
        verifyContents<CODEC>(data, U"hello");
    }

    SECTION("fromEncodedPtrs-Utf32")
    {
        const char32_t *s = U"hello";

        StringData<CODEC> data(Utf32Codec(), s, s + 5);
        verifyContents<CODEC>(data, U"hello");
    }
}

template <class CODEC>
inline void verifyEncodedContents(const typename CODEC::EncodedString &encodedString,
                                  const std::u32string &expectedContents)
{
    // verify the contents

    typename CODEC::template DecodingIterator<typename CODEC::EncodedString::const_iterator> beginIt(
        encodedString.begin(), encodedString.begin(), encodedString.end());
    typename CODEC::template DecodingIterator<typename CODEC::EncodedString::const_iterator> endIt(
        encodedString.end(), encodedString.begin(), encodedString.end());

    std::u32string decResult(beginIt, endIt);

    REQUIRE(decResult == expectedContents);
}

template <class CODEC> inline void testEncodedContents()
{
    typename CODEC::EncodedString encodedString;

    StringData<CODEC> data("hello");

    SECTION("implicit conversion to std string")
    {
        encodedString = data;
        verifyEncodedContents<CODEC>(encodedString, U"hello");
    }

    SECTION("asStd")
    {
        encodedString = data.getEncodedString();
        verifyEncodedContents<CODEC>(encodedString, U"hello");
    }

    SECTION("asPtr")
    {
        encodedString = data.asPtr();
        verifyEncodedContents<CODEC>(encodedString, U"hello");
    }
}

template <class CODEC> inline void testStringData()
{
    SECTION("construct") { testConstruct<CODEC>(); }

    SECTION("encodedContents") { testEncodedContents<CODEC>(); }

    SECTION("assign")
    {
        StringData<CODEC> data("hello");
        StringData<CODEC> data2("world");

        data = data2;

        std::u32string s(data.begin(), data.end());

        REQUIRE(s == U"world");
    }

    SECTION("globalEmpty")
    {
        P<StringData<CODEC>> pData = &StringData<CODEC>::getEmptyData();
        P<StringData<CODEC>> pData2 = &StringData<CODEC>::getEmptyData();

        REQUIRE(pData == pData2);
    }

    SECTION("iterator")
    {
        StringData<CODEC> data("helloworld");

        typename StringData<CODEC>::Iterator it = data.begin();

        REQUIRE(*it == U'h');

        it++;

        REQUIRE(*it == U'e');

        it--;

        REQUIRE(*it == U'h');

        ++it;

        REQUIRE(*it == U'e');

        --it;

        REQUIRE(*it == U'h');

        it += 4;

        REQUIRE(*it == U'o');

        it -= 3;

        REQUIRE(*it == U'e');

        typename StringData<CODEC>::Iterator it2 = it + 6;

        REQUIRE(*it == U'e');
        REQUIRE(*it2 == U'r');

        typename StringData<CODEC>::Iterator it3 = it2 - 6;

        REQUIRE(*it2 == U'r');
        REQUIRE(*it3 == U'e');

        REQUIRE(checkEquality(it3, it, true));
        REQUIRE(checkEquality(it, it2, false));
    }
}

TEST_CASE("StringData")
{
    SECTION("Utf8") { testStringData<Utf8Codec>(); }

    SECTION("Utf16") { testStringData<Utf16Codec>(); }

    SECTION("Utf32") { testStringData<Utf32Codec>(); }

    SECTION("Wchar") { testStringData<WideCodec>(); }
}
