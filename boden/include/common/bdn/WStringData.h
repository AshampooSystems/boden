#ifndef BDN_WStringData_H_
#define BDN_WStringData_H_

#include <bdn/StringData.h>
#include <bdn/Utf16Codec.h>
#include <bdn/Utf32Codec.h>

namespace bdn
{

#if BDN_WCHAR_SIZE==2
	typedef StringData< Utf16Codec<wchar_t> > WStringData;

#elif BDN_WCHAR_SIZE==4
	typedef StringData< Utf32Codec<wchar_t> > WStringData;

#else
	#error Unsupported wchar size

#endif

}


#endif
