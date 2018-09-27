#ifndef BDN_WideStringData_H_
#define BDN_WideStringData_H_

#include <bdn/StringData.h>
#include <bdn/WideCodec.h>

namespace bdn
{

/** A StringData implementation that stores the data in wchar_t-encoding (either
   UTF-16 or UTF-32, depending on the size of wchar_t).

    This class is a typedef to StringData<WideCodec>.

    See StringData for information about constructors and methods.

    */
#ifdef BDN_GENERATING_DOCS

    class WideStringData
    {
    };

#else

    typedef StringData<WideCodec> WideStringData;

#endif
}

#endif
