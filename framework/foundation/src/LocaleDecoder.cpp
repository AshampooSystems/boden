
#include <bdn/LocaleDecoder.h>

#include <locale>
#include <cstring>

namespace bdn
{

#ifndef BDN_OVERRIDE_LOCALE_ENCODING_UTF8

    bool LocaleDecoder::Iterator::fillOutBuffer() const
    {
        size_t outWritten = 0;
        while (outWritten == 0 && _inNext != _inEnd) {
            if (_decodeState == nullptr)
                _decodeState = new DecodeState;

            const char *inBegin = _inNext;
            wchar_t *outBegin = _decodeState->outBuffer;
            wchar_t *outNext = outBegin;

            int convResult =
                _codec->in(_decodeState->state, inBegin, _inEnd, _inNext, outBegin, outBegin + outBufferSize, outNext);

            outWritten = outNext - outBegin;

            // some codec implementations return "partial" when they
            // cannot convert a character. At the same time, partial will be
            // returned in normal operation when the output buffer is not big
            // enough. So we need to distinguish the two "partial" results.
            // Luckily, that is pretty easy. We can treat partial like error
            // when we got not output and no input was consumed. That is
            // sufficient, even in the case where convertible character precede
            // the problem area. In that case the successfully converted
            // characters will be consumed and in the next iteration the problem
            // character will be at the start, thus returning 0 written and 0
            // read.
            if (outWritten == 0 && (convResult == std::codecvt_base::error ||
                                    (convResult == std::codecvt_base::partial && _inNext == inBegin))) {
                // no character can be converted because of an error. This
                // should be pretty rare. On most systems wchar_t is 32 bit and
                // can hold the whole unicode range. On Windows it is 16 bit,
                // but Windows has no locale encodings that cannot be
                // represented by the 16 bit wchar_t (since wchar_t is the
                // internal kernel encoding).

                // Usually we insert a replacement character.

                // However, on Macs this error happens when the input is a zero
                // character (zero byte). In that case we simply want to copy
                // the zero character through.
                if (_inNext != _inEnd && *_inNext == 0)
                    _decodeState->outBuffer[0] = L'\0';
                else
                    _decodeState->outBuffer[0] = L'\xfffd';

                if (_inNext != _inEnd) {
                    // now we want to skip over the problematic character.
                    // Unfortunately we do not know how big it is. So we skip 1
                    // byte and hope that the codec is able to resynchronize.
                    _inNext++;
                }

                outWritten = 1;
            }
        }

        if (outWritten == 0)
            return false;
        else {
            const wchar_t *out = _decodeState->outBuffer;
            const wchar_t *outEnd = out + outWritten;

            _decodeState->outCurr = WideCodec::DecodingIterator<const wchar_t *>(out, out, outEnd);
            _decodeState->outEnd = WideCodec::DecodingIterator<const wchar_t *>(outEnd, out, outEnd);

            return true;
        }
    }

#endif
}
