#ifndef BDN_NativeStringData_H_
#define BDN_NativeStringData_H_

#include <bdn/Utf8StringData.h>
#include <bdn/WStringData.h>

namespace bdn
{

/** #StringData class for the 'native' character encoding. On Windows this is wide char (UTF-16)
	and on other systems this is UTF-8.*/

#if BDN_WINDOWS

// native string data is wide char.
typedef WStringData NativeStringData;

#else

// on all other systems the native string data is UTF-8
typedef Utf8StringData NativeStringData;

#endif


}


#endif
