#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/LocaleDecoder.h>
#include <bdn/localeUtil.h>

using namespace bdn;

void verifyLocaleDecoderWithLocale(const std::wstring &wide, const std::locale &loc, bool knowSupportsUnicode)
{
    std::string localeEncoded = wideToLocaleEncoding(wide, loc);

    LocaleDecoder decoder(localeEncoded.c_str(), localeEncoded.length(), loc);

    LocaleDecoder::Iterator localeCurr = decoder.begin();
    LocaleDecoder::Iterator localeEnd = decoder.end();

    LocaleDecoder::Iterator lastItCopy = localeCurr;
    REQUIRE(lastItCopy == localeCurr);

    WideCodec::DecodingIterator<std::wstring::const_iterator> expectedIt(wide.begin(), wide.begin(), wide.end());
    WideCodec::DecodingIterator<std::wstring::const_iterator> expectedEnd(wide.end(), wide.begin(), wide.end());

    while (expectedIt != expectedEnd) {
        lastItCopy = localeCurr;
        REQUIRE(checkEquality(lastItCopy, localeCurr, true));

        REQUIRE(checkEquality(localeCurr, localeEnd, false));

        char32_t expectedChr = *expectedIt;
        char32_t actualChr = *localeCurr;

        // accessing the character might have changed the internal state.
        // verify the equality again
        REQUIRE(checkEquality(lastItCopy, localeCurr, true));

        if (knowSupportsUnicode || expectedChr <= 128)
            REQUIRE(actualChr == expectedChr);
        else
            REQUIRE((actualChr == expectedChr || actualChr == 0xfffd || actualChr == '?'));

        ++localeCurr;

        if (actualChr != expectedChr) {
            // we had an error. Multiple errors chars may have been inserted if
            // the problem input wchar_t was a surrogate pair.
            if (sizeof(wchar_t) == 2 && expectedChr >= 0x10000) {
                // should have gotten two replacement chars. Skip over the
                // second one
                REQUIRE(checkEquality(localeCurr, localeEnd, false));
                char32_t followingChar = *localeCurr;
                REQUIRE(followingChar == actualChr);
                ++localeCurr;
            }
        }

        // verify that copying the iterator assignment/copying as expected

        LocaleDecoder::Iterator itCopy = localeCurr;

        checkEquality(itCopy, localeCurr, true);
        checkEquality(itCopy, lastItCopy, false);
        checkEquality(localeCurr, lastItCopy, false);

        ++expectedIt;
    }

    REQUIRE(checkEquality(localeCurr, localeEnd, true));
}

void verifyLocaleDecoder(const std::wstring &wide)
{
    SECTION("classic locale")
    verifyLocaleDecoderWithLocale(wide, std::locale::classic(), false);

    SECTION("classic utf8 locale")
    verifyLocaleDecoderWithLocale(wide, deriveUtf8Locale(std::locale::classic()), true);

    SECTION("global locale")
    verifyLocaleDecoderWithLocale(wide, deriveUtf8Locale(std::locale()), false);
}

TEST_CASE("LocaleDecoder")
{

    struct LocaleDecoderSubTestData
    {
        std::wstring wide;
        const char *desc;
    };

    LocaleDecoderSubTestData allData[] = {{L"", "empty"},
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
        LocaleDecoderSubTestData *currData = &allData[t];

        SECTION(currData->desc) { verifyLocaleDecoder(currData->wide); }

        SECTION(std::string(currData->desc) + " mixed")
        {
            verifyLocaleDecoder(L"hello" + std::wstring(currData->wide) + L"wo" + std::wstring(currData->wide) +
                                std::wstring(currData->wide) + L"rld");
        }
    }
}
