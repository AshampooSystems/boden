#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/LocaleEncoder.h>
#include <bdn/localeUtil.h>

using namespace bdn;

void verifyLocaleEncoderWithLocale(const String &inString, const std::locale &loc, bool knowIsUtf8,
                                   bool containsSurrogatePairCharacters)
{
    std::string expected;

    // if we know that the codec is UTF-8 then we use our own codec
    // implementation to generate the expected result. BUT we only do that if
    // the string does not contain characters that are encoded as surrogate
    // pairs in the wchar representation. The reason is that the standard
    // library codec often does not handle surrogate pairs in the input data
    // correctly and will generate two separate UTF-8 sequences for each pair
    // element. That is something that we cannot prevent, since the locale codec
    // is simply buggy. So we must adapt our own expected string accordingly. We
    // do that by using wideToLocaleEncoding to generate it (which also uses the
    // codec).
    if (knowIsUtf8 && (!containsSurrogatePairCharacters || sizeof(wchar_t) != 2))
        expected = inString.asUtf8();
    else {
        expected = wideToLocaleEncoding(inString.asWide(), loc);

        // the test strings consist of either just a single character,
        // or helloCHARwoCHARCHARrld

        // If CHAR is a unicode character in the form of a surrogate pair
        // then we might get two unencodable wide char elements for a single
        // character, resulting in double ? in the expected string.
        // Since the locale encoder uses UTF-32 as input, that character counts
        // only as a single element and will result in a single ? there when it
        // is not encodable.
        // So, for our test to work we need to replace such double ? in expected
        // with single ?. BUT we cannot simply replace any ?? with ?, since the
        // unicode character may also occur twice in the input. So instead we
        // use our knowledge of the input string and replace
        // ?? with ? (whole string), hello?? with hello? and wo???? with wo??.

        if (expected == "??")
            expected = "?";

        size_t found = expected.find("hello??");
        if (found != std::string::npos) {
            // erase one of the ?
            expected.erase(expected.begin() + found + 5);
        }

        found = expected.find("wo????");
        if (found != std::string::npos) {
            // erase two of the ?
            expected.erase(expected.begin() + found + 2);
            expected.erase(expected.begin() + found + 2);
        }
    }

    LocaleEncoder<String::Iterator> encoder(inString.begin(), inString.end(), loc);

    LocaleEncoder<String::Iterator>::Iterator localeCurr = encoder.begin();
    LocaleEncoder<String::Iterator>::Iterator localeEnd = encoder.end();

    LocaleEncoder<String::Iterator>::Iterator lastItCopy = localeCurr;
    checkEquality(lastItCopy, localeCurr, true);

    std::string::const_iterator expectedIt = expected.begin();
    std::string::const_iterator expectedEnd = expected.end();

    while (expectedIt != expectedEnd) {
        lastItCopy = localeCurr;
        REQUIRE(checkEquality(lastItCopy, localeCurr, true));

        REQUIRE(checkEquality(localeCurr, localeEnd, false));

        char expectedChr = *expectedIt;
        char actualChr = *localeCurr;

        // accessing the character might have changed the internal state.
        // verify the equality again
        REQUIRE(checkEquality(lastItCopy, localeCurr, true));

        REQUIRE(actualChr == expectedChr);

        ++localeCurr;

        // verify that copying the iterator assignment/copying as expected

        LocaleEncoder<String::Iterator>::Iterator itCopy = localeCurr;

        checkEquality(itCopy, localeCurr, true);
        checkEquality(itCopy, lastItCopy, false);
        checkEquality(localeCurr, lastItCopy, false);

        ++expectedIt;
    }

    REQUIRE(checkEquality(localeCurr, localeEnd, true));
}

void verifyLocaleEncoder(const String &inString, bool containsSurrogatePairCharacters)
{
    SECTION("classic locale")
    verifyLocaleEncoderWithLocale(inString, std::locale::classic(), false, containsSurrogatePairCharacters);

    SECTION("classic utf8 locale")
    verifyLocaleEncoderWithLocale(inString, deriveUtf8Locale(std::locale::classic()), true,
                                  containsSurrogatePairCharacters);

    SECTION("global locale")
    verifyLocaleEncoderWithLocale(inString, std::locale(), false, containsSurrogatePairCharacters);
}

TEST_CASE("LocaleEncoder")
{
    struct LocaleEncoderSubTestData
    {
        LocaleEncoderSubTestData(String inString, const char *desc, bool containsSurrogatePairCharacters = false)
            : inString(inString), desc(desc), containsSurrogatePairCharacters(containsSurrogatePairCharacters)
        {}

        String inString;
        const char *desc;
        bool containsSurrogatePairCharacters;
    };

    LocaleEncoderSubTestData allData[] = {{L"", "empty"},
                                          // note that gcc has a bug. \u0000 is represented as 1, not 0.
                                          // Use \0 instead.
                                          {String(L"\0", 1), "zero char"},
                                          {String(L"he\0llo", 6), "zero char in middle"},
                                          {L"h", "ascii char"},
                                          {L"hx", "ascii 2 chars"},
                                          {L"\u0345", "non-ascii below surrogate range"},
                                          {L"\U00010437", "surrogate range A", true},
                                          {L"\U00024B62", "surrogate range B", true},
                                          {L"\uE000", "above surrogate range A"},
                                          {L"\uF123", "above surrogate range B"},
                                          {L"\uFFFF", "above surrogate range C"}};

    int dataCount = std::extent<decltype(allData)>().value;

    for (int t = 0; t < dataCount; t++) {
        LocaleEncoderSubTestData *currData = &allData[t];

        SECTION(currData->desc) { verifyLocaleEncoder(currData->inString, currData->containsSurrogatePairCharacters); }

        SECTION(std::string(currData->desc) + " mixed")
        {
            verifyLocaleEncoder(L"hello" + currData->inString + L"wo" + currData->inString + currData->inString +
                                    L"rld",
                                currData->containsSurrogatePairCharacters);
        }
    }
}
