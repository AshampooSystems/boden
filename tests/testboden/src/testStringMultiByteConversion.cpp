#include <bdn/init.h>
#include <bdn/test.h>

#include "testStringImpl.h"

#include <iostream>

void verifyWideMultiByteConversion(const std::wstring &inWide,
                                   const std::string &multiByte,
                                   const std::wstring &outWide)
{
    // the back-converted string should have the same length at least.
    // Unencodable character should have been replaced with a replacement
    // character.

    // note that the wide versions should have the same lengths
    for (size_t i = 0; i < inWide.length(); i++) {
        wchar_t inChr = inWide[i];

        REQUIRE(i < outWide.length());

        wchar_t outChr = outWide[i];

        // we assume that ASCII characters are representable in the multibyte
        // encoding.
        if (inChr < 0x80) {
            REQUIRE(outChr == inChr);
        } else {
            // non-ASCII characters might have been replaced with one of the
            // replacement characters
            REQUIRE((outChr == inChr || outChr == U'\ufffd' || outChr == U'?'));
        }
    }

    REQUIRE(outWide.length() == inWide.length());
}

void verifyWideLocaleEncodingConversion(const std::wstring &inWide)
{
    std::string multiByte;
    std::wstring outWide;

    SECTION("defaultLocale")
    {
        multiByte = wideToLocaleEncoding(inWide);
        outWide = localeEncodingToWide(multiByte);

        verifyWideMultiByteConversion(inWide, multiByte, outWide);
    }

    SECTION("globalLocale")
    {
        multiByte = wideToLocaleEncoding(inWide, std::locale());
        outWide = localeEncodingToWide(multiByte, std::locale());

        verifyWideMultiByteConversion(inWide, multiByte, outWide);
    }

    SECTION("classicLocale")
    {
        multiByte = wideToLocaleEncoding(inWide, std::locale::classic());
        outWide = localeEncodingToWide(multiByte, std::locale::classic());

        verifyWideMultiByteConversion(inWide, multiByte, outWide);
    }
}

void verifyWideUtf8EncodingConversion(const std::wstring &inWide)
{
    std::string utf8;
    std::wstring outWide;

    utf8 = wideToUtf8(inWide);

    REQUIRE(String(utf8) == String(inWide));

    outWide = utf8ToWide(utf8);

    REQUIRE(outWide == inWide);
}

void verifyWideConversion(const std::wstring &inWide)
{
    SECTION("utf8")
    verifyWideUtf8EncodingConversion(inWide);

    SECTION("localeEncoding")
    verifyWideLocaleEncodingConversion(inWide);
}

TEST_CASE("wideStringConversion")
{

    struct SubTestData
    {
        std::wstring wide;
        const char *desc;
    };

    SubTestData allData[] = {
        {L"", "empty"},
        // note that gcc has a bug. \u0000 is represented as 1, not 0.
        // Use \0 instead.
        {std::wstring(L"\0", 1), "zero char"},
        {std::wstring(L"he\0llo", 6), "zero char in middle"},
        {L"h", "ascii char"},
        {L"hx", "ascii 2 chars"},
        {L"\u0345", "non-ascii below surrogate range"},
        {L"\U00010437", "surrogate range A"},
        {L"\U00024B62", "surrogate range B"},
        {L"\uE000", "above surrogate range A"},
        {L"\uF123", "above surrogate range B"},
        {L"\uFFFF", "above surrogate range C"}};

    int dataCount = std::extent<decltype(allData)>().value;

    for (int t = 0; t < dataCount; t++) {
        SubTestData *pCurrData = &allData[t];

        SECTION(pCurrData->desc) { verifyWideConversion(pCurrData->wide); }

        SECTION(std::string(pCurrData->desc) + " mixed")
        {
            verifyWideConversion(L"hello" + std::wstring(pCurrData->wide) +
                                 L"wo" + std::wstring(pCurrData->wide) +
                                 std::wstring(pCurrData->wide) + L"rld");
        }
    }
}
