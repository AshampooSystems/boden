#ifndef BDN_WcharCodec_H_
#define BDN_WcharCodec_H_


#include <bdn/Utf16Codec.h>
#include <bdn/Utf32Codec.h>

namespace bdn
{

/** String codec that is used for wchar_t strings / std::wstring.

	This is usually either UTF-16 (if wchar_t is two bytes - i.e. on Windows)
	or UTF-32 (if wchar_t is four bytes - i.e. most other systems).
*/
#if BDN_WCHAR_SIZE==2
typedef Utf16Codec<wchar_t> WcharCodec;

#elif BDN_WCHAR_SIZE==4
typedef Utf32Codec<wchar_t> WcharCodec;

#else
#error Unsupported wchar size

#endif


}


#endif
