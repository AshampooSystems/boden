#ifndef BDN_localeUtil_H_
#define BDN_localeUtil_H_

#include <codecvt>

namespace bdn
{


/** Returns true if the specified locale uses Utf-8 as its multibyte encoding.*/
bool isUtf8Locale(const std::locale& loc);


/** Creates a variant of the specified input locale that uses Utf-8
    encoding but copies all other settings unchanged.*/
std::locale deriveUtf8Locale(const std::locale& baseLocale);


// Visual Studio 2015 and 2017 have a bug when using char16_t and char32_t
// as codecvt parameters. The standard library only provides an implementation for the
// codecvt static elements with unsigned int (instead of wchar_t) and unsigned short (instead of char16_t).
// The error occurred because char16_t used to be a typedef in older versions but is now a distinct type -
// and the standard library was apparently not updated when this changed.
// See https://connect.microsoft.com/VisualStudio/Feedback/Details/1403302.
// So to work around this problem we have to use unsigned int / short instead of char16_t and char32_t.

/** A typedef to the std::codecvt implementation from the C++ standard library
    that converts between UTF-8 and UTF-16.

    This is usually an alias to std::codecvt_utf8_utf16<char16_t>, except if
    compiler or standard library problems make it necessary to use a different type.

    For example, certain versions of Microsoft Visual Studio (namely VS2015 and VS2017)
    do not provide std::codecvt_utf8_utf16<char16_t>.  Instead they only provide
    std::codecvt_utf8_utf16<unsigned short> - which is functionally equivalent but a different type.    
    */
#if defined(_MSC_VER) && _MSC_VER>=1900 && _MSC_VER<=1901  // Visual Studio 2015 and 2017
    typedef std::codecvt_utf8_utf16<unsigned short> CodecVtUtf8Utf16;

#else
    typedef std::codecvt_utf8_utf16<char16_t> CodecVtUtf8Utf16;

#endif



/** A typedef to the std::codecvt implementation from the C++ standard library
    that converts between UTF-8 and UCS-2.

    This is usually an alias to std::codecvt_utf8<char16_t>, except if
    compiler or standard library problems make it necessary to use a different type.

    For example, certain versions of Microsoft Visual Studio (namely VS2015 and VS2017)
    do not provide std::codecvt_utf8<char16_t>.  Instead they only provide
    std::codecvt_utf8<unsigned short> - which is functionally equivalent but a different type.    
    */
#if defined(_MSC_VER) && _MSC_VER>=1900 && _MSC_VER<=1901  // Visual Studio 2015 and 2017
    typedef std::codecvt_utf8<unsigned short> CodecVtUtf8Ucs2;

#else
    typedef std::codecvt_utf8<char16_t> CodecVtUtf8Ucs2;

#endif


/** A typedef to the std::codecvt implementation from the C++ standard library
    that converts between UTF-8 and Utf-32.

    This is usually an alias to std::codecvt_utf8<char32_t>, except if
    compiler or standard library problems make it necessary to use a different type.

    For example, certain versions of Microsoft Visual Studio (namely VS2015 and VS2017)
    do not provide std::codecvt_utf8<char32_t>.  Instead they only provide
    std::codecvt_utf8<unsigned int> - which is functionally equivalent but a different type.    
    */
#if defined(_MSC_VER) && _MSC_VER>=1900 && _MSC_VER<=1901  // Visual Studio 2015 and 2017
    typedef std::codecvt_utf8<unsigned int> CodecVtUtf8Utf32;

#else
    typedef std::codecvt_utf8<char32_t> CodecVtUtf8Utf32;    

#endif


/** A typedef to the std::codecvt implementation from the C++ standard library
    that converts between UTF-8 and UCS-4.

    This is usually an alias for CodecVtUtf8Utf32 because UTF-32 and UCS-4 are the same.
    */
typedef CodecVtUtf8Utf32 CodecVtUtf8Ucs4;



/** A typedef to the std::codecvt base type that converts between char and char16_t.

    This is usually an alias to std::codecvt<char16_t, char, mbstate_t>, except if
    compiler or standard library problems make it necessary to use a different type.

    For example, certain versions of Microsoft Visual Studio (namely VS2015 and VS2017)
    do not provide std::codecvt<char16_t, char, mbstate_t>.  Instead they only provide
    std::codecvt<unsigned short, char, mbstate_t> - which is functionally equivalent but a different type.    
    */
#if defined(_MSC_VER) && _MSC_VER>=1900 && _MSC_VER<=1901  // Visual Studio 2015 and 2017
    typedef std::codecvt<unsigned short, char, std::mbstate_t> CodecVtChar16Char;

#else
    typedef std::codecvt<char16_t, char, std::mbstate_t> CodecVtChar16Char
#endif


/** A typedef to the std::codecvt base type that converts between char and char32_t.

    This is usually an alias to std::codecvt<char32_t, char, mbstate_t>, except if
    compiler or standard library problems make it necessary to use a different type.

    For example, certain versions of Microsoft Visual Studio (namely VS2015 and VS2017)
    do not provide std::codecvt<char16_t, char, mbstate_t>.  Instead they only provide
    std::codecvt<unsigned short, char, mbstate_t> - which is functionally equivalent but a different type.    
    */
#if defined(_MSC_VER) && _MSC_VER>=1900 && _MSC_VER<=1901  // Visual Studio 2015 and 2017
    typedef std::codecvt<unsigned int, char, std::mbstate_t> CodecVtChar32Char;

#else
    typedef std::codecvt<char32_t, char, std::mbstate_t> CodecVtChar32Char
#endif
    
}



#endif

