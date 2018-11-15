#ifndef BDN_Utf32Codec_H_
#define BDN_Utf32Codec_H_

#include <bdn/Utf32CodecImpl.h>

namespace bdn
{

/** Implements the Utf-32 string codec.

    Note that this class is actually implemented as a typedef to
   Utf32CodecImpl<char32_t>
*/
#ifdef BDN_GENERATING_DOCS

    class Utf32Codec
    {
      public:
        /** The encoded element type. This is a 32 bit integer type.*/
        typedef char32_t EncodedElement;

        /** The std string type for the encoded string.*/
        typedef std::u32string EncodedString;

        /** Returns the maximum number of encoded elements that can be needed
         * for a character.*/
        constexpr static int getMaxEncodedElementsPerCharacter() { return 2; }

        /** A character iterator that decodes UTF-16 data from an
            arbitrary source iterator into Unicode characters (char32_t).
        */
        template <class SourceIterator>
        class DecodingIterator
            : public std::iterator<std::bidirectional_iterator_tag, char32_t, std::ptrdiff_t, char32_t *, char32_t>
        {
          public:
            /** @param sourceIt the source iterator that provides the UTF-16
               data.
                @param beginSourceIt an iterator that points to the beginning of
               the valid source data. The implementation uses this to avoid
               overshooting the data boundaries if the UTF-16 data is corrupted.
                    This is often the same as sourceIt.
                @param endSourceIt an iterator that points to the end of the
               valid source data (the position after the last valid source
               element). The implementation uses this to avoid overshooting the
               data boundaries if the UTF-16 data is corrupted.
            */
            DecodingIterator(const SourceIterator &sourceIt, const SourceIterator &beginSourceIt,
                             const SourceIterator &endSourceIt);
            DecodingIterator();

            DecodingIterator &operator++();
            DecodingIterator operator++(int);

            DecodingIterator &operator--();
            DecodingIterator operator--(int);

            char32_t operator*() const;

            bool operator==(const DecodingIterator &o) const;
            bool operator!=(const DecodingIterator &o) const;

            /** Returns an iterator to the inner encoded string that the
               decoding iterator is working on. The inner iterator points to the
               first encoded element of the character, that the decoding
                iterator is currently pointing to.*/
            const SourceIterator &getInner() const;
        };

        /** A decoding iterator that works on the iterator type of the encoded
           standard string type #EncodedString.*/
        typedef DecodingIterator<typename EncodedString::const_iterator> DecodingStringIterator;

        /** Encodes unicode characters to UTF-16.*/
        template <class SourceIterator>
        class EncodingIterator
            : public std::iterator<std::bidirectional_iterator_tag, char32_t, std::ptrdiff_t, char32_t *, char32_t>
        {
          public:
            EncodingIterator(const SourceIterator &sourceIt);
            EncodingIterator();

            EncodingIterator &operator++();
            EncodingIterator operator++(int);

            EncodingIterator &operator--();
            EncodingIterator operator--(int);

            char32_t operator*() const;

            bool operator==(const EncodingIterator<SourceIterator> &o) const;
            bool operator!=(const EncodingIterator<SourceIterator> &o) const;
        };
    };

#else

    typedef Utf32CodecImpl<char32_t> Utf32Codec;

#endif
}

#endif