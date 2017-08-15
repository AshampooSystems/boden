#include <bdn/init.h>
#include <bdn/localeUtil.h>


#include <locale>


namespace bdn
{


std::locale deriveUtf8Locale(const std::locale& baseLocale)
{
    // we need to replace the default codec facets with their utf-8 counterparts
    std::locale loc = std::locale( baseLocale, new std::codecvt_utf8<wchar_t> );

    loc = std::locale( loc, new CodecVtUtf8Utf16 );
    loc = std::locale( loc, new CodecVtUtf8Utf32 );

    return loc;
}


bool isUtf8Locale(const std::locale& loc)
{
    if( std::has_facet< CodecVtChar16Char >(loc) )
    {
        const CodecVtChar16Char& codec = std::use_facet< CodecVtChar16Char >(loc);

        if( typeid(codec) == typeid(CodecVtUtf8Utf16) )
            return true;

        // some systems might also use the UTF-8 to UCS-2 codec here (instead of UTF-8 to UTF-16)
        if( typeid(codec) == typeid(CodecVtUtf8Ucs2) )
            return true;
    }

    if( std::has_facet< CodecVtChar32Char >(loc) )
    {
        const CodecVtChar32Char& codec = std::use_facet< CodecVtChar32Char >(loc);

        if( typeid(codec) == typeid(CodecVtUtf8Utf32) )
            return true;

        // normally UCS4 is the same as UTF-32, but we check anyway
        if( typeid(codec) == typeid(CodecVtUtf8Ucs4) )
            return true;
    }

    if( std::has_facet< std::codecvt<wchar_t,char,mbstate_t> >(loc) )
    {
        const std::codecvt<wchar_t,char,mbstate_t>& codec = std::use_facet< std::codecvt<wchar_t,char,mbstate_t> >(loc);

        if( typeid(codec) == typeid(std::codecvt_utf8<wchar_t>) )
            return true;
    
        // we do not recognize any of the codec types. But it might still be a custom codec that
        // implements UTF-8. We check for that by trying to decode a UTF-8 sequence.
        // Note that this can be a little risky: some codecs have bugs that lead to a crash when
        // the data is not encoded correctly.
        // To reduce the likelihood for such a bug to trigger we make sure that the sequence
        // we decode is embedded in a "sea of zeros", to make sure that any misguided attempts
        // to read data outside the actual sequence do not cause a crash.

        // We encode a character that fits into a 16 bit value and does not need a surrogate pair when
        // encoded as UTF-16. We also make sure that the character has a good mix of 1 bits and 0 bits
        // to make it less likely that some other encoding gives us the same result.

        // We use a three-byte sequence. Some utf-8 implementations have problems with longer sequences.
        const char* pUtf8WithZeros = u8"\0\0\0\0\0\0\0\0\uea7d\0\0\0\0\0\0\0\0";

        const char* pUtf8Begin = pUtf8WithZeros+8;    
        const char* pUtf8End = pUtf8Begin+3;
        const char* pUtf8Next = pUtf8Begin;

        wchar_t outBuffer[16] = {0};
        wchar_t* pOutNext = outBuffer;

        std::mbstate_t state = std::mbstate_t();

        std::codecvt_base::result result = codec.in(
            state,
            pUtf8Begin,
            pUtf8End,
            pUtf8Next,
            outBuffer,
            outBuffer+16,
            pOutNext
            );

        if(result==std::codecvt_base::ok)
        {
            // the codec says that it was able to decode this.
            // Verify that the result is what we expected.

            if(pOutNext == &outBuffer[1] && outBuffer[0]==0xea7d )
            {
                // it seems that we have utf-8
                return true;
            }
        }
    }

    // not utf8
    return false;
}



}

