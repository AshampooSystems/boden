#ifndef BDN_UtfCodec_H_
#define BDN_UtfCodec_H_

#include <bdn/Utf8Codec.h>
#include <bdn/Utf16Codec.h>
#include <bdn/Utf32Codec.h>
#include <bdn/WideCodec.h>

namespace bdn
{

    /** A generic helper template that selects the corresponding utf codec
        (Utf8Codec, Utf16Codec, Utf32Codec), based on the CharType
        parameter.

        - UtfCodec<char> is Utf8Codec
        - UtfCodec<char16_t> is Utf16Codec
        - UtfCodec<char32_t> is Utf32Codec
        - UtfCodec<wchar_t> is WideCodec (which in turn is either Utf16Codec or
       Utf32Codec, depending on the size of wchar_t on the system).

        */
    template <typename CharType> class UtfCodec;

    template <> class UtfCodec<char> : public Utf8Codec
    {
    };

    template <> class UtfCodec<wchar_t> : public WideCodec
    {
    };

    template <> class UtfCodec<char16_t> : public Utf16Codec
    {
    };

    template <> class UtfCodec<char32_t> : public Utf32Codec
    {
    };
}

#endif
