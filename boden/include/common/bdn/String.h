#ifndef BDN_String_H_
#define BDN_String_H_

#include <bdn/StringImpl.h>
#include <bdn/NativeStringData.h>

namespace bdn
{

    
/** Stores a text string and performs operations on it.

	All operations of the String class work with decoded 32 bit Unicode characters. As a user of String
	you should forget about the internal encoding of the data (like UTF-8 or UTF-16 or the like).

	The String class is 99% code-compatible with std::basic_string and the various variants of std::string.
	It provides all the same member functions and types with almost the same semantics.

	The main difference is that the character access functions do not allow the direct modifcation of characters
	in-place. Iterators, the [] operator and the at() method return character values, not references. So you cannot
	use them to modify individual characters. You need to use methods like replace() if you want to replace characters.

	String objects do store their data internally in a certain encoding (like UTF-8 or UTF-16). But
	they automatically and efficiently decode them for access and only present full characters to the class user.	
	This contrasts with the string classes in the standard library (like std::string), which expose the
	encoded data bytes to the user.

	Conceptually, the String class is most equivalent to std::u32string, which also exposes a sequence of
	full characters.

	Since the String methods work with decoded characters, that means that its iterators also return
	full characters and all lengths and indices etc. refer to character counts and indices as well.
	
	When you do need to access raw encoded data (for example, if you want to pass it to a system function
	or a library that works with encoded strings) then you can call one of the asXYZ functions
	(e.g. asUtf8(), asUtf8Ptr(), asNative(), ...). The String implementation will provide the data in the requested
	encoding. If the requested format does not match the internal encoding, then a converted copy is created
	on the fly and cached in case it is needed again.

	Multiple String objects can share the same internal data as a result of copy or subString() (slicing) operations.
	So assignment, copying and subString operations are very cheap and fast, since basically only a pointer to the existing data
	is copied.


	Implementation notes:

	The following paragraphs contain notes about performance and the current implementation. The implementation
	is subject to change, so you should not depend on it.

	String objects store their data internally with the "native" Unicode encoding of the system.
	On Windows this is UTF-16 (wchar_t), on most other systems it is UTF-8. Note that String always uses a Unicode
	encoding for its primary data, even if the native multibyte encoding defined in the locale is not a Unicode
	encoding.

	String objects can hold a copy of their data in one additional encoding. This copy is generated on demand,
	depending on which encoding is requested. For example, if you call #asUtf8Ptr() on a UTF-16 string
	then it will automatically create a copy of the data in UTF-8 and return a pointer to that.
	This copy is cached, so if you request the same encoding again then no further conversion is necessary.
		
	If you request a different non-native encoding then any previously cached non-native copy is replaced.

	Note that the String will never replace their copy of the data in the primary encoding. That one always remains.
			
	Note that this class is actually implemented as a typedef to StringImpl<NativeStringData> instead of being
	derived from it.
*/

#ifdef BDN_GENERATING_DOCS

class String : public StringImpl<NativeStringData>
{
};

#else
typedef StringImpl<NativeStringData> String;

#endif

}


#endif

