#include <bdn/init.h>
#include <bdn/LocaleDecoder.h>

#include <locale>
#include <cstring>

namespace bdn
{

#ifndef BDN_OVERRIDE_LOCALE_ENCODING_UTF8

    bool LocaleDecoder::Iterator::fillOutBuffer() const
    {
        size_t outWritten = 0;
        while (outWritten == 0 && _pInNext != _pInEnd) {
            if (_pDecodeState == nullptr)
                _pDecodeState = new DecodeState;

            const char *pInBegin = _pInNext;
            wchar_t *pOutBegin = _pDecodeState->outBuffer;
            wchar_t *pOutNext = pOutBegin;

            int convResult =
                _pCodec->in(_pDecodeState->state, pInBegin, _pInEnd, _pInNext,
                            pOutBegin, pOutBegin + outBufferSize, pOutNext);

            outWritten = pOutNext - pOutBegin;

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
                                    (convResult == std::codecvt_base::partial &&
                                     _pInNext == pInBegin))) {
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
                if (_pInNext != _pInEnd && *_pInNext == 0)
                    _pDecodeState->outBuffer[0] = L'\0';
                else
                    _pDecodeState->outBuffer[0] = L'\xfffd';

                if (_pInNext != _pInEnd) {
                    // now we want to skip over the problematic character.
                    // Unfortunately we do not know how big it is. So we skip 1
                    // byte and hope that the codec is able to resynchronize.
                    _pInNext++;
                }

                outWritten = 1;
            }
        }

        if (outWritten == 0)
            return false;
        else {
            const wchar_t *pOut = _pDecodeState->outBuffer;
            const wchar_t *pOutEnd = pOut + outWritten;

            _pDecodeState->outCurr =
                WideCodec::DecodingIterator<const wchar_t *>(pOut, pOut,
                                                             pOutEnd);
            _pDecodeState->outEnd =
                WideCodec::DecodingIterator<const wchar_t *>(pOutEnd, pOut,
                                                             pOutEnd);

            return true;
        }
    }

#endif
}
