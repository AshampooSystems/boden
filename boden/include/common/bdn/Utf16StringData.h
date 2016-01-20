#ifndef BDN_Utf16StringData_H_
#define BDN_Utf16StringData_H_

#include <bdn/StringData.h>
#include <bdn/Utf16EncodingIterator.h>
#include <bdn/Utf16DecodingIterator.h>


namespace bdn
{

	typedef UTF16StringData StringData<char16_t, Utf16EncodingIterator, Utf16DecodingIterator>;

}


#endif
