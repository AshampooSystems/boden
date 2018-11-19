#include <bdn/init.h>

namespace bdn
{

    const size_t StringImplBase::npos;
    const size_t StringImplBase::noMatch;
    const size_t StringImplBase::toEnd;

    template <> BDN_SAFE_STATIC_IMPL(Utf8StringData, Utf8StringData::getEmptyData);

    template <> BDN_SAFE_STATIC_IMPL(Utf16StringData, Utf16StringData::getEmptyData);

    template <> BDN_SAFE_STATIC_IMPL(Utf32StringData, Utf32StringData::getEmptyData);

    template <> BDN_SAFE_STATIC_IMPL(WideStringData, WideStringData::getEmptyData);

    std::string wideToUtf8(const std::wstring &wideString)
    {
        WideCodec::DecodingIterator<std::wstring::const_iterator> beginCharIt(wideString.begin(), wideString.begin(),
                                                                              wideString.end());
        WideCodec::DecodingIterator<std::wstring::const_iterator> endCharIt(wideString.end(), wideString.begin(),
                                                                            wideString.end());

        return std::string(Utf8Codec::EncodingIterator<decltype(beginCharIt)>(beginCharIt),
                           Utf8Codec::EncodingIterator<decltype(endCharIt)>(endCharIt));
    }

    std::wstring utf8ToWide(const std::string &utf8String)
    {
        Utf8Codec::DecodingIterator<std::string::const_iterator> beginCharIt(utf8String.begin(), utf8String.begin(),
                                                                             utf8String.end());
        Utf8Codec::DecodingIterator<std::string::const_iterator> endCharIt(utf8String.end(), utf8String.begin(),
                                                                           utf8String.end());

        return std::wstring(WideCodec::EncodingIterator<decltype(beginCharIt)>(beginCharIt),
                            WideCodec::EncodingIterator<decltype(endCharIt)>(endCharIt));
    }

    template <class Codec, class InType, class OutType>
    inline int callCodecOut(Codec &codec, mbstate_t &state, const InType *inBegin, const InType *inEnd,
                            const InType *&inNext, OutType *outBegin, OutType *outEnd, OutType *&outNext)
    {
        int result = codec.out(state, inBegin, inEnd, inNext, outBegin, outEnd, outNext);
        if (result != std::codecvt_base::error) {
            // some buggy codec implementations will return ok for zero
            // characters, but they will not write any data and also will not
            // advance the next pointers.
            if (inBegin != inEnd && outBegin != outEnd && inNext == inBegin && outNext == outBegin) {
                // just copy the input character over. It is most likely a zero
                // character.
                *outNext = (char)*inNext;
                outNext++;
                inNext++;
            }
        }

        return result;
    }

    std::string wideToLocaleEncoding(const std::wstring &wideString, const std::locale &loc)
    {
#ifdef BDN_OVERRIDE_LOCALE_ENCODING_UTF8
        // This to work around bugs in the codecvt implementation when it is
        // known that all locales use UTF-8 as their encoding.

        // we use our own UTF-8 implementation here.
        return wideToUtf8(wideString);

#else

        const std::codecvt<wchar_t, char, mbstate_t> &codec =
            std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(loc);

        size_t len = wideString.length();
        if (len == 0)
            return std::string();
        else {
            const wchar_t *currIn = wideString.c_str();
            const wchar_t *inEnd = currIn + len;
            std::mbstate_t state = std::mbstate_t();

            std::string resultString;
            const int outBufferSize = MB_LEN_MAX * 4;
            char outBuffer[outBufferSize];

            while (currIn != inEnd) {
                const wchar_t *inNext = currIn;
                char *outNext = outBuffer;

                int convResult =
                    callCodecOut(codec, state, currIn, inEnd, inNext, outBuffer, outBuffer + outBufferSize, outNext);
                if (convResult == std::codecvt_base::error) {
                    // a character cannot be converted. The standard defines
                    // that inNext SHOULD point to that character. And all
                    // others up to that point should have been converted.

                    // But unfortunately with some standard libraries (e.g. on
                    // Mac with libc++) inNext and outNext always point to the
                    // first character, even if it is not the problem. So to
                    // work around that we try to convert character by character
                    // until we hit the problem.
                    if (outNext == outBuffer && inNext == currIn) {
                        while (inNext != inEnd) {
                            convResult = callCodecOut(codec, state, inNext, inNext + 1, inNext, outNext,
                                                      outBuffer + outBufferSize, outNext);
                            if (convResult != std::codecvt_base::ok) {
                                // found the actual error. We now know that
                                // inNext and outNext really do point to the
                                // problem character.
                                break;
                            }
                        }
                    }

                    // add the successfully converted part to the result.
                    // That is important, because the replacement char might not
                    // fit otherwise.
                    if (outNext != outBuffer)
                        resultString.append(outBuffer, outNext - outBuffer);

                    if (inNext == inEnd) {
                        // character by character conversion was successful. The
                        // codec is misbehaving, but we have to assume that
                        // everything is ok now.

                        // So. do nothing.
                    } else {
                        // Insert a replacement character.

                        const wchar_t *replacement = L"\xfffd";
                        const wchar_t *replacementNext = replacement;

                        outNext = outBuffer;
                        convResult = callCodecOut(codec, state, replacement, replacement + 1, replacementNext,
                                                  outBuffer, outBuffer + outBufferSize, outNext);
                        if (convResult != std::codecvt_base::ok) {
                            // character cannot be represented. Use question
                            // mark instead.
                            replacement = L"?";
                            replacementNext = replacement;
                            outNext = outBuffer;
                            convResult = callCodecOut(codec, state, replacement, replacement + 1, replacementNext,
                                                      outBuffer, outBuffer + outBufferSize, outNext);
                        }

                        if (outNext != outBuffer)
                            resultString.append(outBuffer, outNext - outBuffer);

                        // ignore the final replacement conversion result. If
                        // the ? character can also not be represented then we
                        // just insert nothing.

                        // skip over the problematic input character.
                        inNext++;
                    }
                } else {
                    if (outNext != outBuffer)
                        resultString.append(outBuffer, outNext - outBuffer);
                }

                currIn = inNext;
            }

            // "unshift", i.e. flush remaining state cleanup data.

            char *outNext = outBuffer;

            int convResult = codec.unshift(state, outBuffer, outBuffer + outBufferSize, outNext);
            // if we cannot unshift then we simply ignore that fact. It should
            // never happen.
            if (convResult != std::codecvt_base::error && outNext != outBuffer)
                resultString.append(outBuffer, outNext - outBuffer);

            return resultString;
        }
#endif
    }

    std::wstring localeEncodingToWide(const std::string &multiByte, const std::locale &loc)
    {
#ifdef BDN_OVERRIDE_LOCALE_ENCODING_UTF8
        // This to work around bugs in the codecvt implementation when it is
        // known that all locales use UTF-8 as their encoding.

        return utf8ToWide(multiByte);

#else
        const std::codecvt<wchar_t, char, mbstate_t> &codec =
            std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(loc);

        size_t len = multiByte.length();
        if (len == 0)
            return std::wstring();
        else {
            const char *currIn = multiByte.c_str();
            const char *inEnd = currIn + len;
            const char *inNext = currIn;
            std::mbstate_t state = std::mbstate_t();

            std::wstring resultString;
            const int outBufferSize = 16;
            wchar_t outBuffer[outBufferSize];

            while (currIn != inEnd) {
                wchar_t *outNext = outBuffer;

                int convResult = codec.in(state, currIn, inEnd, inNext, outBuffer, outBuffer + outBufferSize, outNext);

                if (outNext != outBuffer) {
                    // some data has been written.
                    resultString.append(outBuffer, outNext - outBuffer);
                }

                if (convResult == std::codecvt_base::error) {
                    // a character cannot be converted. This should be pretty
                    // rare. On most systems wchar_t is 32 bit and can hold the
                    // whole unicode range. On Windows it is 16 bit, but Windows
                    // has no locale encodings that cannot be represented by the
                    // 16 bit wchar_t (since wchar_t is the internal kernel
                    // encoding).

                    // Usually we insert a replacement character.

                    // However, on Macs this error happens when the input is a
                    // zero character (zero byte). In that case we simply want
                    // to copy the zero character through.
                    if (inNext != inEnd && *inNext == 0)
                        resultString += L'\0';
                    else
                        resultString += L'\xfffd';

                    // now we want to skip over the problematic character.
                    // Unfortunately we do not know how big it is. So we skip 1
                    // byte and hope that the codec is able to resynchronize.
                    if (inNext != inEnd)
                        inNext++;
                }

                currIn = inNext;
            }

            // no need to unshift. wchar does not use shifting.
            return resultString;
        }
#endif
    }
}
