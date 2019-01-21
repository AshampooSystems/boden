#pragma once

#include <bdn/WideCodec.h>
#include <bdn/Utf16Codec.h>
#include <bdn/Utf8Codec.h>

#include <cstring>
#include <locale>

namespace bdn
{

    /** Decodes string data from the multibyte encoding used in the given
       locale.

        LocaleDecoder follows the \ref sequence.md "sequence" protocol, i.e.
        it provides begin() and end() methods that return iterators. The
       iterators return the fully decoded 32 bit unicode characters.

        If the encoded data contains invalid data (i.e. data that fails to
       decode) then the iterators return the U'fffd' unicode character for each
       byte that cannot be decoded.

    */
    class LocaleDecoder
    {
      public:
        /** \param data pointer to the encoded multibyte character data
            \param bytes size of the encoded multibyte data in bytes
            \param loc the locale whose multibyte encoding should be used to
                decode the data.*/
        LocaleDecoder(const char *data, size_t bytes, const std::locale &loc)
            : _data(data), _bytes(bytes)
              // note that we store a copy of the locale to ensure that it is
              // not deleted while we decode.
              ,
              _locale(loc)
        {
            _codec = &std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(_locale);
        }

#ifdef BDN_OVERRIDE_LOCALE_ENCODING_UTF8

        using Iterator = Utf8Codec::DecodingIterator<const char *>;

        Iterator begin() const { return Iterator(_data, _data, _data + _bytes); }

        Iterator end() const { return Iterator(_data + _bytes, _data, _data + _bytes); }

#else

        class Iterator
        {
          public:
            // If we follow the standard precisely then we would need to declare
            // this as an input_iterator instead of a forward_iterator. The
            // reason is that we return values, not references, which only an
            // input iterator may do. However, input iterators are also single
            // pass only, so their use is very restrictive. String algorithms
            // frequently want to do multiple passes over the data (for example,
            // one to determine the length and one to do the actual work). We do
            // support multipass, so we declare ourselves as a forward
            // iterators, so that this capability will actually be used. In
            // practice it almost never causes a problem if a forward iterator
            // returns values instead of references.
            using iterator_category = std::forward_iterator_tag;
            using value_type = char32_t;
            using difference_type = size_t;
            using pointer = char32_t *;
            using reference = char32_t;

            Iterator() : _inNext(nullptr), _inEnd(nullptr), _codec(nullptr), _decodeState(nullptr) {}

          private:
            Iterator(const char *data, size_t bytes, const std::codecvt<wchar_t, char, mbstate_t> *codec)
                : _inNext(data), _inEnd(data + bytes), _codec(codec), _decodeState(nullptr)
            {}

          public:
            Iterator(Iterator &&o)
                : _inNext(o._inNext), _inEnd(o._inEnd), _codec(o._codec), _decodeState(o._decodeState)
            {
                o._decodeState = nullptr;
            }

            Iterator(const Iterator &o) : _inNext(o._inNext), _inEnd(o._inEnd), _codec(o._codec)
            {
                if (o._decodeState != nullptr)
                    _decodeState = new DecodeState(*o._decodeState);
                else
                    _decodeState = nullptr;
            }

            ~Iterator()
            {
                if (_decodeState != nullptr)
                    delete _decodeState;
            }

            Iterator &operator++()
            {
                if (_decodeState == nullptr) {
                    // this happens when we are at the start position and the
                    // first character has not been accessed. fill the output
                    // buffer first.
                    fillOutBuffer();
                }

                // _decodeState can still be null if the input data was empty.
                // In that case this call was invalid (++ past end of the data)
                if (_decodeState != nullptr) {
                    // outCurr is currently at a valid index.
                    // advance it...
                    ++_decodeState->outCurr;

                    if (_decodeState->outCurr == _decodeState->outEnd) {
                        // end of output data reached. Fill the buffer again
                        fillOutBuffer();

                        // if there is no more data to be decoded then this was
                        // an invalid call. (advancing past the end of the
                        // data).
                    }
                }

                return *this;
            }

            Iterator operator++(int)
            {
                Iterator oldVal = *this;
                operator++();

                return oldVal;
            }

            char32_t operator*() const
            {
                if (_decodeState == nullptr)
                    fillOutBuffer();

                return *_decodeState->outCurr;
            }

            bool operator==(const Iterator &o) const
            {
                // we can only compare iterators that follow the same input data
                // grid. I.e. iterators that will start and end the input decode
                // operations on the same packet boundaries. This is
                // automatically the case if 1) one started out at some point as
                // a copy of the other 2) one is positioned at the end of the
                // data That is all we need.

                // There is one additional problem: when an iterator is first
                // created it does not automatically fill the output buffer. But
                // accessing the first decoded char will fill the output buffer
                // and also advance _inNext. So we have to make sure that
                // iterators that have accessed the first char still compare
                // equal to those that have not, even though their _inNext is
                // different.

                // If we have an iterator with uninitialized decode state then
                // we initialize it

                if (_decodeState == nullptr)
                    fillOutBuffer();
                if (o._decodeState == nullptr)
                    o.fillOutBuffer();

                // once the output buffer has been filled at least once the
                // iterator will always point at a valid decoded character
                // in the output buffer. So we can compare
                // _inNext and the number of characters that are left in the
                // buffer

                if (_inNext == o._inNext) {
                    // the pointer to the NEXT input data package.
                    // We now compare the number of unused wide characters left
                    // in the out buffer. It is important to compare the number
                    // of characters left rather that the current index in the
                    // buffer, because we want an iterator that was manually
                    // positioned at the end of the input data to compare equal
                    // to one that has iterated through the data before it.

                    // Also note that _decodeState will still be null here
                    // (even though we called fillBuffer below) if the input
                    // data is empty.

                    size_t outWideLeft = (_decodeState == nullptr)
                                             ? 0
                                             : (_decodeState->outEnd.getInner() - _decodeState->outCurr.getInner());
                    size_t otherOutWideLeft =
                        (o._decodeState == nullptr)
                            ? 0
                            : (o._decodeState->outEnd.getInner() - o._decodeState->outCurr.getInner());

                    return (outWideLeft == otherOutWideLeft);
                }

                return false;
            }

            bool operator!=(const Iterator &o) const { return !operator==(o); }

            Iterator &operator=(Iterator &&o)
            {
                if (_decodeState != nullptr)
                    delete _decodeState;

                _inNext = o._inNext;
                _inEnd = o._inEnd;
                _codec = o._codec;
                _decodeState = o._decodeState;

                o._inNext = nullptr;
                o._inEnd = nullptr;
                o._codec = nullptr;
                o._decodeState = nullptr;

                return *this;
            }

            Iterator &operator=(const Iterator &o)
            {
                if (_decodeState != nullptr)
                    delete _decodeState;

                _inNext = o._inNext;
                _inEnd = o._inEnd;
                _codec = o._codec;

                if (o._decodeState == nullptr)
                    _decodeState = nullptr;
                else
                    _decodeState = new DecodeState(*o._decodeState);

                return *this;
            }

          private:
            /** Fills the output buffer. Returns false when that is not possible
               because the end of the input data has been reached.*/
            bool fillOutBuffer() const;

            enum
            {
                outBufferSize = 16
            };

            struct DecodeState
            {
                DecodeState() : state(std::mbstate_t()) {}

                DecodeState(const DecodeState &o) : state(o.state)
                {
                    std::memcpy(outBuffer, o.outBuffer, sizeof(wchar_t) * outBufferSize);

                    // the iterators operate on pointers. We must ensure that
                    // OUR iterators also refer to OUR buffer.
                    wchar_t *outBegin = outBuffer;

                    size_t currIndex = o.outCurr.getInner() - (wchar_t *)o.outBuffer;
                    size_t endIndex = o.outEnd.getInner() - (wchar_t *)o.outBuffer;

                    outCurr = WideCodec::DecodingIterator<const wchar_t *>(outBegin + currIndex, outBegin,
                                                                           outBegin + endIndex);
                    outEnd = WideCodec::DecodingIterator<const wchar_t *>(outBegin + endIndex, outBegin,
                                                                          outBegin + endIndex);
                }

                std::mbstate_t state;
                wchar_t outBuffer[outBufferSize];
                WideCodec::DecodingIterator<const wchar_t *> outCurr;
                WideCodec::DecodingIterator<const wchar_t *> outEnd;
            };

            mutable const char *_inNext;
            const char *_inEnd;
            const std::codecvt<wchar_t, char, mbstate_t> *_codec;
            mutable DecodeState *_decodeState;

            friend class LocaleDecoder;
        };

        Iterator begin() const { return Iterator(_data, _bytes, _codec); }

        Iterator end() const { return Iterator(_data + _bytes, 0, _codec); }

#endif

      private:
        const char *_data;
        size_t _bytes;
        const std::codecvt<wchar_t, char, mbstate_t> *_codec;
        std::locale _locale;
    };
}
