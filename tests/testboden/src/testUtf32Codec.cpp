#include <bdn/init.h>
#include <bdn/Utf32Codec.h>

#include <bdn/test.h>

#include "codecIteratorUtil.h"

namespace bdn
{

    template <class CATEGORY> class DummyUtf32TestIterator_
    {
      public:
        using iterator_category = CATEGORY;
        using value_type = char32_t;
        using difference_type = size_t;
        using pointer = char32_t *;
        using reference = const char32_t &;

        DummyUtf32TestIterator_ &operator++() { return *this; }

        DummyUtf32TestIterator_ operator++(int)
        {
            return DummyUtf32TestIterator_();
        }

        reference operator*() const { return *(char32_t *)nullptr; }
    };

    TEST_CASE("Utf32Codec", "[string]")
    {

        SECTION("iterator category")
        {
            SECTION("decoding")
            {
                SECTION("inner input")
                {
                    REQUIRE(typeid(typename Utf32Codec::DecodingIterator<
                                   DummyUtf32TestIterator_<
                                       std::input_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::input_iterator_tag));
                }

                SECTION("inner forward")
                {
                    REQUIRE(typeid(typename Utf32Codec::DecodingIterator<
                                   DummyUtf32TestIterator_<
                                       std::forward_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::forward_iterator_tag));
                }

                SECTION("inner bidir")
                {
                    REQUIRE(typeid(typename Utf32Codec::DecodingIterator<
                                   DummyUtf32TestIterator_<
                                       std::bidirectional_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner random")
                {
                    // with UTF-32 the iterator is random access if the source
                    // iterator is random access (since we have a 1:1
                    // relationship between input and output)
                    REQUIRE(typeid(typename Utf32Codec::DecodingIterator<
                                   DummyUtf32TestIterator_<
                                       std::random_access_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner pointer")
                {
                    REQUIRE(typeid(typename Utf32Codec::DecodingIterator<
                                   char32_t *>::iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }
            }

            SECTION("encoding")
            {
                SECTION("inner input")
                {
                    REQUIRE(typeid(typename Utf32Codec::EncodingIterator<
                                   DummyUtf32TestIterator_<
                                       std::input_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::input_iterator_tag));
                }

                SECTION("inner forward")
                {
                    REQUIRE(typeid(typename Utf32Codec::EncodingIterator<
                                   DummyUtf32TestIterator_<
                                       std::forward_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::forward_iterator_tag));
                }

                SECTION("inner bidir")
                {
                    REQUIRE(typeid(typename Utf32Codec::EncodingIterator<
                                   DummyUtf32TestIterator_<
                                       std::bidirectional_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner random")
                {
                    REQUIRE(typeid(typename Utf32Codec::EncodingIterator<
                                   DummyUtf32TestIterator_<
                                       std::random_access_iterator_tag>>::
                                       iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }

                SECTION("inner pointer")
                {
                    REQUIRE(typeid(typename Utf32Codec::EncodingIterator<
                                   char32_t *>::iterator_category) ==
                            typeid(std::bidirectional_iterator_tag));
                }
            }
        }

        struct SubTestData
        {
            const std::u32string utf32;
            const char *desc;
        };

        SubTestData allData[] = {
            {U"", "empty"},
            // note that gcc has a bug. \u0000 is represented as 1, not 0.
            // Use \0 instead.
            {std::u32string(U"\0", 1), "zero char"},
            {U"h", "ascii char"},
            {U"hx", "ascii 2 chars"},
            {U"\U00024B62", "non-ascii"},
            {U"\U00010437\U00024B62", "2 non-ascii"},
        };

        int dataCount = std::extent<decltype(allData)>().value;
        for (int dataIndex = 0; dataIndex < dataCount; dataIndex++) {
            SubTestData *pCurrData = &allData[dataIndex];

            SECTION(pCurrData->desc)
            {
                std::u32string data(pCurrData->utf32);

                // Utf 32 decoding and encoding is very straight forward. Both
                // decoder and encoder should simply pass each character through

                SECTION("decoding")
                testCodecDecodingIterator<Utf32Codec>(data, data);

                SECTION("encoding")
                testCodecEncodingIterator<Utf32Codec>(data, data);
            }
        }
    }
}
