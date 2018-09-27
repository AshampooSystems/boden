#ifndef BDN_Utf16StringData_H_
#define BDN_Utf16StringData_H_

#include <bdn/StringData.h>
#include <bdn/Utf16Codec.h>

namespace bdn
{

/** A StringData implementation that stores the data in UTF-16 format.

    This class is a typedef to StringData<Utf16Codec>.

    See StringData for information about constructors and methods.
*/
#ifdef BDN_GENERATING_DOCS

    class Utf16StringData
    {
    };

#else

    typedef StringData<Utf16Codec> Utf16StringData;

#endif
}

#endif
