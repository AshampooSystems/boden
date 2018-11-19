#ifndef BDN_TextSinkStdStreamBuf_H_
#define BDN_TextSinkStdStreamBuf_H_

#include <bdn/UtfCodec.h>
#include <bdn/ITextSink.h>
#include <bdn/localeUtil.h>

namespace bdn
{

    /** Internal base class for TextSinkStreamBuf. Do not use.*/
    template <typename CharType> class TextSinkStdStreamBufBase_ : public std::basic_streambuf<CharType>
    {
      public:
        TextSinkStdStreamBufBase_(ITextSink *sink) : _localeDecodingState(std::mbstate_t())
        {
            _sink = sink;

            this->setp(_inBuffer, _inBuffer + (sizeof(_inBuffer) / sizeof(CharType)));
        }

      protected:
        typename std::basic_streambuf<CharType>::int_type
        overflow(typename std::basic_streambuf<CharType>::int_type c) override
        {
            // write as much from the buffer data to the UI as we can
            this->sync();

            // if we have a real char then also write that. Note that
            // we have at most 5 bytes left in the buffer after sync,
            // so we know that the additional character will fit and
            // we will not enter an infinite loop.
            if (c != EOF)
                this->sputc(c);

            return c;
        }

        int _syncWithLocaleDecoding()
        {
            // This is the generic implementation that depends on the locale to
            // provide a conversion for the data.

            // For these we need the locale to provide the encoding.
            std::locale loc = this->getloc();
            if (!std::has_facet<std::codecvt<wchar_t, CharType, mbstate_t>>(loc)) {
                // the locale does not provide a conversion to wchar_t.
                // This is an error.
                throw InvalidArgumentError("TextUiStdStreamBuf used with locale that does not provide "
                                           "a decoding facet to wchar_t.");
            }

            const std::codecvt<wchar_t, CharType, mbstate_t> &codec =
                std::use_facet<std::codecvt<wchar_t, CharType, mbstate_t>>(loc);

            const CharType *inStart = this->pbase();
            const CharType *inEnd = this->pptr();

            wchar_t *outStart = _localeDecodingOutBuffer;
            wchar_t *outEnd = _localeDecodingOutBuffer + (sizeof(_localeDecodingOutBuffer) / sizeof(wchar_t));

            while (inStart != inEnd) {
                const CharType *inNext = inStart;
                wchar_t *outNext = outStart;

                // in converts from CharType to wchar_t
                std::codecvt_base::result result =
                    codec.in(_localeDecodingState, inStart, inEnd, inNext, outStart, outEnd, outNext);

                if (result == std::codecvt_base::noconv) {
                    // input data is already char32_t. For consistency, copy it
                    // into the out buffer
                    inNext = inStart;
                    outNext = outStart;
                    while (inNext != inEnd && outNext != outEnd) {
                        *outNext = *inNext;
                        ++outNext;
                        ++inNext;
                    }

                    if (inNext == inEnd)
                        result = std::codecvt_base::ok;
                    else
                        result = std::codecvt_base::partial;
                }

                if (result == std::codecvt_base::error && outNext < outEnd) {
                    // we have a broken character in the data.
                    // inNext SHOULD point to it.
                    // We do not know how big the broken character is.
                    // All we can do here is advance inNext by one
                    // and then try again.
                    // We also write 0xfffd character to the output string.
                    *outNext = 0xfffd;
                    ++outNext;

                    inStart = inNext + 1;
                } else {
                    // do another decoding iteration.
                    inStart = inNext;
                }

                if (outNext > outStart) {
                    String s(outStart, outNext - outStart);
                    _sink->write(s);
                } else {
                    // if no output was produced then we abort the loop.
                    // Note that we always produce at least one character of
                    // output if we have a decoding error.
                    break;
                }
            }

            // The codecvt SHOULD have consumed all input data. If a partial
            // character was at the end then it SHOULD have copied it into its
            // internal state.
            // However, many codecvt implementations are buggy, so we do not
            // depend on this. If input data remains then we copy it to the
            // start of the buffer.

            // reset the "current" pointer of the buffer to the start
            this->setp(this->pbase(), this->epptr());

            // copy the unconsumed data to the beginning of the buffer
            while (inStart < inEnd) {
                this->sputc(*inStart);
                inStart++;
            }

            return 0;
        }

        int _utfSync()
        {
            CharType *inStart = this->pbase();
            CharType *inEnd = this->pptr();

            // we need to decode the written data. So we can only flush full
            // encoded character sequences and need to ignore unfinished
            // sequences at the end. So first we find the end of fully encoded
            // data.
            typename UtfCodec<CharType>::template DecodingIterator<CharType *> startIt(inStart, inStart, inEnd);
            typename UtfCodec<CharType>::template DecodingIterator<CharType *> endIt(inEnd, inStart, inEnd);

            // Note that the DecodingIterator class of the UTF codecs can
            // iterate backwards. So we use that to find the end of the valid
            // data.

            while (endIt != startIt) {
                // go backwards until we find a decodable character.
                typename UtfCodec<CharType>::template DecodingIterator<CharType *> it(endIt);

                --it;
                if (*it != 0xfffd) {
                    // ok, the last character is decodable => the current
                    // endIt is the end of the valid data.
                    break;
                }

                size_t unconsumedElements = inEnd - it.getInner();
                if (unconsumedElements >= (size_t)UtfCodec<CharType>::getMaxEncodedElementsPerCharacter()) {
                    // the "invalid" data at the end is definitely long enough
                    // for an encoded character. That means that the reason the
                    // data is invalid is NOT that it is incomplete.
                    // In these cases we use the invalid data as is.
                    break;
                }

                // set the end to the current position, then try again
                endIt = it;
            }

            if (endIt != startIt) {
                // we have some valid data to decode. Write that.
                String text(startIt, endIt);

                _sink->write(text);
            }

            CharType *remaining = endIt.getInner();

            // reset the "current" pointer of the buffer to the start
            this->setp(this->pbase(), this->epptr());

            // copy the unconsumed data to the beginning of the buffer
            while (remaining < inEnd) {
                this->sputc(*remaining);
                remaining++;
            }

            return 0;
        }

        P<ITextSink> _sink;

        CharType _inBuffer[64];
        wchar_t _localeDecodingOutBuffer[128];
        std::mbstate_t _localeDecodingState;
    };

    /** A std::basic_streambuf implementation that writes the data
        to a bdn::ITextSink object.

        Note that bdn::ITextUi also exposes ITextSink objects, so this class can
       also be used to connect a stdio stream buffer to an ITextUi.

        This class is rarely used directly - see bdn::TextSinkStdIoStream
       instead.

        The streambuf uses the encoding of the locale that is selected into it
       with pubimbue() to decode the string data to Unicode.

        Special UTF-8 handling
        ----------------------

        There is special handling for UTF-8: the TextSinkStdStreamBuf object
       will detect if the multibyte encoding of the selected stream locale is
       UTF-8. If it is then it will use its own UTF-8 decoding routines instead
       of using the locale codec. This is done to improve the consistency on
       different platforms because many C++ standard libraries have buggy UTF-8
       implementations.

        If the locale uses any other encoding then the TextSinkStdStreamBuf will
       use the codec provided by the locale.


    */
    template <typename CHAR_TYPE> class TextSinkStdStreamBuf : public TextSinkStdStreamBufBase_<CHAR_TYPE>
    {
      public:
        TextSinkStdStreamBuf(ITextSink *sink) : TextSinkStdStreamBufBase_<CHAR_TYPE>(sink) {}

        int sync() override
        {
            // this branch is used if the char type is not recognized by us.
            // Use the locale provided decoding.
            return this->_syncWithLocaleDecoding();
        }
    };

    template <> class TextSinkStdStreamBuf<char> : public TextSinkStdStreamBufBase_<char>
    {
      public:
        TextSinkStdStreamBuf(ITextSink *sink) : TextSinkStdStreamBufBase_<char>(sink) {}

        int sync() override
        {
            // for char we cannot be certain that we have utf-8. It depends on
            // the selected locale.

            // If the locale uses UTF-8 then we want to use our own decoding
            // implementation. It ensures that the decoder works properly and
            // consistently on all platforms. This special handling is necessary
            // because the UTF-8 implementation in many C++ standard libraries
            // is buggy.
            if (isUtf8Locale(getloc()))
                return this->_utfSync();
            else
                return this->_syncWithLocaleDecoding();
        }
    };

    template <> class TextSinkStdStreamBuf<wchar_t> : public TextSinkStdStreamBufBase_<wchar_t>
    {
      public:
        TextSinkStdStreamBuf(ITextSink *sink) : TextSinkStdStreamBufBase_<wchar_t>(sink) {}

        int sync() override { return this->_utfSync(); }
    };

    template <> class TextSinkStdStreamBuf<char16_t> : public TextSinkStdStreamBufBase_<char16_t>
    {
      public:
        TextSinkStdStreamBuf(ITextSink *sink) : TextSinkStdStreamBufBase_<char16_t>(sink) {}

        int sync() override { return this->_utfSync(); }
    };

    template <> class TextSinkStdStreamBuf<char32_t> : public TextSinkStdStreamBufBase_<char32_t>
    {
      public:
        TextSinkStdStreamBuf(ITextSink *sink) : TextSinkStdStreamBufBase_<char32_t>(sink) {}

        int sync() override { return this->_utfSync(); }
    };
}

#endif
