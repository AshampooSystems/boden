#ifndef BDN_Utf8StringData_H_
#define BDN_Utf8StringData_H_

#include <bdn/StringData.h>
#include <bdn/Utf8Codec.h>


namespace bdn
{

/** A StringData implementation that stores the data in UTF-8 format.
	
	This class is a typedef to StringData<Utf8Codec>.

	See StringData for information about constructors and methods.
*/
#ifdef BDN_GENERATING_DOCS

class Utf8StringData
{
};

#else

typedef StringData<Utf8Codec> Utf8StringData;

#endif

}


#endif
