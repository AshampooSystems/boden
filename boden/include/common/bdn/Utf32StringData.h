#ifndef BDN_Utf32StringData_H_
#define BDN_Utf32StringData_H_

#include <bdn/StringData.h>
#include <bdn/Utf32Codec.h>


namespace bdn
{

typedef StringData< Utf32Codec<char32_t> > Utf32StringData;

}


#endif
