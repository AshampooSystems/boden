#ifndef BDN_NativeStringData_H_
#define BDN_NativeStringData_H_

#include <bdn/Utf8StringData.h>
#include <bdn/WideStringData.h>

namespace bdn
{

/** StringData class for the 'native' character encoding. On Windows this is wide char (UTF-16)
	and on other systems this is currently UTF-8.
	Other encoding might be used on some systems when new implementations are added.

	This is currently a typedef to either StringData<Utf8Codec> or StringData<Utf16Codec>
	- but you should not depend on this fact, since new target systems might use other encodings.
	
	See StringData for information about constructors and methods.
	*/
#ifdef BDN_GENERATING_DOCS

class NativeStringData
{
};

#else

#if BDN_WINDOWS

// native string data is wide char.
typedef WideStringData NativeStringData;

#else

// on all other systems the native string data is UTF-8
typedef Utf8StringData NativeStringData;

#endif

#endif


}


#endif
