#include <bdn/init.h>
#include <bdn/Utf8Codec.h>

#include <bdn/test.h>

#include "codecIteratorUtil.h"

namespace bdn
{

    template <typename VALUE, class CATEGORY> class DummyUtf8TestIterator_
    {
      public:
        using iterator_category = CATEGORY;
        using value_type = VALUE;
        using difference_type = size_t;
        using pointer = VALUE *;
        using reference = const VALUE &;

        DummyUtf8TestIterator_ &operator++() { return *this; }

        DummyUtf8TestIterator_ operator++(int) { return DummyUtf8TestIterator_(); }

        reference operator*() const { return *(VALUE *)nullptr; }
    };

    static void testUtf8CodecDecodeChar(const std::string &utf8, const std::u32string &expectedDecoded)
    {
        std::u32string decoded;

        auto it = utf8.begin();
        while (it != utf8.end()) {
            auto itBefore = it;
            char32_t chr = Utf8Codec::decodeChar(it, utf8.end());

            if (chr == 0xfffd) {
                // it should be at one byte after itBefore
                auto expectedIt = itBefore;
                ++expectedIt;

                REQUIRE(it == expectedIt);
            }

            decoded += chr;
        }

        REQUIRE(decoded == expectedDecoded);
    }

    TEST_CASE("Utf8Codec", "[string]")
    {

        SECTION("iterator category")
        {
            SECTION("decoding")
            {
                SECTION("inner input")
                {
                    REQUIRE(typeid(typename Utf8Codec::DecodingIterator<
                                   DummyUtf8TestIterator_<uint8_t, std::input_iterator_tag>>::iterator_category) ==
                            typeid(std::input_iterator_tag));
                }

                SECTION("inner forward")
                {
                    REQUIRE(typeid(typename Utf8Codec::DecodingIterator<
                                   DummyUtf8TestIterator_<char, std::forward_iterator_tag>>::iterator_category) ==
                            typeid(std::forward_iterator_tag));
                }

                SECTION("inner bidir")
                {
                    REQUIRE(typeid(typename Utf8Codec::DecodingIterator<
                                   DummyUtf8TestIterator_<char, std::bidirectional_iterator_tag>>::iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner random")
                {
                    // the result should still be bidir, since we do not support
                    // random access for utf-16
                    REQUIRE(typeid(typename Utf8Codec::DecodingIterator<
                                   DummyUtf8TestIterator_<char, std::random_access_iterator_tag>>::iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner pointer")
                {
                    REQUIRE(typeid(typename Utf8Codec::DecodingIterator<char *>::iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }
            }

            SECTION("encoding")
            {
                SECTION("inner input")
                {
                    REQUIRE(typeid(typename Utf8Codec::EncodingIterator<
                                   DummyUtf8TestIterator_<char32_t, std::input_iterator_tag>>::iterator_category) ==
                            typeid(std::input_iterator_tag));
                }

                SECTION("inner forward")
                {
                    REQUIRE(typeid(typename Utf8Codec::EncodingIterator<
                                   DummyUtf8TestIterator_<char32_t, std::forward_iterator_tag>>::iterator_category) ==
                            typeid(std::forward_iterator_tag));
                }

                SECTION("inner bidir")
                {
                    REQUIRE(
                        typeid(typename Utf8Codec::EncodingIterator<
                               DummyUtf8TestIterator_<char32_t, std::bidirectional_iterator_tag>>::iterator_category) ==
                        typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner random")
                {
                    // the result should still be bidir, since we do not support
                    // random access for utf-16
                    REQUIRE(
                        typeid(typename Utf8Codec::EncodingIterator<
                               DummyUtf8TestIterator_<char32_t, std::random_access_iterator_tag>>::iterator_category) ==
                        typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner pointer")
                {
                    REQUIRE(typeid(typename Utf8Codec::EncodingIterator<char32_t *>::iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }
            }
        }

        SECTION("decoding")
        {
            struct SubTestData
            {
                std::string utf8;
                std::u32string expectedDecoded;
                const char *desc;
            };

            SubTestData allData[] = {
                {"", U"", "empty"},
                // note that gcc has a bug. \u0000 is represented as 1, not 0.
                // Use \0 instead.
                {std::string("\0", 1), std::u32string(U"\0", 1), "zero char"},
                {"h", U"h", "ascii char"},
                {"hi", U"hi", "ascii 2 chars"},
                {u8"\u0181", U"\u0181", "2 byte"},
                {u8"\u0810", U"\u0810", "3 bytes"},
                {u8"\U00010010", U"\U00010010", "4 bytes"},
                // C++ literals do not recognize characters in the 5 and 6 byte
                // range. So we specify the encoded version manually
                {"\xF8\x88\x80\x80\x90", U"\x00200010", "5 bytes"},
                {"\xFC\x84\x80\x80\x80\x90", U"\x04000010", "6 bytes"},

                {"\xc2", U"\ufffd", "byte missing in 2 byte sequence"},
                {"\xe2", U"\ufffd", "two bytes missing in 3 byte sequence"},
                {"\xe2\x82", U"\ufffd\ufffd", "byte missing in 3 byte sequence"},
                {"\x82\xa2", U"\ufffd\ufffd", "only top bit set in start byte"},
                {"\xfe\xa2\xa2\xa2\xa2\xa2\xa2", U"\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd",
                 "top 7 bits set in start byte"},
                {"\xff\xa2\xa2\xa2\xa2\xa2\xa2\xa2", U"\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd",
                 "all bits set in start byte"},
                {"\xc2hello", U"\ufffdhello", "byte missing at start of string"},
                {"hello\xc2", U"hello\ufffd", "byte missing at end of string"},
                {"hell\xc2o", U"hell\ufffdo", "byte missing in middle of string"},

                {"\xE0\xA4\x92", U"\u0912", "valid 3 byte sequence"},
                {"\xE0\xA4\x12", U"\ufffd\ufffd\u0012", "last byte ascii in 3 byte sequence"},
                {"\xE0\xA4\xd2", U"\ufffd\ufffd\ufffd", "last byte two top bits set in 3 byte sequence"},
                {"\xE0\x24\x92", U"\ufffd\u0024\ufffd", "middle byte ascii in 3 byte sequence"},
                {"\xE0\xe4\x92", U"\ufffd\ufffd\ufffd", "middle byte two top bits set in 3 byte sequence"},

                {"\xc2\xe0\xa0\x90\xc2", U"\ufffd\u0810\ufffd", "valid sequence sandwiched between bad sequences"},
            };

            int dataCount = std::extent<decltype(allData)>().value;

            for (int dataIndex = 0; dataIndex < dataCount; dataIndex++) {
                SubTestData *currData = &allData[dataIndex];

                SECTION(currData->desc)
                {
                    std::string encoded(currData->utf8);
                    std::u32string expectedDecoded(currData->expectedDecoded);

                    SECTION("decodeChar")
                    testUtf8CodecDecodeChar(encoded, expectedDecoded);

                    SECTION("iterator")
                    testCodecDecodingIterator<Utf8Codec>(encoded, expectedDecoded);
                }
            }

            SECTION("decodeChar on empty data")
            {
                std::string empty;

                auto it = empty.begin();
                char32_t chr = Utf8Codec::decodeChar(it, empty.end());

                REQUIRE(chr == 0xfffd);

                REQUIRE(it == empty.end());
            }
        }

        SECTION("encoding")
        {
            struct SubTestData
            {
                std::u32string input;
                std::string expectedUtf8;
                const char *desc;
            };

            SubTestData allData[] = {
                {U"", "", "empty"},
                // note that gcc has a bug. \u0000 is represented as 1, not 0.
                // Use \0 instead.
                {std::u32string(U"\0", 1), std::string("\0", 1), "zero char"},
                {U"h", "h", "ascii char"},
                {U"hi", "hi", "ascii 2 chars"},
                {U"\u0181", u8"\u0181", "2 byte"},
                {U"\u0810", u8"\u0810", "3 bytes"},
                {U"\U00010010", u8"\U00010010", "4 bytes"},
                // C++ literals do not recognize characters in the 5 and 6 byte
                // range. So we specify the encoded version manually
                {U"\x00200010", "\xF8\x88\x80\x80\x90", "5 bytes"},
                {U"\x04000010", "\xFC\x84\x80\x80\x80\x90", "6 bytes"},
                {U"\x80000000", u8"\ufffd", "range exceeded"},
            };

            int dataCount = std::extent<decltype(allData)>().value;

            for (int dataIndex = 0; dataIndex < dataCount; dataIndex++) {
                SubTestData *currData = &allData[dataIndex];

                SECTION(currData->desc)
                {
                    std::u32string input(currData->input);
                    std::string expectedUtf8(currData->expectedUtf8);

                    testCodecEncodingIterator<Utf8Codec>(input, expectedUtf8);
                }
            }
        }
    }
}
