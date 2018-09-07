#include <bdn/init.h>
#include <bdn/Utf16Codec.h>

#include <bdn/test.h>

#include "codecIteratorUtil.h"

namespace bdn
{

    template <typename VALUE, class CATEGORY> class DummyUtf16TestIterator_
    {
      public:
        using iterator_category = CATEGORY;
        using value_type = VALUE;
        using difference_type = size_t;
        using pointer = VALUE *;
        using reference = const VALUE &;

        DummyUtf16TestIterator_ &operator++() { return *this; }

        DummyUtf16TestIterator_ operator++(int)
        {
            return DummyUtf16TestIterator_();
        }

        reference operator*() const { return *(VALUE *)nullptr; }
    };

    TEST_CASE("Utf16Codec", "[string]")
    {
        SECTION("iterator category")
        {
            SECTION("decoding")
            {
                SECTION("inner input")
                {
                    REQUIRE(typeid(typename Utf16Codec::DecodingIterator<
                                   DummyUtf16TestIterator_<
                                       char16_t, std::input_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::input_iterator_tag));
                }

                SECTION("inner forward")
                {
                    REQUIRE(typeid(typename Utf16Codec::DecodingIterator<
                                   DummyUtf16TestIterator_<
                                       char16_t, std::forward_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::forward_iterator_tag));
                }

                SECTION("inner bidir")
                {
                    REQUIRE(
                        typeid(typename Utf16Codec::DecodingIterator<
                               DummyUtf16TestIterator_<
                                   char16_t, std::bidirectional_iterator_tag>>::
                                   iterator_category) ==
                        typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner random")
                {
                    // the result should still be bidir, since we do not support
                    // random access for utf-16
                    REQUIRE(
                        typeid(typename Utf16Codec::DecodingIterator<
                               DummyUtf16TestIterator_<
                                   char16_t, std::random_access_iterator_tag>>::
                                   iterator_category) ==
                        typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner pointer")
                {
                    REQUIRE(typeid(typename Utf16Codec::DecodingIterator<
                                   char16_t *>::iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }
            }

            SECTION("encoding")
            {
                SECTION("inner input")
                {
                    REQUIRE(typeid(typename Utf16Codec::EncodingIterator<
                                   DummyUtf16TestIterator_<
                                       char32_t, std::input_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::input_iterator_tag));
                }

                SECTION("inner forward")
                {
                    REQUIRE(typeid(typename Utf16Codec::EncodingIterator<
                                   DummyUtf16TestIterator_<
                                       char32_t, std::forward_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::forward_iterator_tag));
                }

                SECTION("inner bidir")
                {
                    REQUIRE(
                        typeid(typename Utf16Codec::EncodingIterator<
                               DummyUtf16TestIterator_<
                                   char32_t, std::bidirectional_iterator_tag>>::
                                   iterator_category) ==
                        typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner random")
                {
                    // the result should still be bidir, since we do not support
                    // random access for utf-16
                    REQUIRE(
                        typeid(typename Utf16Codec::EncodingIterator<
                               DummyUtf16TestIterator_<
                                   char32_t, std::random_access_iterator_tag>>::
                                   iterator_category) ==
                        typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner pointer")
                {
                    REQUIRE(typeid(typename Utf16Codec::EncodingIterator<
                                   char32_t *>::iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }
            }
        }

        SECTION("decoding")
        {
            struct SubTestData
            {
                std::u16string utf16;
                std::u32string expectedDecoded;
                const char *desc;
            };

            // \uFFFF yields and incorrect string with G++ 4.8. So we work
            // around it.
            char16_t u16ffff[2] = {0xffff, 0};
            char32_t u32ffff[2] = {0xffff, 0};

            SubTestData allData[] = {
                {u"", U"", "empty"},
                // note that gcc has a bug. \u0000 is represented as 1, not 0.
                // Use \0 instead.
                {std::u16string(u"\0", 1), std::u32string(U"\0", 1),
                 "zero char"},
                {u"h", U"h", "ascii char"},
                {u"hx", U"hx", "ascii 2 chars"},
                {u"\u0345", U"\u0345", "non-ascii below surrogate range"},
                {u"\U00010437", U"\U00010437", "surrogate pair A"},
                {u"\U00024B62", U"\U00024B62", "surrogate pair B"},
                {u"\uE000", U"\uE000", "above surrogate range A"},
                {u"\uF123", U"\uF123", "above surrogate range B"},

                // \uFFFF yields and incorrect string with G++ 4.8. So we work
                // around it.
                {u16ffff, u32ffff, "above surrogate range C"},

                {u"\xD801", U"\ufffd", "only high surrogate"},
                {u"\xDC37", U"\ufffd", "only low surrogate"},

                {u"\xD801\U00024B62", U"\ufffd\U00024B62",
                 "high surrogate at start, pair follows"},
                {u"\xD801\u0345", U"\ufffd\u0345",
                 "high surrogate at start, single follows"},
                {u"\U00024B62\xD801", U"\U00024B62\ufffd",
                 "pair, high surrogate at end"},
                {u"\u0345\xD801", U"\u0345\ufffd",
                 "single, high surrogate at end"},
                {u"\U00024B62\xD801\U00010437", U"\U00024B62\ufffd\U00010437",
                 "high surrogate between pairs"},
                {u"\u0345\xD801\u0567", U"\u0345\ufffd\u0567",
                 "high surrogate between singles"},

                {u"\xDC37\U00024B62", U"\ufffd\U00024B62",
                 "low surrogate at start, pair follows"},
                {u"\xDC37\u0345", U"\ufffd\u0345",
                 "low surrogate at start, single follows"},
                {u"\U00024B62\xDC37", U"\U00024B62\ufffd",
                 "pair, low surrogate at end"},
                {u"\u0345\xDC37", U"\u0345\ufffd",
                 "single, low surrogate at end"},
                {u"\U00024B62\xDC37\U00010437", U"\U00024B62\ufffd\U00010437",
                 "low surrogate between pairs"},
                {u"\u0345\xDC37\u0567", U"\u0345\ufffd\u0567",
                 "low surrogate between singles"},
            };

            int dataCount = std::extent<decltype(allData)>().value;

            for (int dataIndex = 0; dataIndex < dataCount; dataIndex++) {
                SubTestData *pCurrData = &allData[dataIndex];

                SECTION(pCurrData->desc)
                {
                    std::u16string encoded(pCurrData->utf16);
                    std::u32string expectedDecoded(pCurrData->expectedDecoded);

                    testCodecDecodingIterator<Utf16Codec>(encoded,
                                                          expectedDecoded);
                }
            }
        }

        SECTION("encoding")
        {
            struct SubTestData
            {
                std::u32string input;
                std::u16string expectedUtf16;
                const char *desc;

                int inputChars;
            };

            // \U0000e000 yields and incorrect string with G++ 4.8. So we work
            // around it.
            char16_t u16e000[2] = {0xe000, 0};
            char32_t u32e000[2] = {0xe000, 0};

            // same with \U0000ffff
            char16_t u16ffff[2] = {0xffff, 0};
            char32_t u32ffff[2] = {0xffff, 0};

            SubTestData allData[] = {
                {U"", u"", "empty"},
                {std::u32string(U"\0", 1), std::u16string(u"\x00", 1),
                 "zero char"},
                {U"h", u"h", "ascii char"},
                {U"hx", u"hx", "ascii 2 chars"},
                {U"\u0181", u"\u0181", "non-ascii single"},
                {U"\u0181\u0810", u"\u0181\u0810", "two singles"},
                // \U0000e000 yields and incorrect string with G++ 4.8. So we
                // work around it.
                {u32e000, u16e000, "single above surrogate range A"},
                {u32ffff, u16ffff, "single above surrogate range B"},

                // MSVC in Visual Studio 2015 has a bug that causes it to NOT
                // encode U00010000 as a pair. Instead it encodes it as 0x0001
                // 0x0000. So we have to specify the encoded version manually
                {U"\U00010000", u"\xd800\xdc00", "pair A"},

                {U"\U00054321", u"\U00054321", "pair B"},

                {U"\U0010FFFF", u"\U0010FFFF", "pair C"},

                {U"\xd800", u"\ufffd", "high surrogate A"},
                {U"\xdbff", u"\ufffd", "high surrogate B"},
                {U"\xdc00", u"\ufffd", "low surrogate A"},
                {U"\xdfff", u"\ufffd", "low surrogate B"},

                {U"\x00110000", u"\ufffd", "unencodable A"},
                {U"\x00234567", u"\ufffd", "unencodable B"},
                {U"\xffffffff", u"\ufffd", "unencodable C"},
            };

            int dataCount = std::extent<decltype(allData)>().value;

            for (int dataIndex = 0; dataIndex < dataCount; dataIndex++) {
                SubTestData *pCurrData = &allData[dataIndex];

                SECTION(pCurrData->desc)
                {
                    testCodecEncodingIterator<Utf16Codec>(
                        pCurrData->input, pCurrData->expectedUtf16);
                }
            }
        }
    }
}
