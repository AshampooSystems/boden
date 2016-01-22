#ifndef BDN_Utf32StringData_H_
#define BDN_Utf32StringData_H_

#include <bdn/StringData.h>
#include <bdn/Utf32Codec.h>


namespace bdn
{

/** A StringData implementation that stores the data in UTF-32 format.

	This class is a typedef to StringData<Utf32Codec>.

	See StringData for information about constructors and methods.
*/
#ifdef BDN_GENERATING_DOCS

class Utf32StringData
{
};

#else

typedef StringData< Utf32Codec<char32_t> > Utf32StringData;

#endif

}


#endif
