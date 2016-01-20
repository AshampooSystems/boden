#ifndef BDN_String_H_
#define BDN_String_H_

#include <bdn/StringImpl.h>
#include <bdn/NativeStringData.h>

namespace bdn
{

    
/** Stores and performs operations on a text string.

	In contrast to the string classes in the standard library, String objects present
	the string as a sequence of Unicode characters, not a string of encoded bytes.
	While the String object stores the data internally in a certain encoding, it automatically
	and efficiently decodes it when needed and only presents full characters to the class user.

	That implies that all lengths and indices also refer to characters, not bytes or encoded entities.
	The iterators also work on characters and return full characters.

	As a user of String you do not usually need to concern yourself with the internal encoding.

	When you do need to access raw encoded data (for example, if you want to pass it to a system function
	or a library that works with encoded strings) then you can call one of the getXYZString functions
	(e.g. #getCString_Utf8, #getStdString, ...). The String implementation will provide the requested data
	and cache it in case it is needed again.

	Some notes about performance and the implementation:

	String objects store their data internally with the "native" Unicode encoding of the system.
	On Windows this is UTF-16, on most other systems it is UTF-8. Note that String always uses a Unicode
	encoding for its primary data, even if the native multibyte encoding defined in the locale is not UTF-8.

	String objects can hold a copy of their data in one additional encoding. This copy is generated on demand,
	depending on which encoding is requested. For example, if you call #getCString_Utf8 on a UTF-16 string
	then it will automatically create a copy of the data in UTF-8 and return a pointer to that.
	This copy is cached, so if you request the same encoding again then no further conversion is necessary.
		
	If you request another non-native encoding then any previously cached copy is replaced.

	Note that the String will never replace the copy of the data in native encoding. That one always remains.

	Multiple String objects can share the same internal data as a result of copy or subString (slicing) operations.
	So assignment, copying and subString operations are very cheap and fast, since basically only a pointer to the existing data
	is copied.

	This has the implication that String objects have to copy shared data before they can modify it (copy-on-write).
	So all modifying operations (#replace, etc.) can potentially result in the entire string being copied once.
	Note that this is not really a performance penalty, since this copy operation would simply have occurred
	earlier if the String objects had not shared the same data.
*/
typedef StringImpl<NativeStringData> String;

}


#endif

