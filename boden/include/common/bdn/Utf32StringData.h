#ifndef BDN_Utf32StringData_H_
#define BDN_Utf32StringData_H_

#include <bdn/StringData.h>
#include <bdn/Utf32EncodingIterator.h>
#include <bdn/Utf32DecodingIterator.h>


namespace bdn
{

	typedef UTF32StringData StringData<char32_t, Utf32EncodingIterator, Utf32DecodingIterator>;

}


#endif
