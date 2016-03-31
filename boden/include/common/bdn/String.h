#ifndef BDN_String_H_
#define BDN_String_H_

#include <bdn/StringImpl.h>
#include <bdn/NativeStringData.h>


namespace bdn
{


/** Stores a text string and performs operations on it.

	The String class primarily exposes the string as a sequence of fully decoded 32 bit Unicode characters (although other
	encodings can also be provided).
	As a user of String you should forget about the internal encoding of the data (like UTF-8 or UTF-16 or the like).

	String objects do store their data internally in a certain encoding (like UTF-8 or UTF-16). But
	they automatically and efficiently decode them for access and only present full characters to the class user.
	This contrasts with the string classes in the standard library (like std::string), which expose the
	encoded data bytes to the user.

	Since the String methods work with decoded characters, that means that its iterators also return
	full characters and all lengths and indices etc. refer to character counts and indices as well.

	The String class is 99% code-compatible with std::basic_string and the various variants of std::string.
	It provides all the same member functions and types with almost the same semantics. Since it exposes the string
	as a sequence of full 32 bit unicode characters, it is most similar to std::u32string.

	The main difference is that the character access functions do not allow the direct modifcation of characters
	in-place. Iterators, the [] operator and the at() method return character values, not references. So you cannot
	use them to modify individual characters. You need to use methods like replace() if you want to replace characters.

	If you do need to implement an algorithm that benefits from direct modification of individual characters then
	we recommend that you temporarily convert the string to std::u32string, perform your modifications and then convert it
	back to String. For example:

	\code
	String			s = ...;
	std::u32string	t( s.asUtf32() );

	// do your modifications here.
	t[a] = b;
	t[x] = y;

	// convert back
	s = t;
	\endcode



	When you do need to access raw encoded data (for example, if you want to pass it to a system function
	or a library that works with encoded strings) then you can call one of the asXYZ functions
	(e.g. asUtf8(), asUtf8Ptr(), asNative(), ...). The String implementation will provide the data in the requested
	encoding. If the requested format does not match the internal encoding, then a converted copy is created
	on the fly and cached in case it is needed again.

	Multiple String objects can share the same internal data as a result of copy or subString() (slicing) operations.
	So assignment, copying and subString operations are very cheap and fast, since basically only a pointer to the existing data
	is copied.


	The main benefits of String are:

	- String exposes the string as a sequence of full Unicode characters, not encoded data.
	  This avoids common pitfalls (especially for english-speaking programmers) where your tests work fine
	  because they happen to use only ASCII characters, but in a real-world scenario with non-ASCII text the code fails.
	  With String there is no difference between ASCII and non-ASCII characters.

	- Whenever a string value is taken by a String methods, it can take the form of many different encoded string types, like
	  C-style strings (const char*, const wchar_t*, ...) or std::string objects in any Unicode encoding (UTF-8, UTF-16, UTF-32, widechar).
	  So String objects can easily interact with other kinds of string objects.

	- String object can very efficiently interact with strings in other encodings. String data is decoded / encoded on the fly with special
	  converting iterators, without the need to copy or allocate any memory.

	- String objects implement internal data sharing. So copying and subString operations are very fast and often do not need to copy
	  any string data.

	- The (hidden) internal primary encoding used for the string data matches the native encoding of the operating system that the
	  code is running on.

	- When encoded string data is needed (for example, for passing the string to the operating system or a third party library),
	  it can be provided in any Unicode encoding. If the desired encoding matches the internal encoding used by String
	  (which is the case for operating system functions) then no conversion is needed and the access is very efficient. If a different
	  encoding is requested then conversion is performed automatically and transparently. Then the converted version is cached,
	  so future requests for that encoding are instantaneous.

	- String is 99% code-compatible with the std::string variants (most compatible with std::u32string). So it can be
	  used as a drop-in replacement in most cases.

	- String provides more user-friendly method names than std::string (in addition to the std::string names). It tries to
	  avoid excessive abbreviations (for example, subString() is an alias to substr() ).

	- String provides additional ease-of-use functions that make the programmer's life easier (like findReplace() ).


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



inline bdn::String operator+(const bdn::String& a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const bdn::String& a, const std::string& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const bdn::String& a, const std::wstring& b)
{
	bdn::String result(a);
	result += b;

	return result;
}

inline bdn::String operator+(const bdn::String& a, const std::u16string& b)
{
	bdn::String result(a);
	result += b;

	return result;
}

inline bdn::String operator+(const bdn::String& a, const std::u32string& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const bdn::String& a, const char* b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const bdn::String& a, const wchar_t* b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const bdn::String& a, const char16_t* b)
{
	bdn::String result(a);
	result += b;

	return result;
}

inline bdn::String operator+(const bdn::String& a, const char32_t* b)
{
	bdn::String result(a);
	result += b;

	return result;
}






inline bdn::String operator+(bdn::String&& a, const bdn::String& b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}


inline bdn::String operator+(bdn::String&& a, const std::string& b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}


inline bdn::String operator+(bdn::String&& a, const std::wstring& b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}

inline bdn::String operator+(bdn::String&& a, const std::u16string& b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}

inline bdn::String operator+(bdn::String&& a, const std::u32string& b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}


inline bdn::String operator+(bdn::String&& a, const char* b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}


inline bdn::String operator+(bdn::String&& a, const wchar_t* b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}


inline bdn::String operator+(bdn::String&& a, const char16_t* b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}

inline bdn::String operator+(bdn::String&& a, const char32_t* b)
{
	bdn::String result( std::move(a) );
	result += b;

	return result;
}





inline bdn::String operator+(const std::string& a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const std::wstring& a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const std::u16string& a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const std::u32string& a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const char* a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const wchar_t* a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}


inline bdn::String operator+(const char16_t* a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}

inline bdn::String operator+(const char32_t* a, const bdn::String& b)
{
	bdn::String result(a);
	result += b;

	return result;
}






inline bool operator==(const bdn::String& a, const bdn::String& b)
{
	return a.operator==(b);
}


inline bool operator==(const bdn::String& a, const std::string& b)
{
	return a.operator==(b);
}


inline bool operator==(const bdn::String& a, const std::wstring& b)
{
	return a.operator==(b);
}

inline bool operator==(const bdn::String& a, const std::u16string& b)
{
	return a.operator==(b);
}

inline bool operator==(const bdn::String& a, const std::u32string& b)
{
	return a.operator==(b);
}


inline bool operator==(const bdn::String& a, const char* b)
{
	return a.operator==(b);
}


inline bool operator==(const bdn::String& a, const wchar_t* b)
{
	return a.operator==(b);
}


inline bool operator==(const bdn::String& a, const char16_t* b)
{
	return a.operator==(b);
}

inline bool operator==(const bdn::String& a, const char32_t* b)
{
	return a.operator==(b);
}



inline bool operator==(const std::string& a, const bdn::String& b)
{
	return b.operator==(a);
}


inline bool operator==(const std::wstring& a, const bdn::String& b)
{
	return b.operator==(a);
}


inline bool operator==(const std::u16string& a, const bdn::String& b)
{
	return b.operator==(a);
}


inline bool operator==(const std::u32string& a, const bdn::String& b)
{
	return b.operator==(a);
}


inline bool operator==(const char* a, const bdn::String& b)
{
	return b.operator==(a);
}


inline bool operator==(const wchar_t* a, const bdn::String& b)
{
	return b.operator==(a);
}


inline bool operator==(const char16_t* a, const bdn::String& b)
{
	return b.operator==(a);
}

inline bool operator==(const char32_t* a, const bdn::String& b)
{
	return b.operator==(a);
}






inline bool operator!=(const bdn::String& a, const bdn::String& b)
{
	return a.operator!=(b);
}


inline bool operator!=(const bdn::String& a, const std::string& b)
{
	return a.operator!=(b);
}


inline bool operator!=(const bdn::String& a, const std::wstring& b)
{
	return a.operator!=(b);
}

inline bool operator!=(const bdn::String& a, const std::u16string& b)
{
	return a.operator!=(b);
}

inline bool operator!=(const bdn::String& a, const std::u32string& b)
{
	return a.operator!=(b);
}


inline bool operator!=(const bdn::String& a, const char* b)
{
	return a.operator!=(b);
}


inline bool operator!=(const bdn::String& a, const wchar_t* b)
{
	return a.operator!=(b);
}


inline bool operator!=(const bdn::String& a, const char16_t* b)
{
	return a.operator!=(b);
}

inline bool operator!=(const bdn::String& a, const char32_t* b)
{
	return a.operator!=(b);
}



inline bool operator!=(const std::string& a, const bdn::String& b)
{
	return b.operator!=(a);
}


inline bool operator!=(const std::wstring& a, const bdn::String& b)
{
	return b.operator!=(a);
}


inline bool operator!=(const std::u16string& a, const bdn::String& b)
{
	return b.operator!=(a);
}


inline bool operator!=(const std::u32string& a, const bdn::String& b)
{
	return b.operator!=(a);
}


inline bool operator!=(const char* a, const bdn::String& b)
{
	return b.operator!=(a);
}


inline bool operator!=(const wchar_t* a, const bdn::String& b)
{
	return b.operator!=(a);
}


inline bool operator!=(const char16_t* a, const bdn::String& b)
{
	return b.operator!=(a);
}

inline bool operator!=(const char32_t* a, const bdn::String& b)
{
	return b.operator!=(a);
}







inline bool operator<(const bdn::String& a, const bdn::String& b)
{
	return a.operator<(b);
}


inline bool operator<(const bdn::String& a, const std::string& b)
{
	return a.operator<(b);
}


inline bool operator<(const bdn::String& a, const std::wstring& b)
{
	return a.operator<(b);
}

inline bool operator<(const bdn::String& a, const std::u16string& b)
{
	return a.operator<(b);
}

inline bool operator<(const bdn::String& a, const std::u32string& b)
{
	return a.operator<(b);
}


inline bool operator<(const bdn::String& a, const char* b)
{
	return a.operator<(b);
}


inline bool operator<(const bdn::String& a, const wchar_t* b)
{
	return a.operator<(b);
}


inline bool operator<(const bdn::String& a, const char16_t* b)
{
	return a.operator<(b);
}

inline bool operator<(const bdn::String& a, const char32_t* b)
{
	return a.operator<(b);
}



inline bool operator<(const std::string& a, const bdn::String& b)
{
	return b.operator>(a);
}


inline bool operator<(const std::wstring& a, const bdn::String& b)
{
	return b.operator>(a);
}


inline bool operator<(const std::u16string& a, const bdn::String& b)
{
	return b.operator>(a);
}


inline bool operator<(const std::u32string& a, const bdn::String& b)
{
	return b.operator>(a);
}


inline bool operator<(const char* a, const bdn::String& b)
{
	return b.operator>(a);
}


inline bool operator<(const wchar_t* a, const bdn::String& b)
{
	return b.operator>(a);
}


inline bool operator<(const char16_t* a, const bdn::String& b)
{
	return b.operator>(a);
}

inline bool operator<(const char32_t* a, const bdn::String& b)
{
	return b.operator>(a);
}








inline bool operator<=(const bdn::String& a, const bdn::String& b)
{
	return a.operator<=(b);
}


inline bool operator<=(const bdn::String& a, const std::string& b)
{
	return a.operator<=(b);
}


inline bool operator<=(const bdn::String& a, const std::wstring& b)
{
	return a.operator<=(b);
}

inline bool operator<=(const bdn::String& a, const std::u16string& b)
{
	return a.operator<=(b);
}

inline bool operator<=(const bdn::String& a, const std::u32string& b)
{
	return a.operator<=(b);
}


inline bool operator<=(const bdn::String& a, const char* b)
{
	return a.operator<=(b);
}


inline bool operator<=(const bdn::String& a, const wchar_t* b)
{
	return a.operator<=(b);
}


inline bool operator<=(const bdn::String& a, const char16_t* b)
{
	return a.operator<=(b);
}

inline bool operator<=(const bdn::String& a, const char32_t* b)
{
	return a.operator<=(b);
}



inline bool operator<=(const std::string& a, const bdn::String& b)
{
	return b.operator>=(a);
}


inline bool operator<=(const std::wstring& a, const bdn::String& b)
{
	return b.operator>=(a);
}


inline bool operator<=(const std::u16string& a, const bdn::String& b)
{
	return b.operator>=(a);
}


inline bool operator<=(const std::u32string& a, const bdn::String& b)
{
	return b.operator>=(a);
}


inline bool operator<=(const char* a, const bdn::String& b)
{
	return b.operator>=(a);
}


inline bool operator<=(const wchar_t* a, const bdn::String& b)
{
	return b.operator>=(a);
}


inline bool operator<=(const char16_t* a, const bdn::String& b)
{
	return b.operator>=(a);
}

inline bool operator<=(const char32_t* a, const bdn::String& b)
{
	return b.operator>=(a);
}








inline bool operator>(const bdn::String& a, const bdn::String& b)
{
	return a.operator>(b);
}


inline bool operator>(const bdn::String& a, const std::string& b)
{
	return a.operator>(b);
}


inline bool operator>(const bdn::String& a, const std::wstring& b)
{
	return a.operator>(b);
}

inline bool operator>(const bdn::String& a, const std::u16string& b)
{
	return a.operator>(b);
}

inline bool operator>(const bdn::String& a, const std::u32string& b)
{
	return a.operator>(b);
}


inline bool operator>(const bdn::String& a, const char* b)
{
	return a.operator>(b);
}


inline bool operator>(const bdn::String& a, const wchar_t* b)
{
	return a.operator>(b);
}


inline bool operator>(const bdn::String& a, const char16_t* b)
{
	return a.operator>(b);
}

inline bool operator>(const bdn::String& a, const char32_t* b)
{
	return a.operator>(b);
}



inline bool operator>(const std::string& a, const bdn::String& b)
{
	return b.operator<(a);
}


inline bool operator>(const std::wstring& a, const bdn::String& b)
{
	return b.operator<(a);
}


inline bool operator>(const std::u16string& a, const bdn::String& b)
{
	return b.operator<(a);
}


inline bool operator>(const std::u32string& a, const bdn::String& b)
{
	return b.operator<(a);
}


inline bool operator>(const char* a, const bdn::String& b)
{
	return b.operator<(a);
}


inline bool operator>(const wchar_t* a, const bdn::String& b)
{
	return b.operator<(a);
}


inline bool operator>(const char16_t* a, const bdn::String& b)
{
	return b.operator<(a);
}

inline bool operator>(const char32_t* a, const bdn::String& b)
{
	return b.operator<(a);
}







inline bool operator>=(const bdn::String& a, const bdn::String& b)
{
	return a.operator>=(b);
}


inline bool operator>=(const bdn::String& a, const std::string& b)
{
	return a.operator>=(b);
}


inline bool operator>=(const bdn::String& a, const std::wstring& b)
{
	return a.operator>=(b);
}

inline bool operator>=(const bdn::String& a, const std::u16string& b)
{
	return a.operator>=(b);
}

inline bool operator>=(const bdn::String& a, const std::u32string& b)
{
	return a.operator>=(b);
}


inline bool operator>=(const bdn::String& a, const char* b)
{
	return a.operator>=(b);
}


inline bool operator>=(const bdn::String& a, const wchar_t* b)
{
	return a.operator>=(b);
}


inline bool operator>=(const bdn::String& a, const char16_t* b)
{
	return a.operator>=(b);
}

inline bool operator>=(const bdn::String& a, const char32_t* b)
{
	return a.operator>=(b);
}



inline bool operator>=(const std::string& a, const bdn::String& b)
{
	return b.operator<=(a);
}


inline bool operator>=(const std::wstring& a, const bdn::String& b)
{
	return b.operator<=(a);
}


inline bool operator>=(const std::u16string& a, const bdn::String& b)
{
	return b.operator<=(a);
}


inline bool operator>=(const std::u32string& a, const bdn::String& b)
{
	return b.operator<=(a);
}


inline bool operator>=(const char* a, const bdn::String& b)
{
	return b.operator<=(a);
}


inline bool operator>=(const wchar_t* a, const bdn::String& b)
{
	return b.operator<=(a);
}


inline bool operator>=(const char16_t* a, const bdn::String& b)
{
	return b.operator<=(a);
}

inline bool operator>=(const char32_t* a, const bdn::String& b)
{
	return b.operator<=(a);
}



/** Behaves the same way as the corresponding operators for std::string and const char*. The string
	is written in UTF-8 encoding.
*/
inline std::ostream& operator<<(std::ostream& stream, const bdn::String& s)
{
	return stream << s.toLocaleEncoding( stream.getloc() );
}



/** Behaves the same way as the corresponding operators for std::wstring and const wchar_t*
*/
inline std::wostream& operator<<(std::wostream& stream, const bdn::String& s)
{
	return stream << s.asWide();
}





/** Behaves the same way as the corresponding operators for std::string and const char*. The string
	is read in UTF-8 encoding.
*/
inline std::istream& operator>>(std::istream& stream, bdn::String& s)
{
	std::string temp;
	stream >> temp;
	s = temp;

	return stream;
}



/** Behaves the same way as the corresponding operators for std::wstring and const wchar_t*
*/
inline std::wistream& operator>>(std::wistream& stream, bdn::String& s)
{
	std::wstring temp;
	stream >> temp;
	s = temp;

	return stream;
}


namespace std
{

	/** Behaves the same way as std::getline for std::string, except that the result is stored
		in a bdn::String object.*/
	inline std::istream& getline(std::istream& stream, bdn::String& s, char delimiterChar)
	{
		std::string temp;

		std::getline(stream, temp, delimiterChar);

		s = temp;

		return stream;
	}

	/** Behaves the same way as std::getline for std::string, except that the result is stored
		in a bdn::String object.*/
	inline std::istream& getline(std::istream& stream, bdn::String& s)
	{
		std::string temp;

		std::getline(stream, temp);

		s = temp;

		return stream;
	}


// g++ before version 5 did not have the move overloads for getline. Work around that bug.
#if !defined(__GNUC__) || __GNUC__>=5

	/** Behaves the same way as std::getline for std::string, except that the result is stored
		in a bdn::String object.*/
	inline std::istream& getline(std::istream&& stream, bdn::String& s, char delimiterChar)
	{
		std::string temp;

		std::getline(std::move(stream), temp, delimiterChar);

		s = temp;

		return stream;
	}

	/** Behaves the same way as std::getline for std::string, except that the result is stored
		in a bdn::String object.*/
	inline std::istream& getline(std::istream&& stream, bdn::String& s)
	{
		std::string temp;

		std::getline(std::move(stream), temp);

		s = temp;

		return stream;
	}

#endif


	/** Behaves the same way as std::getline for std::wstring, except that the result is stored
		in a bdn::String object.*/
	inline std::wistream& getline(std::wistream& stream, bdn::String& s, wchar_t delimiterChar)
	{
		std::wstring temp;

		std::getline(stream, temp, delimiterChar);

		s = temp;

		return stream;
	}

	/** Behaves the same way as std::getline for std::wstring, except that the result is stored
		in a bdn::String object.*/
	inline std::wistream& getline(std::wistream& stream, bdn::String& s)
	{
		std::wstring temp;

		std::getline(stream, temp);

		s = temp;

		return stream;
	}



// g++ before version 5 did not have the move overloads for getline. Work around that bug.
#if !defined(__GNUC__) || __GNUC__>=5


	/** Behaves the same way as std::getline for std::wstring, except that the result is stored
		in a bdn::String object.*/
	inline std::wistream& getline(std::wistream&& stream, bdn::String& s, wchar_t delimiterChar)
	{
		std::wstring temp;

		std::getline(std::move(stream), temp, delimiterChar);

		s = temp;

		return stream;
	}

	/** Behaves the same way as std::getline for std::wstring, except that the result is stored
		in a bdn::String object.*/
	inline std::wistream& getline(std::wistream&& stream, bdn::String& s)
	{
		std::wstring temp;

		std::getline(std::move(stream), temp);

		s = temp;

		return stream;
	}

#endif

	/** Swaps the values of a and b. Behaves as if a.swap(b) was called.
		This function causes swapping of strings in standard algorithms like std::sort
		to be more optimized.*/
	inline void swap(bdn::String& a, bdn::String& b)
	{
		return a.swap(b);
	}

}



#endif

