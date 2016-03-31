#ifndef BDN_StringImpl_H_
#define BDN_StringImpl_H_

#include <bdn/StringData.h>
#include <bdn/NativeStringData.h>
#include <bdn/Utf32StringData.h>
#include <bdn/Utf16StringData.h>
#include <bdn/WideStringData.h>
#include <bdn/OutOfRangeError.h>

#include <iterator>
#include <list>
#include <locale>
#include <algorithm>

namespace bdn
{

/** Converts a wide char string into the multibyte encoding of the specified locale.
	If the locale is not specified then the global locale is used.

	Unencodable characters are replaced with the Unicode replacement character (0xfffd).
	If the replacement character is also unencodable then a question mark ('?') is used instead.
	If that is also unencodable then the character is simply skipped.

	*/
std::string wideToLocaleEncoding(const std::wstring& wideString, const std::locale& loc = std::locale());


/** Converts a string that is encoded with the multibyte encoding of the specified locale
	to a wide char string.
	If the locale is not specified then the global locale is used.

	Unencodable characters are replaced with the Unicode replacement character (0xfffd).
	If the replacement character is also unencodable then a question mark ('?') is used instead.
	If that is also unencodable then the character is simply skipped.
*/
std::wstring localeEncodingToWide(const std::string& multiByteString, const std::locale& loc = std::locale());



/** Helper base class for StringImpl. It defines some global constants only.*/
class StringImplBase : public Base
{
public:
    /** A special constant that is used sometimes for special length, position and character index values.

     When used as a length value it means "until the end of the string".

     It is also sometimes used as a special return value. For example, find() returns it to indicate that the
     string was not found.

     It is recommended to use more descriptive aliases for this constant like noMatch and toEnd for better readability.

     The constant's value is the greatest possible value for a size_t variable (note that size_t is an unsigned type).
     */
    static const size_t npos = -1;


    /** A special constant that is used to indicate that a search operation did not find any matches.

     This is an alias for npos.

     The constant's value is the greatest possible value for a size_t variable (note that size_t is an unsigned type).
     */
    static const size_t noMatch = npos;


    /** A special constant that can be used in some cases when a sub string length is needed to indicate
     that the whole remaining part of the string up to the end should be used.

     This is an alias for npos.

     The constant's value is the greatest possible value for a size_t variable (note that size_t is an unsigned type).
     */
    static const size_t toEnd = npos;
};



/** Provides an implementation of a String class with the internal encoding being
	controlled by the template parameter MainDataType. MainDataType must be a StringData object
	(or one that provides the same interface)

	StringImpl provides the implementation for the String class (which is a typedef for StringImpl<NativeDataType>).
	See the String documentation for an explanation of how StringImpl objects work.
*/
template<class MainDataType>
class StringImpl : public StringImplBase
{
public:

	/** Returns a static string constant containing all whitespace character
	   (including the unicode whitespace characters).*/
	static const StringImpl& getWhitespaceChars()
	{
		static StringImpl whitespace( U"\u0009\u000A\u000B\u000c\u000D\u0020\u0085\u00a0\u1680\u2000\u2001\u2002\u2003\u2004\u2005\u2006\u2007\u2008\u2009\u200A\u2028\u2029\u202F\u205F\u3000" );
		return whitespace;
	}


	/** Type of the character iterators used by this string.*/
	typedef typename MainDataType::Iterator Iterator;


	/** Iterator type for reverse iterators of this String (see rbegin()).*/
	typedef std::reverse_iterator<Iterator> ReverseIterator;


	/** iterator is an alias to Iterator. This is included for std::string compatibility.

		Note that Iterator does not allow direct modification of the string's characters
		via the iterator. So it is not 100% compatible to std::string::iterator.

	*/
	typedef Iterator iterator;


	/** const_iterator is an alias to Iterator. This is included for std::string compatibility.
		*/
	typedef Iterator const_iterator;



	/** reverse_iterator is an alias to ReverseIterator. This is included for std::string compatibility.
	*/
	typedef ReverseIterator reverse_iterator;


	/** const_reverse_iterator is an alias to ReverseIterator. This is included for std::string compatibility.
	*/
	typedef ReverseIterator const_reverse_iterator;



	/** The allocator type that is used for memory allocation of the encoded string data.*/
	typedef typename MainDataType::Allocator Allocator;


	/** allocator_type is an alias to Allocator. This is included for std::string compatibility.*/
	typedef Allocator allocator_type;



	/** The type of the C++ std string class for the "native" encoding.

		The "native" encoding is the one that is used by most operating system functions (see NativeStringData)

		- Windows: NativeEncodedString is an alias for std::wstring (UTF-16 encoded)
		- Other platforms: NativeEncodedString is an alias for std::string
		*/
	typedef typename NativeStringData::EncodedString NativeEncodedString;


	/** The type of an encoded element in the system's "native" encoding.

		The "native" encoding is the one that is used by most operating system functions (see NativeStringData)

		- Windows: NativeEncodedElement is an alias for wchar_t
		- Other platforms: NativeEncodedElement is an alias char
		*/
	typedef NativeStringData::EncodedElement NativeEncodedElement;


	/** Included with compatibility for std::string only. Conceptually, the string is a sequence of
		full unicode characters (char32_t) (even if the actual internal encoding can be different).*/
	typedef char32_t value_type;


	/** Included with compatibility for std::string only. Conceptually, the string is a sequence of
		full unicode characters (char32_t) (even if the actual internal encoding can be different).*/
	typedef std::char_traits<char32_t> traits_type;


	/** Included with compatibility for std::string only. Conceptually, the string is a sequence of
		full unicode characters (char32_t) (even if the actual internal encoding can be different).*/
	typedef char32_t* pointer;

	/** Included with compatibility for std::string only. Conceptually, the string is a sequence of
		full unicode characters (char32_t) (even if the actual internal encoding can be different).*/
	typedef const char32_t* const_pointer;


	/** Included with compatibility for std::string only. Conceptually, the string is a sequence of
		full unicode characters (char32_t) (even if the actual internal encoding can be different).*/
	typedef char32_t& reference;

	/** Included with compatibility for std::string only. Conceptually, the string is a sequence of
		full unicode characters (char32_t) (even if the actual internal encoding can be different).*/
	typedef const char32_t& const_reference;


	/** Included with compatibility for std::string only.*/
	typedef size_t size_type;



	/** Contructor for an empty string.*/
	StringImpl()
		: StringImpl( MainDataType::getEmptyData() )
	{
		_lengthIfKnown = 0;
	}


	/** Initializes the string with a copy of the specified string.
	*/
	StringImpl(const StringImpl& s)
	{
		_pData = s._pData;
		_pDataInDifferentEncoding = s._pDataInDifferentEncoding;

		_beginIt = s._beginIt;
		_endIt = s._endIt;

		_lengthIfKnown = s._lengthIfKnown;
	}


	/** Move constructor. Behaves the same as assign(const StringImpl&&). */
	StringImpl(StringImpl&& s) noexcept
	{
		_lengthIfKnown = -1;

		assign( std::move(s) );
	}


	/** Initializes the string with a substring of the specified string.*/
	StringImpl(const StringImpl& s, const Iterator& beginIt, const Iterator& endIt )
	{
		_pData = s._pData;

		// cannot copy _pDataInDifferentEncoding because we only want a substring of it.

		_beginIt = beginIt;
		_endIt = endIt;

		_lengthIfKnown = -1;
	}

	/** Initializes the string with a substring of the specified string.

		If subStringStartIndex is bigger than the length of the string then OutOfRangeError (which is the same as
		std::out_of_range) is thrown.
		startIndex can equal the string length - in that case the resulting string is empty.

		If subStringLength is not String::toEnd or String::npos then at most the specified number of character are copied
		from the source string. If the specified length exceeds the end of the source string, or if subStringLength is
		String::toEnd or String::npos then the remaining part of the string after the start index is copied.
	*/
	StringImpl(const StringImpl& s, size_t subStringStartIndex, size_t subStringLength = toEnd )
	{
		_lengthIfKnown = -1;

		assign(s, subStringStartIndex, subStringLength);
	}


	/** Initializes the object from a C-style UTF-8 encoded string.

		If lengthElements is String::toEnd or String::npos then \c s must be zero-terminated.
		If lengthElements is not String::toEnd / String::npos then lengthElements indicates the number
		of encoded UTF-8 bytes of \c s.

		To initialize with data in the locale-dependent multibyte encoding
		see #fromLocale.
	*/
	StringImpl(const char* s, size_t lengthElements = toEnd)
		: StringImpl( newObj<MainDataType>(s, lengthElements ) )
	{
	}


	/** Initializes the object from a UTF-8 encoded std::string.

		To initialize with data in the locale-dependent multibyte encoding
		see #fromLocale.
	*/
	StringImpl(const std::string& s)
		: StringImpl(newObj<MainDataType>(s))
	{
	}


	/** Initializes the object from a C-style wchar_t encoded string.

		If lengthElements is String::toEnd or String::npos then \c s must be zero-terminated.
		If lengthElements is not String::toEnd / String::npos then lengthElements indicates the number
		of encoded wchar_t elements of \c s.
	*/
	StringImpl(const wchar_t* s, size_t lengthElements = toEnd)
		: StringImpl(newObj<MainDataType>(s, lengthElements))
	{
	}


	/** Initializes the object from a wide-char std::wstring.*/
	StringImpl(const std::wstring& s)
		: StringImpl(newObj<MainDataType>(s))
	{
	}


	/** Initializes the object from a C-style UTF-16 encoded string.

		If lengthElements is String::toEnd or String::npos then \c s must be zero-terminated.
		If lengthElements is not String::toEnd / String::npos then lengthElements indicates the number
		of encoded 16 bit elements of \c s.
	*/
	StringImpl(const char16_t* s, size_t lengthElements = toEnd)
		: StringImpl(newObj<MainDataType>(s, lengthElements))
	{
	}


	/** Initializes the object from a UTF-16 std::u16string.*/
	StringImpl(const std::u16string& s)
		: StringImpl(newObj<MainDataType>(s))
	{
	}


	/** Initializes the object from a C-style UTF-32 encoded string.

		If lengthElements is String::toEnd or String::npos then \c s must be zero-terminated.
		If lengthElements is not String::toEnd / String::npos then lengthElements indicates the number
		of 32 bit elements of \c s.
	*/
	StringImpl(const char32_t* s, size_t lengthElements = toEnd)
		: StringImpl( newObj<MainDataType>(s, lengthElements) )
	{
	}


	/** Initializes the object from a UTF-32 std::u32string.*/
	StringImpl(const std::u32string& s)
		: StringImpl(newObj<MainDataType>(s))
	{
	}


	/** Initializes the object with the data between two character iterators.
		The iterators must return fully decoded 32 bit Unicode characters.*/
	template<class InputDecodedCharIterator>
	StringImpl(InputDecodedCharIterator beginIt, InputDecodedCharIterator endIt)
		: StringImpl( newObj<MainDataType>(beginIt, endIt) )
	{
	}



	/** Initializes the string to be \c numChars times the \c chr character.
		*/
	StringImpl(size_t numChars, char32_t chr)
		: StringImpl()
	{
		assign(numChars, chr);
	}


	/** Initializes the string with a sequence of characters.

		initializerList is automatically created by the compiler when you call this method
		with an initializer list.

		Example:

		\code
		String myString( {'a', 'b', 'c' } );
		\endcode
		*/
	StringImpl(std::initializer_list<char32_t> initializerList)
		: StringImpl(initializerList.begin(), initializerList.end())
	{
	}


	/** Constructs a string that uses the specified string data object.*/
	StringImpl(MainDataType* pData)
		: _pData(pData)
		, _beginIt( pData->begin() )
		, _endIt( pData->end() )
	{
		_lengthIfKnown = -1;
	}




	/** Static construction method. Initializes the String object from a C-style
		string in the locale-dependent multibyte encoding.*/
	static StringImpl fromLocaleEncoding(const char* s, std::locale& loc = std::locale(), size_t lengthElements=toEnd)
	{
		return localeEncodingToWide( (lengthElements==toEnd) ? std::string(s) : std::string(s, lengthElements), loc );
	}

	/** Static construction method. Initializes the String object from a std::string
	in the locale-dependent multibyte encoding.*/
	static StringImpl fromLocaleEncoding(const std::string& s, std::locale& loc = std::locale() )
	{
		return localeEncodingToWide( s, loc );
	}


	/** Returns true if the string is empty (i.e. if its length is 0).*/
	bool isEmpty() const noexcept
	{
		return (_beginIt == _endIt);
	}

	/** Same as isEmpty. This is included for compatibility with std::string.*/
	bool empty() const noexcept
	{
		return isEmpty();
	}


	/** Returns the number of characters in this string.*/
	size_t getLength() const noexcept
	{
		if (_lengthIfKnown == -1)
		{
			// character count is unknown. We need to count it first.
			int c = 0;
			Iterator it = _beginIt;
			while (it != _endIt)
			{
				++c;
				++it;
			}

			_lengthIfKnown = c;
		}

		return (size_t)_lengthIfKnown;
	}


	/** Same as getLength. This is included for compatibility with std::string.*/
	size_t length() const noexcept
	{
		return getLength();
	}

	/** Same as getLength. This is included for compatibility with std::string.*/
	size_t size() const noexcept
	{
		return getLength();
	}



	/** Used to reserve space for future modifications of the string, or to free previously reserved extra space.

		Reserving space is never necessary. This is a purely optional call that may allow the implementation to
		prevent reallocation when multiple smaller modifications are done.

		If \c reserveChars is less or equal to the length of the string then the call is a non-binding request
		to free any unneeded excess space.

		If \c reserveChars is bigger than the length of the string then this tells the implementation to reserve
		enough space for a string of that length (in characters).

		*/
	void reserve(size_t reserveChars = 0)
	{
		int excessCapacityCharacters = reserveChars-length();
		if(excessCapacityCharacters<0)
			excessCapacityCharacters = 0;

		int excessCapacityElements = excessCapacityCharacters * MainDataType::Codec::getMaxEncodedElementsPerCharacter();

		Modify m(this);

		m.pStd->reserve( m.pStd->length() + excessCapacityElements );
	}


	/** Requests that the string object reduces its capacity (see capacity()) to fit its size.

		In other words, requests that internal buffers are re-allocated to a smaller size, if they are
		bigger than needed.

		This is a non-binding request - the implementation is free to ignore it.

		This function does not alter the string contents.
	*/
	void shrink_to_fit()
	{
		Modify m(this);

		m.pStd->shrink_to_fit();
	}


	/** Returns the size of the storage space currently allocated for the string, in characters.

		The capacity is always bigger or equal to the current string length. If it is bigger then that means that the implementation
		has reserved additional space for future modifications, with the aim to avoid reallocations.
	*/
	size_t capacity() const noexcept
	{
		int excessCapacityCharacters;

		if(_pData->getRefCount()!=1)
		{
			// we are sharing the string with someone else. So every modification will cause us to copy it.
			// => no excess capacity.
			excessCapacityCharacters = 0;
		}
		else
		{
			typename MainDataType::EncodedString* pStd = &_pData->getEncodedString();

			int excessCapacity = pStd->capacity()-pStd->length();
			if(excessCapacity<0)
				excessCapacity = 0;

			excessCapacity += pStd->cend() - _endIt.getInner();

			excessCapacityCharacters = excessCapacity / MainDataType::Codec::getMaxEncodedElementsPerCharacter();
		}

		return length() + excessCapacityCharacters;
	}





	/** Returns the theoretical maximum size of a string, given an infinite amount of memory.
		This is included for compatibility with std::string.
		*/
	size_t max_size() const noexcept
	{
		size_t m = _pData->getEncodedString().max_size();

		return (m>INT_MAX) ? (size_t)INT_MAX : m;
	}


	/** Resizes the string to the specified number of characters.

		If newLength is less than the current length then the string is truncated.
		If newLength is more than the current length then the string is extended
		with padChar characters.
	*/
	void resize(size_t newLength, char32_t padChar = U'\0')
	{
		size_t currLength = getLength();

		if(newLength<currLength)
		{
			// all we need to do is change our end iterator.
			setEnd( _beginIt+newLength, newLength );
		}
		else if(newLength>currLength)
			append( (newLength-currLength), padChar );
	}


	/** Returns an iterator that points to the start of the string.*/
	Iterator begin() const noexcept
	{
		return _beginIt;
	}

	/** Retuns an iterator that points to the position just after the last character of the string.*/
	Iterator end() const noexcept
	{
		return _endIt;
	}


	/** Returns an iterator that iterates over the characters of the string in reverse order.

		The iterator starts at the last character of the string. Advancing it with ++ moves it
		to the previous character.

		Use this together with rend() to check for the end of the iteration.

		Example:

		\code
		s = "hello";
		for(auto it = s.reverseBegin(); it!=s.reverseEnd(); it++)
			print(*it);
		\endcode

		This will print out "olleh" (the reverse of "hello").

		*/
	ReverseIterator reverseBegin() const noexcept
	{
		return std::reverse_iterator<Iterator>( end() );
	}

	/** Returns an iterator that points to the end of a reverse iteration.
		See rbegin().*/
	ReverseIterator reverseEnd() const noexcept
	{
		return std::reverse_iterator<Iterator>( begin() );
	}


	/** Same as reverseBegin(). This is included for compatibility with std::string.*/
	ReverseIterator rbegin() const noexcept
	{
		return std::reverse_iterator<Iterator>( end() );
	}


	/** Same as reverseEnd(). This is included for compatibility with std::string.*/
	ReverseIterator rend() const noexcept
	{
		return std::reverse_iterator<Iterator>( begin() );
	}


	/** Same as begin(). This is included for compatibility with std::string.*/
	Iterator cbegin() const noexcept
	{
		return begin();
	}

	/** Same as end(). This is included for compatibility with std::string.*/
	Iterator cend() const noexcept
	{
		return end();
	}






	/** Same as rbegin(). This is included for compatibility with std::string.*/
	ReverseIterator crbegin() const noexcept
	{
		return rbegin();
	}

	/** Same as rend(). This is included for compatibility with std::string.*/
	ReverseIterator crend() const noexcept
	{
		return rend();
	}



	/** Returns a sub string of this string, starting at the character that beginIt points to
		and ending at the character before the position pointed to by endIt.
	*/
	StringImpl subString(const Iterator& beginIt, const Iterator& endIt) const
	{
		return StringImpl(*this, beginIt, endIt);
	}


	/** Returns a sub string of this string, starting at startIndex and including charCount characters
		from that point.

		If the string has less than charCount characters then the sub string up to the end
		is returned.

		charCount can be String::toEnd or String::npos, in which case the rest of the string up to the end is returned.

		If startIndex is invalid (<0 or >length) then an OutOfRangeError (which is the same as
		std::out_of_range) is thrown.
		startIndex can equal the string length - in that case the resulting sub string is always empty.
	*/
	StringImpl subString(size_t startIndex=0, size_t charCount = toEnd) const
	{
		size_t myLength = getLength();

		if (startIndex>myLength)
			throw OutOfRangeError("String::subString: Invalid start index: "+std::to_string(startIndex) );
		if (charCount==toEnd || startIndex+charCount>myLength)
			charCount = myLength-startIndex;

		Iterator startIt = _beginIt+startIndex;
		Iterator endIt = (charCount<0) ? _endIt : (startIt+charCount);

		return StringImpl(*this, startIt, endIt);
	}


	/** An alias for subString(). This function is included for compatibility with std::string.
	*/
	StringImpl substr(size_t startIndex=0, size_t charCount = toEnd) const
	{
		return subString(startIndex, charCount );
	}


	/** Returns a pointer to the string as a zero terminated c-style string in UTF-8 encoding.

		This operation might invalidate existing iterators.

		The pointer remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const char* asUtf8Ptr() const
	{
		return getEncoded( (Utf8StringData*)nullptr ).c_str();
	}


	/** Returns a reference to the string as a std::string object in UTF-8 encoding.

		This operation might invalidate existing iterators. The returned object reference
		remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const std::string& asUtf8() const
	{
		return getEncoded( (Utf8StringData*)nullptr );
	}


	/** Conversion operator to const char.
		Same as calling asUtf8Ptr().*/
	explicit operator const char*() const
	{
		return asUtf8Ptr();
	}


	/** Conversion operator to std::string.
		Same as calling asUtf8().*/
	operator const std::string&() const
	{
		return asUtf8();
	}


	/** Same as asUtf32Ptr(). This function is included for compatibility with std::basic_string.

		Implementation note: it might seem strange that this function returns an UTF32 string. However,
		in order to be consistent with the interface definition for basic_string there is no other way.
		For example, there is a requirement that the range [c_str() ... c_str()+size()] is valid
		and represents the contents of the string. Since our size() implementation returns the length in characters,
		This function must return an UTF-32 pointer, where each element represents a full character.
	*/
	const char32_t* c_str() const
	{
		// Implementation note: it might

		return asUtf32Ptr();
	}


	/** Same as asUtf32Ptr(). This function is included for compatibility with std::basic_string.

		See also c_str(). */
	const char32_t* data() const
	{
		return asUtf32Ptr();
	}


	/** Returns a pointer to the string as a zero terminated c-style string in "wide char" encoding
		(either UTF-16 or UTF-32, depending on the size of wchar_t).

		This operation might invalidate existing iterators.

		The pointer remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const wchar_t* asWidePtr() const
	{
		return getEncoded( (WideStringData*)nullptr ).c_str();
	}


	/** Returns a reference to the string as a std::wstring object in in "wide char" encoding
		(either UTF-16 or UTF-32, depending on the size of wchar_t).

		This operation might invalidate existing iterators. The returned object reference
		remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const std::wstring& asWide() const
	{
		return getEncoded( (WideStringData*)nullptr );
	}

	/** Conversion operator to const wchar_t.
		Same as calling asWidePtr().*/
	explicit operator const wchar_t*() const
	{
		return asWidePtr();
	}


	/** Conversion operator to const std::wstring.
		Same as calling asWide().*/
	operator const std::wstring&() const
	{
		return asWide();
	}


	/** Returns a pointer to the string as a zero terminated c-style string in UTF-16 encoding.

		This operation might invalidate existing iterators.

		The pointer remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const char16_t* asUtf16Ptr() const
	{
		return getEncoded( (Utf16StringData*)nullptr ).c_str();
	}


	/** Returns a reference to the string as a std::u16string object in UTF-16 encoding

		This operation might invalidate existing iterators. The returned object reference
		remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const std::u16string& asUtf16() const
	{
		return getEncoded( (Utf16StringData*)nullptr );
	}


	/** Conversion operator to const char16_t.
		Same as calling asUtf16Ptr().*/
	explicit operator const char16_t*() const
	{
		return asUtf16Ptr();
	}

	/** Conversion operator to const char16_t.
	Same as calling asUtf16Ptr().*/
	operator const std::u16string&() const
	{
		return asUtf16();
	}


	/** Returns a pointer to the string as a zero terminated c-style string in UTF-32 encoding.

		This operation might invalidate existing iterators.

		The pointer remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const char32_t* asUtf32Ptr() const
	{
		return getEncoded( (Utf32StringData*)nullptr ).c_str();
	}


	/** Returns a reference to the string as a std::u32string object in UTF-32 encoding

	This operation might invalidate existing iterators. The returned object reference
	remains valid at least until one of the other asXYZ conversion functions is called
	or the entire string object is destroyed.
	*/
	const std::u32string& asUtf32() const
	{
		return getEncoded( (Utf32StringData*)nullptr );
	}


	/** Conversion operator to const char32_t.
		Same as calling asUtf32Ptr().*/
	explicit operator const char32_t*() const
	{
		return asUtf32Ptr();
	}

	/** Conversion operator to const char32_t.
		Same as calling asUtf32().*/
	operator const std::u32string&() const
	{
		return asUtf32();
	}



	/** Returns a reference to the string as a std::basic_string object in "native" encoding.

		The "native" encoding is the one that is used by most operating system functions (see NativeStringData)

		- Windows: std::wstring is returned (UTF-16 encoded)
		- Other platforms: std::string is returned (UTF-8 encoded)

		This operation might invalidate existing iterators. The returned object reference
		remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const NativeEncodedString& asNative() const
	{
		return getEncoded( (NativeStringData*)nullptr );
	}


	/** Returns a pointer to the string as a zero terminated c-style string in "native" encoding encoding.

		The "native" encoding is the one that is used by most operating system functions (see NativeStringData)

		- Windows: const wchar_t* is returned (UTF-16 encoded)
		- Other platforms: const char* is returned (UTF-8 encoded)

		This operation might invalidate existing iterators.

		The pointer remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const NativeEncodedElement* asNativePtr() const
	{
		return asNative().c_str();
	}




	/** Returns a copy of the string as a std::string object in the specified
		locale's multibyte encoding.

		If the locale is not specified then the global locale is used.

		Note that in contrast to the asXYZ conversion routines this function always
		returns a new copy of the data.
	*/
	std::string toLocaleEncoding(const std::locale& loc = std::locale()) const
	{
		// note: we must use the wide char encoding as a basis, because that is the
		// only facet provided by the locale object that converts to the locale-specific
		// multibyte encoding. All other facets only convert to UTF-8.
		return wideToLocaleEncoding(asWide(), loc);
	}

	/** Compares this string with the specified other string.

		Returns -1 if this string is "smaller", 0 if it is the same string and 1 if this string is "bigger".

		"Smaller" and "Bigger" are defined by a character-by-character comparison (using fully decoded
		Unicode characters).
		If two characters at a position are different then the string whose Unicode character value is
		smaller is considered to be smaller.
		If one of the two strings is shorter and all characters up to that point are the same then the shorter
		string is smaller.
	*/
	int compare(const StringImpl& o) const
	{
		return compare(o.begin(), o.end());
	}


	/** Compares this string with a character sequence, specified by two iterators.

		Returns -1 if this string is "smaller", 0 if it is the same string and 1 if this string is "bigger".

		"Smaller" and "Bigger" are defined by a character-by-character comparison (using fully decoded
		Unicode characters).
		If two characters at a position are different then the string whose Unicode character value is
		smaller is considered to be smaller.
		If one of the two strings is shorter and all characters up to that point are the same then the shorter
		string is smaller.

		@param otherIt iterator pointing to the first character of the character sequence.
			The iterator must return char32_t Unicode characters!
		@param otherEnd iterator pointing to the position just after the last character in the character sequence.
			The iterator must return char32_t Unicode characters!
	*/
	template<class IT>
	int compare(IT otherIt, IT otherEnd) const
	{
		return compare(0, toEnd, otherIt, otherEnd);
	}


	template<class IT>
	int compare(size_t compareStartIndex, size_t compareLength, IT otherIt, IT otherEnd) const
	{
		size_t myLength = getLength();
		if(compareStartIndex>myLength)
			throw OutOfRangeError("Invalid compareStartIndex passed to String::compare.");

		if(compareLength==toEnd || compareStartIndex+compareLength>myLength)
			compareLength = myLength-compareStartIndex;

		Iterator myIt = _beginIt+compareStartIndex;

		for(size_t i=0; i<compareLength; i++)
		{
			if(otherIt==otherEnd)
				return 1;

			else
			{
				char32_t myChr = *myIt;
				char32_t otherChr = *otherIt;

				if(myChr<otherChr)
					return -1;
				else if(myChr>otherChr)
					return 1;
			}

			++myIt;
			++otherIt;
		}

		return (otherIt==otherEnd) ? 0 : -1;
	}


	/** See compare()
	*/
	int compare(const std::string& o) const
	{
		return compare( Utf8Codec::DecodingStringIterator(o.begin(), o.begin(), o.end()),
						Utf8Codec::DecodingStringIterator(o.end(), o.begin(), o.end()) );
	}

	/** See compare()
	*/
	int compare(const char* other, size_t otherLength = toEnd) const
	{
		const char* oEnd = getStringEndPtr(other, otherLength);

		return compare( Utf8Codec::DecodingIterator<const char*>(other, other, oEnd),
						Utf8Codec::DecodingIterator<const char*>(oEnd, other, oEnd) );
	}

	/** See compare()
	*/
	int compare(const std::u16string& other) const
	{
		return compare( Utf16Codec::DecodingStringIterator(other.begin(), other.begin(), other.end()),
					    Utf16Codec::DecodingStringIterator(other.end(), other.begin(), other.end()) );
	}

	/** See compare()
	*/
	int compare(const char16_t* other, size_t otherLength = toEnd) const
	{
		const char16_t* oEnd = getStringEndPtr(other, otherLength);

		return compare( Utf16Codec::DecodingIterator<const char16_t*>(other, other, oEnd),
						Utf16Codec::DecodingIterator<const char16_t*>(oEnd, other, oEnd) );
	}

	/** See compare()
	*/
	int compare(const std::u32string& o) const
	{
		return compare( Utf32Codec::DecodingStringIterator(o.begin(), o.begin(), o.end()),
						Utf32Codec::DecodingStringIterator(o.end(), o.begin(), o.end()) );
	}

	/** See compare()
	*/
	int compare(const char32_t* other, size_t otherLength = toEnd) const
	{
		const char32_t* oEnd = getStringEndPtr(other, otherLength);

		return compare( other, oEnd );
	}

	/** See compare()
	*/
	int compare(const std::wstring& o) const
	{
		return compare( WideCodec::DecodingStringIterator(o.begin(), o.begin(), o.end()),
						WideCodec::DecodingStringIterator(o.end(), o.begin(), o.end()) );
	}

	/** See compare() */
	int compare(const wchar_t* other, size_t otherLength = toEnd) const
	{
		const wchar_t* oEnd = getStringEndPtr(other, otherLength);

		return compare( WideCodec::DecodingIterator<const wchar_t*>(other, other, oEnd),
						WideCodec::DecodingIterator<const wchar_t*>(oEnd, other, oEnd) );
	}


	int compare(size_t compareStartIndex, size_t compareLength, const StringImpl& other, size_type otherStartIndex=0, size_type otherCompareLength=toEnd) const
	{
		size_t otherLength = other.getLength();
		if(otherStartIndex>otherLength)
			throw OutOfRangeError("Invalid otherStartIndex passed to String::compare");

		Iterator otherCompareBegin( other.begin()+otherStartIndex );
		Iterator otherCompareEnd( (otherCompareLength==toEnd || otherStartIndex+otherCompareLength>=otherLength)
									? other.end()
									: (otherCompareBegin+otherCompareLength) );

		return compare( compareStartIndex, compareLength, otherCompareBegin, otherCompareEnd );
	}


	template<class InputCodec, class InputIterator>
	int compareEncoded(const InputCodec& otherCodec, size_t compareStartIndex, size_t compareLength, const InputIterator& otherEncodedBeginIt, const InputIterator& otherEncodedEndIt) const
	{
		return compare( compareStartIndex,
						compareLength,
						typename InputCodec::template DecodingIterator<InputIterator>( otherEncodedBeginIt, otherEncodedBeginIt, otherEncodedEndIt ),
						typename InputCodec::template DecodingIterator<InputIterator>( otherEncodedEndIt, otherEncodedBeginIt, otherEncodedEndIt ) );
	}

	int compare(size_t compareStartIndex, size_t compareLength, const std::string& other) const
	{
		return compareEncoded(	Utf8Codec(),
								compareStartIndex,
								compareLength,
								other.begin(),
								other.end() );
	}

	int compare(size_t compareStartIndex, size_t compareLength, const std::wstring& other) const
	{
		return compareEncoded(	WideCodec(),
								compareStartIndex,
								compareLength,
								other.begin(),
								other.end() );
	}

	int compare(size_t compareStartIndex, size_t compareLength, const std::u16string& other) const
	{
		return compareEncoded(	Utf16Codec(),
								compareStartIndex,
								compareLength,
								other.begin(),
								other.end() );
	}

	int compare(size_t compareStartIndex, size_t compareLength, const std::u32string& other) const
	{
		return compareEncoded(	Utf32Codec(),
								compareStartIndex,
								compareLength,
								other.begin(),
								other.end() );
	}


	int compare(size_t compareStartIndex, size_t compareLength, const char* other, size_t otherLength=toEnd) const
	{
		return compareEncoded(	Utf8Codec(),
								compareStartIndex,
								compareLength,
								other,
								getStringEndPtr(other, otherLength) );
	}

	int compare(size_t compareStartIndex, size_t compareLength, const wchar_t* other, size_t otherLength=toEnd) const
	{
		return compareEncoded(	WideCodec(),
								compareStartIndex,
								compareLength,
								other,
								getStringEndPtr(other, otherLength) );
	}

	int compare(size_t compareStartIndex, size_t compareLength, const char16_t* other, size_t otherLength=toEnd) const
	{
		return compareEncoded(	Utf16Codec(),
								compareStartIndex,
								compareLength,
								other,
								getStringEndPtr(other, otherLength) );
	}

	int compare(size_t compareStartIndex, size_t compareLength, const char32_t* other, size_t otherLength=toEnd) const
	{
		return compare(	compareStartIndex,
						compareLength,
						other,
						getStringEndPtr(other, otherLength) );
	}



	/** Returns true if this string and the specified other string are equal.*/
	template<class OTHER>
	bool operator==(const OTHER& o) const
	{
		return compare(o)==0;
	}

	/** Returns true if this string and the specified other string are not equal.*/
	template<class OTHER>
	bool operator!=(const OTHER& o) const
	{
		return compare(o)!=0;
	}


	/** Returns true if this string is "smaller" than the specified other string. See compare().*/
	template<class OTHER>
	bool operator<(const OTHER& o) const
	{
		return compare(o) < 0;
	}


	/** Returns true if this string is "bigger" than the specified other string. See compare().*/
	template<class OTHER>
	bool operator>(const OTHER& o) const
	{
		return compare(o) > 0;
	}


	/** Returns true if this string is "smaller" or equal to the specified other string. See compare().*/
	template<class OTHER>
	bool operator<=(const OTHER& o) const
	{
		return compare(o) <= 0;
	}


	/** Returns true if this string is "bigger" or equal to the specified other string. See compare().*/
	template<class OTHER>
	bool operator>=(const OTHER& o) const
	{
		return compare(o) >= 0;
	}



	/** \copydoc at()
	*/
	char32_t operator[](size_t index) const
	{
		size_t len = getLength();
		if(index>=len)
		{
			if(index==len)
				return U'\0';

			throw OutOfRangeError("String::operator[]: Invalid index "+std::to_string(index));
		}

		return *(_beginIt+index);
	}


	/** Returns the character at the given string index.

		If index is equal to the length of the string then a null-character is returned.

		If index is negative or bigger than the length of the string then an OutOfRangeError is thrown.
	*/
	char32_t at(size_t index) const
	{
		size_t len = getLength();
		if(index>=len)
		{
			if(index==len)
				return U'\0';

			throw OutOfRangeError("String::operator[]: Invalid index "+std::to_string(index));
		}

		return *(_beginIt+index);
	}


	/** Returns the last character of the string. Throws an OutOfRangeError if the string is empty.*/
	char32_t getLastChar() const
	{
		if(_beginIt==_endIt)
			throw OutOfRangeError("String::getLastChar called on empty string.");

		Iterator it = _endIt;
		--it;

		return *it;
	}

	/** Returns the first character of the string. Throws an OutOfRangeError if the string is empty.*/
	char32_t getFirstChar() const
	{
		if(_beginIt==_endIt)
			throw OutOfRangeError("String::getFirstChar called on empty string.");

		return *_beginIt;
	}

	/** Same as getLastChar()*/
	char32_t back() const
	{
		return getLastChar();
	}

	/** Same as getFirstChar()*/
	char32_t front() const
	{
		return getFirstChar();
	}



	/** Replaces a section of the string (defined by two iterators) with the data between two
		other iterators.
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	template<class InputIterator>
	StringImpl& replace(const Iterator& rangeBegin,
						const Iterator& rangeEnd,
						const InputIterator& replaceWithBegin,
						const InputIterator& replaceWithEnd)
	{
		// we must convert the range to encoded indices because the iterators
		// can be invalidated by Modify.
		size_t encodedBeginIndex = rangeBegin.getInner()-_beginIt.getInner();
		size_t encodedLength = rangeEnd.getInner()-rangeBegin.getInner();

		Modify m(this);

		m.pStd->replace(	m.pStd->begin()+encodedBeginIndex,
							m.pStd->begin()+encodedBeginIndex+encodedLength,
							typename MainDataType::Codec::template EncodingIterator<InputIterator>(replaceWithBegin),
							typename MainDataType::Codec::template EncodingIterator<InputIterator>(replaceWithEnd) );

		return *this;
    }


	/** Replaces a section of the string (defined by two iterators) with the data between two
		other iterators.
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeBegin,
                            const Iterator& rangeEnd,
                            const Iterator& replaceWithBegin,
                            const Iterator& replaceWithEnd)
	{
		// we must convert the range to encoded indices because the iterators
		// can be invalidated by Modify.
		size_t encodedBeginIndex = rangeBegin.getInner()-_beginIt.getInner();
		size_t encodedLength = rangeEnd.getInner() - rangeBegin.getInner();

		Modify m(this);

		m.pStd->replace(	m.pStd->begin()+encodedBeginIndex,
							m.pStd->begin()+encodedBeginIndex+encodedLength,
							replaceWithBegin.getInner(),
							replaceWithEnd.getInner() );

		return *this;
	}


	/** Replaces a section of the string (defined by a start index and a length) with the data between two
		iterators.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	template<class InputIterator>
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const InputIterator& replaceWithBegin,
							const InputIterator& replaceWithEnd)
	{
		size_t myLength = getLength();

		if(rangeStartIndex>myLength)
			throw OutOfRangeError("Invalid start index passed to String::replace");

		Iterator rangeStart( _beginIt + rangeStartIndex );

		Iterator rangeEnd( (rangeLength==toEnd || rangeStartIndex+rangeLength>=myLength)
							? _endIt
							: (rangeStart+rangeLength) );

		return replace(rangeStart, rangeEnd, replaceWithBegin, replaceWithEnd);
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.

		If replaceWithStartIndex is specified then only the part of \c replaceWith starting from that index is
		used. If replaceWithStartIndex is bigger than the length of \c replaceWith then an OutOfRangeError is thrown.

		If \c replaceWithLength is specified then at most this number of characters is used from \c replaceWith.
		If \c replaceWith is not long enough for \c replaceWithLength characters to be copied, or if \c replaceWithLength is String::toEnd
		or String::npos, then only the part replaceWith up to its end is used.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(const Iterator& rangeBegin,
						const Iterator& rangeEnd,
						const StringImpl& replaceWith,
						size_t replaceWithStartIndex = 0,
						size_t replaceWithLength = toEnd)
	{
		if(replaceWithStartIndex==0 && replaceWithLength==toEnd)
			return replace(rangeBegin, rangeEnd, replaceWith.begin(), replaceWith.end() );
		else
		{
			size_t actualReplaceWithLength = replaceWith.getLength();
			if(replaceWithStartIndex>actualReplaceWithLength)
				throw OutOfRangeError("Invalid start index passed to String::replace");

			Iterator replaceWithStart = replaceWith.begin()+replaceWithStartIndex;

			Iterator replaceWithEnd( (replaceWithLength==toEnd || replaceWithStartIndex+replaceWithLength>=actualReplaceWithLength)
									? replaceWith.end()
									: (replaceWithStart+replaceWithLength) );

			return replace(rangeBegin, rangeEnd, replaceWithStart, replaceWithEnd);
		}
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		If replaceWithStartIndex is specified then only the part of \c replaceWith starting from that index is
		used. If replaceWithStartIndex is bigger than the length of \c replaceWith then an OutOfRangeError is thrown.

		If \c replaceWithLength is specified then at most this number of characters is used from \c replaceWith.
		If \c replaceWith is not long enough for \c replaceWithLength characters to be copied, or if \c replaceWithLength is String::toEnd or String::npos,
		then only the part replaceWith up to its end is used.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(size_t rangeStartIndex,
						size_t rangeLength,
						const StringImpl& replaceWith,
						size_t replaceWithStartIndex = 0,
						size_t replaceWithLength = toEnd)
	{
		if(replaceWithStartIndex==0 && replaceWithLength==toEnd)
			return replace(rangeStartIndex, rangeLength, replaceWith.begin(), replaceWith.end() );
		else
		{
			size_t actualReplaceWithLength = replaceWith.getLength();
			if(replaceWithStartIndex>actualReplaceWithLength)
				throw OutOfRangeError("Invalid start index passed to String::replace");

			Iterator replaceWithStart = replaceWith.begin()+replaceWithStartIndex;

			Iterator replaceWithEnd( (replaceWithLength==toEnd || replaceWithStartIndex+replaceWithLength>=actualReplaceWithLength)
									? replaceWith.end()
									: (replaceWithStart+replaceWithLength) );

			return replace(rangeStartIndex, rangeLength, replaceWithStart, replaceWithEnd);
		}
	}


	/** Replaces a section of this string (defined by a start index and length) with the data between iterators that provide
		encoded string data in the format indicated by \c codec.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	template<class CODEC, class InputIterator>
	StringImpl& replaceEncoded(	const CODEC& codec,
								size_t rangeStartIndex,
								size_t rangeLength,
								InputIterator encodedReplaceWithBegin,
								InputIterator encodedReplaceWithEnd )
	{
		return replace( rangeStartIndex,
						rangeLength,
						typename CODEC::template DecodingIterator<InputIterator>( encodedReplaceWithBegin, encodedReplaceWithBegin, encodedReplaceWithEnd),
						typename CODEC::template DecodingIterator<InputIterator>( encodedReplaceWithEnd, encodedReplaceWithBegin, encodedReplaceWithEnd) );
	}


	/** Replaces a section of this string (defined by two iterators) with the data between iterators that provide
		encoded string data in the format indicated by \c codec.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	template<class CODEC, class InputIterator>
	StringImpl& replaceEncoded(	const CODEC& codec,
								const Iterator& rangeStart,
								const Iterator& rangeEnd,
								InputIterator encodedReplaceWithBegin,
								InputIterator encodedReplaceWithEnd )
	{
		return replace( rangeStart,
						rangeEnd,
						typename CODEC::template DecodingIterator<InputIterator>( encodedReplaceWithBegin, encodedReplaceWithBegin, encodedReplaceWithEnd),
						typename CODEC::template DecodingIterator<InputIterator>( encodedReplaceWithEnd, encodedReplaceWithBegin, encodedReplaceWithEnd) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-8 format.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in bytes.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const char* replaceWith,
							size_t replaceWithLength = toEnd )
	{
		return replaceEncoded(	Utf8Codec(),
								rangeStartIndex,
								rangeLength,
								replaceWith,
								getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.
		replaceWith must be in UTF-8 format.

		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in bytes.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeStart,
							const Iterator& rangeEnd,
							const char* replaceWith,
							size_t replaceWithLength = toEnd )
	{
		return replaceEncoded(	Utf8Codec(),
								rangeStart,
								rangeEnd,
								replaceWith,
								getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-8 format.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const std::string& replaceWith )
	{
		return replaceEncoded(	Utf8Codec(),
								rangeStartIndex,
								rangeLength,
								replaceWith.begin(),
								replaceWith.end() );
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.
		replaceWith must be in UTF-8 format.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeStart,
							const Iterator& rangeEnd,
							const std::string& replaceWith )
	{
		return replaceEncoded(	Utf8Codec(),
								rangeStart,
								rangeEnd,
								replaceWith.begin(),
								replaceWith.end() );
	}



	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-16 format.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in 16 bit elements.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const char16_t* replaceWith,
							size_t replaceWithLength = toEnd )
	{
		return replaceEncoded(	Utf16Codec(),
								rangeStartIndex,
								rangeLength,
								replaceWith,
								getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.
		replaceWith must be in UTF-16 format.

		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in 16 bit elements.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeStart,
							const Iterator& rangeEnd,
							const char16_t* replaceWith,
							size_t replaceWithLength = toEnd )
	{
		return replaceEncoded(	Utf16Codec(),
								rangeStart,
								rangeEnd,
								replaceWith,
								getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-16 format.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const std::u16string& replaceWith )
	{
		return replaceEncoded(	Utf16Codec(),
								rangeStartIndex,
								rangeLength,
								replaceWith.begin(),
								replaceWith.end() );
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.
		replaceWith must be in UTF-16 format.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeStart,
							const Iterator& rangeEnd,
							const std::u16string& replaceWith )
	{
		return replaceEncoded(	Utf16Codec(),
								rangeStart,
								rangeEnd,
								replaceWith.begin(),
								replaceWith.end() );
	}



	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-32 format (i.e. unencoded Unicode characters).

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in 32 bit elements.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const char32_t* replaceWith,
							size_t replaceWithLength = toEnd )
	{
		return replace( rangeStartIndex,
						rangeLength,
						replaceWith,
						getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.
		replaceWith must be in UTF-32 format (i.e. unencoded Unicode characters).

		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in 32 bit elements.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeStart,
							const Iterator& rangeEnd,
							const char32_t* replaceWith,
							size_t replaceWithLength = toEnd )
	{
		return replace( rangeStart,
						rangeEnd,
						replaceWith,
						getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-32 format (i.e. unencoded Unicode characters).

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.


		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const std::u32string& replaceWith )
	{
		return replace( rangeStartIndex,
						rangeLength,
						replaceWith.begin(),
						replaceWith.end() );
	}



	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.
		replaceWith must be in UTF-32 format (i.e. unencoded Unicode characters).

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeStart,
							const Iterator& rangeEnd,
							const std::u32string& replaceWith )
	{
		return replace( rangeStart,
						rangeEnd,
						replaceWith.begin(),
						replaceWith.end() );
	}



	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in wchar_t elements.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const wchar_t* replaceWith,
							size_t replaceWithLength = toEnd )
	{
		return replaceEncoded(	WideCodec(),
								rangeStartIndex,
								rangeLength,
								replaceWith,
								getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.

		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in wchar_t elements.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeStart,
							const Iterator& rangeEnd,
							const wchar_t* replaceWith,
							size_t replaceWithLength = toEnd )
	{
		return replaceEncoded(	WideCodec(),
								rangeStart,
								rangeEnd,
								replaceWith,
								getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const std::wstring& replaceWith )
	{
		return replaceEncoded(	WideCodec(),
								rangeStartIndex,
								rangeLength,
								replaceWith.begin(),
								replaceWith.end() );
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeStart,
							const Iterator& rangeEnd,
							const std::wstring& replaceWith )
	{
		return replaceEncoded(	WideCodec(),
								rangeStart,
								rangeEnd,
								replaceWith.begin(),
								replaceWith.end() );
	}


	/** Replaces a section of this string (defined by two iterators) with a sequence of characters.

		The charList parameters is automatically created by the compiler when you pass the characters as
		an initializer list.

		Example

		\code
		replace(start, end, {'a', 'b', 'c'} );
		\endcode

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(const Iterator& rangeStart, const Iterator& rangeEnd, std::initializer_list<char32_t> charList)
	{
		return replace(rangeStart, rangeEnd, charList.begin(), charList.end());
	}


	/** Replaces a section of this string (defined by a start index and length) with a sequence of characters.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		The charList parameters is automatically created by the compiler when you pass the characters as
		an initializer list.

		Example

		\code
		replace(start, end, {'a', 'b', 'c'} );
		\endcode

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							std::initializer_list<char32_t> charList)
	{
		return replace(rangeStartIndex, rangeLength, charList.begin(), charList.end());
	}



	/** Replaces a section of this string (defined by two iterators) with \c numChars occurrences of
		the character \c chr.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	const Iterator& rangeBegin,
							const Iterator& rangeEnd,
							size_t numChars,
							char32_t chr )
	{
		typename MainDataType::Codec::template EncodingIterator<const char32_t*> encodedBegin( &chr );
		typename MainDataType::Codec::template EncodingIterator<const char32_t*> encodedEnd( (&chr)+1 );

		// get the size of the encoded character
		int                                     encodedCharSize = 0;
		typename MainDataType::EncodedElement	lastEncodedElement=0;
		for( auto it = encodedBegin; it!=encodedEnd; it++)
		{
			lastEncodedElement = *it;
			encodedCharSize++;
		}

		// we must convert the range to encoded indices because the iterators
		// can be invalidated by Modify.
		size_t encodedRangeBeginIndex = rangeBegin.getInner() - _beginIt.getInner();
		size_t encodedRangeLength = rangeEnd.getInner() - rangeBegin.getInner();

		{
			Modify m(this);

			if(encodedCharSize==0 || numChars==0)
			{
				// we can use erase
				m.pStd->erase( encodedRangeBeginIndex, encodedRangeLength );
			}
			else if(encodedCharSize==1)
			{
				// we can use the std::string version of replace
				m.pStd->replace( encodedRangeBeginIndex, encodedRangeLength, numChars, lastEncodedElement );
			}
			else
			{
				// we must insert in a loop.
				// to make room we first fill with zero elements.

				m.pStd->replace( encodedRangeBeginIndex, encodedRangeLength, numChars*encodedCharSize, 0 );

				auto destIt = m.pStd->begin()+encodedRangeBeginIndex;
				for(size_t c=0; c<numChars; c++)
				{
					for( auto it = encodedBegin; it!=encodedEnd; it++)
					{
						*destIt = *it;
						destIt++;
					}
				}
			}
		}

		return *this;
	}


	/** Replaces a section of this string (defined by a start index and length) with \c numChars occurrences of
		the character \c chr.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::toEnd or String::npos3 or exceeds the end of the string then the end of the range is the end
		of the string.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							size_t numChars,
							char32_t chr )
	{
		size_t myLength = getLength();

		if(rangeStartIndex>myLength)
			throw OutOfRangeError("Invalid start index passed to String::replace");

		Iterator rangeStart( _beginIt + rangeStartIndex );

		Iterator rangeEnd( (rangeLength==toEnd || rangeStartIndex+rangeLength>=myLength)
							? _endIt
							: (rangeStart+rangeLength) );

		return replace(rangeStart, rangeEnd, numChars, chr);
	}




	/** Appends the specified string to the end of this string \c other.

		If otherSubStartIndex is specified then only the part of \c other starting from that index is
		appended. If otherSubStartIndex is bigger than the length of \c other then an OutOfRangeError is thrown.

		If otherSubLength is specified then at most this number of characters is copied from \c other.
		If \c other is not long enough for \c otherSubLength characters to be copied then only the available
		characters up to the end of \c other are copied.
	*/
	StringImpl& append(const StringImpl& other, size_t otherSubStartIndex=0, size_t otherSubLength=toEnd)
	{
		return replace( _endIt, _endIt, other, otherSubStartIndex, otherSubLength);
	}


	/** Appends the string data between the specified two iterators to the end of this string.*/
	template<class InputIterator>
	StringImpl& append(const InputIterator& beginIt, const InputIterator& endIt)
	{
		return replace( _endIt, _endIt, beginIt, endIt);
	}


	/** Appends the specified string to this string.*/
	StringImpl& append(const std::string& other)
	{
		return replace(_endIt, _endIt, other);
	}


	/** Appends the specified string to this string.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string in bytes.
	*/
	StringImpl& append(const char* other, size_t length=toEnd)
	{
		return replace(_endIt, _endIt, other, length);
	}


	/** Appends the specified string to this string.*/
	StringImpl& append(const std::u16string& o)
	{
		return replace(_endIt, _endIt, o);
	}


	/** Appends the specified string to this string.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string in 16 bit elements.
	*/
	StringImpl& append(const char16_t* o, size_t length=toEnd)
	{
		return replace(_endIt, _endIt, o, length);
	}


	/** Appends the specified string to this string.
	*/
	StringImpl& append(const std::u32string& o)
	{
		return replace( _endIt, _endIt, o );
	}


	/** Appends the specified string to this string.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string in 32 bit elements.
	*/
	StringImpl& append(const char32_t* o, size_t length=toEnd)
	{
		return replace( _endIt, _endIt, o, length);
	}


	/** Appends the specified string to this string.
	*/
	StringImpl& append(const std::wstring& o)
	{
		return replace(_endIt, _endIt, o );
	}


	/** Appends the specified string to this string.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string in wchar_t elements.
	*/
	StringImpl& append(const wchar_t* o, size_t length=toEnd)
	{
		return replace(_endIt, _endIt, o, length);
	}


	/** Appends \c numChars occurrences of \c chr to this string.
	*/
	StringImpl& append(size_t numChars, char32_t chr)
	{
		return replace(_endIt, _endIt, numChars, chr);
	}


	/** Appends a sequence of characters to this string.

		initializerList is automatically created by the compiler when you call this method
		with an initializer list.

		Example:

		\code
		myString.append( {'a', 'b', 'c' } );
		\endcode
	*/
	StringImpl& append(std::initializer_list<char32_t> initializerList)
	{
		return append(initializerList.begin(), initializerList.end());
	}


	/** Appends a single character to the end of this string.*/
	StringImpl& append(char32_t chr)
	{
		return append(1, chr);
	}


	/** Same as append(). Included for compatibility with std::string.*/
	void push_back(char32_t chr)
	{
		append(chr);
	}




	/** Inserts the specified string at the specified character index.

		If otherSubStartIndex is specified then only the part of \c other starting from that index is
		inserted. If otherSubStartIndex is bigger than the length of \c other then an OutOfRangeError is thrown.

		If otherSubLength is specified then at most this number of characters is copied from \c other.
		If \c other is not long enough for \c otherSubLength characters to be copied then only the available
		characters up to the end of \c other are copied.
	*/
	StringImpl& insert(size_t atIndex, const StringImpl& other, size_t otherSubStartIndex = 0, size_t otherSubLength = toEnd)
	{
		return insert(begin() + atIndex, other, otherSubStartIndex, otherSubLength);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

		If otherSubStartIndex is specified then only the part of \c other starting from that index is
		inserted. If otherSubStartIndex is bigger than the length of \c other then an OutOfRangeError is thrown.

		If otherSubLength is specified then at most this number of characters is copied from \c other.
		If \c other is not long enough for \c otherSubLength characters to be copied then only the available
		characters up to the end of \c other are copied.
	*/
	StringImpl& insert(const Iterator& atIt, const StringImpl& other, size_t otherSubStartIndex = 0, size_t otherSubLength = toEnd)
	{
		return replace(atIt, atIt, other, otherSubStartIndex, otherSubLength);
	}


	/** Inserts the specified string at the specified character index.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 8 bit elements.
	*/
	StringImpl& insert(size_t atIndex, const char* o, size_t length=toEnd)
	{
		return insert(begin() + atIndex, o, length);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 8 bit elements.
	*/
	StringImpl& insert(const Iterator& atIt, const char* o, size_t length = toEnd)
	{
		return replace(atIt, atIt, o, length);
	}


	/** Inserts the specified string at the specified character index.	*/
	StringImpl& insert(size_t atIndex, const std::string& other)
	{
		insert(begin() + atIndex, other);
		return *this;
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.*/
	StringImpl& insert(const Iterator& atIt, const std::string& other)
	{
		return replace(atIt, atIt, other);
	}



	/** Inserts the specified string at the specified character index.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded wchar_t elements.	*/
	StringImpl& insert(size_t atIndex, const wchar_t* o, size_t length = toEnd)
	{
		return insert(begin() + atIndex, o, length);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded wchar_t elements.
	*/
	StringImpl& insert(const Iterator& atIt, const wchar_t* o, size_t length = toEnd)
	{
		return replace(atIt, atIt, o, length);
	}


	/** Inserts the specified string at the specified character index.	*/
	StringImpl& insert(size_t atIndex, const std::wstring& other)
	{
		insert(begin() + atIndex, other);
		return *this;
	}

	/** Inserts the specified string at the position corresponding to the \c atIt iterator.*/
	StringImpl& insert(const Iterator& atIt, const std::wstring& other)
	{
		return replaceEncoded(WideCodec(), atIt, atIt, other.begin(), other.end());
	}


	/** Inserts the specified string at the specified character index.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 16 bit elements.	*/
	StringImpl& insert(size_t atIndex, const char16_t* o, size_t length = toEnd)
	{
		return insert(begin() + atIndex, o, length);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 16 bit elements.
	*/
	StringImpl& insert(const Iterator& atIt, const char16_t* o, size_t length = toEnd)
	{
		return replace(atIt, atIt, o, length);
	}



	/** Inserts the specified string at the specified character index.	*/
	StringImpl& insert(size_t atIndex, const std::u16string& other)
	{
		insert(begin() + atIndex, other);
		return *this;
	}

	/** Inserts the specified string at the position corresponding to the \c atIt iterator.*/
	StringImpl& insert(const Iterator& atIt, const std::u16string& other)
	{
		return replaceEncoded(Utf16Codec(), atIt, atIt, other.begin(), other.end());
	}



	/** Inserts the specified string at the specified character index.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 32 bit elements.	*/
	StringImpl& insert(size_t atIndex, const char32_t* other, size_t length = toEnd)
	{
		return insert(begin() + atIndex, other, length);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

	If \c length is not specified then other must be a zero terminated string.
	If it is specified then it indicates the length of the string to insert in encoded 32 bit elements.
	*/
	StringImpl& insert(const Iterator& atIt, const char32_t* other, size_t length = toEnd)
	{
		return replace(atIt, atIt, other, length);
	}


	/** Inserts the specified string at the specified character index.	*/
	StringImpl& insert(size_t atIndex, const std::u32string& other)
	{
		insert(begin() + atIndex, other);
		return *this;
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.*/
	StringImpl& insert(const Iterator& atIt, const std::u32string& other)
	{
		return replaceEncoded(Utf32Codec(), atIt, atIt, other.begin(), other.end());
	}


	/** Inserts the specified character numChar times at the specified character index.*/
	StringImpl& insert(size_t atIndex, size_t numChars, char32_t chr)
	{
		insert(begin() + atIndex, numChars, chr);
		return *this;
	}


	/** Inserts the specified character numChar times at the position indicated by \c atIt.

		Returns an Iterator that points to the first inserted character. If numChars is 0 and no
		character is inserted then it returns an iterator pointing to the position indicated by \c atIt.
	*/
	Iterator insert(const Iterator& atIt, size_t numChars, char32_t chr)
	{
		int encodedInsertIndex = atIt.getInner() - _beginIt.getInner();

		replace(atIt, atIt, numChars, chr);

		// we must return an iterator to the first inserted character
		return Iterator(_beginIt.getInner() + encodedInsertIndex, _beginIt.getInner(), _endIt.getInner());
	}



	/** Inserts the specified character at the specified character index.

		Returns an Iterator that points to the first inserted character. If numChars is 0 and no
		character is inserted then it returns an iterator pointing to the position indicated by \c atIt.
		*/
	StringImpl& insert(size_t atIndex, char32_t chr)
	{
		return insert(atIndex, 1, chr);
	}

	/** Inserts the specified character at the position indicated by \c atIt.

		Returns an Iterator that points to the first inserted character. If numChars is 0 and no
		character is inserted then it returns an iterator pointing to the position indicated by \c atIt.
		*/
	Iterator insert(const Iterator& atIt, char32_t chr)
	{
		return insert(atIt, 1, chr);
	}


	/** Inserts the data between the two character iterators toInsertBegin and toInsertEnd
		at the character index \c atIndex.
		*/
	template <class IT>
	StringImpl& insert(size_t atIndex, const IT& toInsertBegin, const IT& toInsertEnd)
	{
		return replace(atIndex, 0, toInsertBegin, toInsertEnd);
	}


	/** Inserts the data between the two character iterators toInsertBegin and toInsertEnd
		at the position indicated by \c atIt.

		Returns an Iterator that points to the first inserted character. If numChars is 0 and no
		character is inserted then it returns an iterator pointing to the position indicated by \c atIt.
		*/
	template <class IT>
	Iterator insert(Iterator atIt, const IT& toInsertBegin, const IT& toInsertEnd)
	{
		int encodedInsertIndex = atIt.getInner() - _beginIt.getInner();

		replace(atIt, atIt, toInsertBegin, toInsertEnd);

		// we must return an iterator to the first inserted character
		return Iterator(_beginIt.getInner() + encodedInsertIndex, _beginIt.getInner(), _endIt.getInner());
	}


	/** Inserts a sequence of characters at the character index \c atIndex.

		initializerList is automatically created by the compiler when you call this method
		with an initializer list.

		Example:

		\code
		myString.insert(atIndex, {'a', 'b', 'c' } );
		\endcode
		*/
	StringImpl& insert(size_t atIndex, std::initializer_list<char32_t> initializerList)
	{
		return insert(atIndex, initializerList.begin(), initializerList.end());
	}



	/** Inserts a sequence of characters at the position indicated by \c atIt.

		initializerList is automatically created by the compiler when you call this method
		with an initializer list.

		Example:

		\code
		myString.insert(atIt, {'a', 'b', 'c' } );
		\endcode
		*/
	StringImpl& insert(const Iterator& atIt, std::initializer_list<char32_t> initializerList)
	{
		insert(atIt, initializerList.begin(), initializerList.end());
		return *this;
	}




	/** Removes a part of the string, starting at the character index \c cutIndex and
		cutting out \c cutLength characters from that position.

		If cutLength is String::toEnd or String::npos or cutPos+cutLength exceed the lengths of the
		string then the remainder of the string up to the end is removed.
	*/
	StringImpl& erase(size_t cutIndex=0, size_t cutLength=toEnd)
	{
		return replace(cutIndex, cutLength, U"", 0);
	}


	/** Removes the character at the position of the specified iterator.
		Returns an iterator to the character that now occupies the position of the removed
		character (or end() if it was the last character).*/
	Iterator erase(const Iterator& it)
	{
		int encodedEraseIndex = it.getInner() - _beginIt.getInner();

		replace(it, it+1,  U"", 0);

		// we must return an iterator to the erased position
		return Iterator(_beginIt.getInner() + encodedEraseIndex, _beginIt.getInner(), _endIt.getInner());
	}


	/** Removes a part of the string, starting at the position indicated by
		the \c beginIt iterator and ending at the point just before the position indicated by \c endIt.

		Returns an iterator to the character that now occupies the position of the first removed
		character (or end() if the removed part extended to the end of the string).*/
	Iterator erase(const Iterator& beginIt, const Iterator& endIt)
	{
		int encodedEraseIndex = beginIt.getInner() - _beginIt.getInner();

		replace(beginIt, endIt,  U"", 0);

		// we must return an iterator to the erased position
		return Iterator(_beginIt.getInner() + encodedEraseIndex, _beginIt.getInner(), _endIt.getInner());
	}


	/** Erases the entire contents of the string. The string becomes an empty string.*/
	void clear() noexcept
	{
		_pData = MainDataType::getEmptyData();
		_beginIt = _pData->begin();
		_endIt = _pData->end();

		_pDataInDifferentEncoding = nullptr;

		_lengthIfKnown = 0;
	}


	/** Assigns the value of another string to this string.

		If otherSubStartIndex is specified then only the part of \c other starting from that index is
		assigned. If otherSubStartIndex is bigger than the length of \c other then an OutOfRangeError is thrown.

		If otherSubLength is specified then at most this number of characters is copied from \c other.
		If \c other is not long enough for \c otherSubLength characters to be copied then only the available
		characters up to the end of \c other are copied.
		*/
	StringImpl& assign(const StringImpl& other, size_t otherSubStartIndex=0, size_t otherSubLength=toEnd)
	{
		if(otherSubStartIndex>other.getLength())
			throw OutOfRangeError("Invalid otherSubStartIndex passed to String::assign");

		// just copy a reference to the source string's data
		_pData = other._pData;

		_beginIt = other._beginIt;

		if(otherSubStartIndex>0)
			_beginIt += otherSubStartIndex;

		if(otherSubLength==toEnd || otherSubStartIndex+otherSubLength>=other.length() )
		{
			_endIt = other._endIt;

			if(other._lengthIfKnown==-1)
				_lengthIfKnown = -1;
			else
				_lengthIfKnown = other._lengthIfKnown-otherSubStartIndex;

			if(otherSubStartIndex==0)
				_pDataInDifferentEncoding = other._pDataInDifferentEncoding;
			else
				_pDataInDifferentEncoding = nullptr;
		}
		else
		{
			_endIt = _beginIt + otherSubLength;
			_lengthIfKnown = otherSubLength;

			_pDataInDifferentEncoding = nullptr;
		}

		return *this;
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& assign(const std::string& o)
	{
		return replace(_beginIt, _endIt, o );
	}


	/** Assigns the value of a C-style UTF8 string to this string. If length is not String::toEnd or String::npos
		then it must be the length of the encoded UTF-8 string in bytes.
		If length is String::toEnd or String::npos then the other string must be zero-terminated.
		*/
	StringImpl& assign(const char* o, size_t length=toEnd)
	{
		return replace(_beginIt, _endIt, o, length );
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& assign(const std::wstring& o)
	{
		return replace(_beginIt, _endIt, o);
	}


	/** Assigns the value of a C-style wide char string to this string. If length is not String::toEnd or String::npos
		then it must be the number of encoded wchar_t elements.
		If length is String::toEnd or String::npos then the other string must be zero-terminated.
		*/
	StringImpl& assign(const wchar_t* o, size_t length=toEnd)
	{
		return replace(_beginIt, _endIt, o, length );
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& assign(const std::u16string& o)
	{
		return replace(_beginIt, _endIt, o );
	}


	/** Assigns the value of a C-style UTF-16 string to this string. If length is not String::toEnd or String::npos
		then it must be the number of encoded 16 bit UTF-16 elements.
		If length is String::toEnd or String::npos then the other string must be zero-terminated.
		*/
	StringImpl& assign(const char16_t* o, size_t length=toEnd)
	{
		return replace(_beginIt, _endIt, o, length );
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& assign(const std::u32string& o)
	{
		return replace(_beginIt, _endIt, o );
	}


	/** Assigns the value of a C-style UTF-32 string to this string. If length is not String::toEnd or String::npos
		then it must be the number of encoded 32 bit UTF-32 elements.
		If length is String::toEnd or String::npos then the other string must be zero-terminated.
		*/
	StringImpl& assign(const char32_t* o, size_t length=toEnd)
	{
		return replace(_beginIt, _endIt, o, length );
	}



	/** Sets the contents of this string to be \c numChars times the \c chr character.
		*/
	StringImpl& assign(size_t numChars, char32_t chr)
	{
		return replace(_beginIt, _endIt, numChars, chr);
	}


	/** Assigns the characters between the specified two character iterators to this string.
		*/
	template <class InputIterator>
	StringImpl& assign(InputIterator beginIt, InputIterator endIt)
	{
		return replace(_beginIt, _endIt, beginIt, endIt);
	}


	/** Sets the contents of this string to be a sequence of character.

		initializerList is automatically created by the compiler when you call this method
		with an initializer list.

		Example:

		\code
		myString.assign( {'a', 'b', 'c' } );
		\endcode
		*/
	StringImpl& assign(std::initializer_list<char32_t> initializerList)
	{
		return replace(_beginIt, _endIt, initializerList);
	}



	/** Move-optimized version of assign() that "steals" the contents from another string.
		Sets the contents of this string to the contents of
		\c moveSource. Afterwards moveSource will contain only the empty string.
		*/
	StringImpl& assign(StringImpl&& moveSource) noexcept
	{
		// just copy everything over
		_pData = moveSource._pData;
		_beginIt = moveSource._beginIt;
		_endIt = moveSource._endIt;
		_pDataInDifferentEncoding = moveSource._pDataInDifferentEncoding;
		_lengthIfKnown = moveSource._lengthIfKnown;

		// we could leave moveSource like this. The result state of moveSource is unspecified, but must be valid.
		// So it is perfectly OK to leave it at the current value.
		// BUT most std::string implementation will almost certainly set moveSource to an empty string,
		// because they cannot implement data sharing. So we do the same.
		// Also, if we left the data in a shared state and moveSource is not destroyed immediately, then we might
		// end up having to copy our data at the next modifying operation (because the refCount is not 1).
		// That is something that we should avoid, since the move operation is intended to be super-fast and
		// avoid just this kind of copying.

		moveSource._pData = MainDataType::getEmptyData();
		moveSource._beginIt = moveSource._pData->begin();
		moveSource._endIt = moveSource._pData->end();
		moveSource._pDataInDifferentEncoding = nullptr;
		moveSource._lengthIfKnown = 0;

		return *this;
	}



	/** Swaps the contents between this string and the specified one.
		Afterwards the string passed in will have the contents of this string and this string
		will have the contents of the parameter.*/
	void swap(StringImpl& o)
	{
		P<MainDataType> pData = _pData;
		Iterator		beginIt = _beginIt;
		Iterator		endIt = _endIt;
		P<Base>			pDataInDifferentEncoding = _pDataInDifferentEncoding;
		int				lengthIfKnown = _lengthIfKnown;

		_pData = o._pData;
		_beginIt = o._beginIt;
		_endIt = o._endIt;
		_pDataInDifferentEncoding = o._pDataInDifferentEncoding;
		_lengthIfKnown = o._lengthIfKnown;

		o._pData = pData;
		o._beginIt = beginIt;
		o._endIt = endIt;
		o._pDataInDifferentEncoding = pDataInDifferentEncoding;
		o._lengthIfKnown = lengthIfKnown;
	}




	/** Removes the last character from the string.
		Has no effect if the string is empty.*/
	void removeLast()
	{
		if(!isEmpty())
			erase(_endIt-1);
	}


	/** Same as removeLast(). Included for compatibility with std::string.*/
	void pop_back()
	{
		if(!isEmpty())
			erase(_endIt-1);
	}


	/** Returns a copy of the allocator object associated with the string.*/
	Allocator getAllocator() const noexcept
	{
		return _pData->getEncodedString().get_allocator();
	}


	/** Same as getAllocator(). This is included for compatibility with std::string.*/
	allocator_type get_allocator() const noexcept
	{
		return getAllocator();
	}


	/** Copies characters from the string to the specified buffer. No null-terminator
		is added to the output buffer.

		maxCopyLength indicates the maximum number of characters to copy. If this exceeds the length of
		the string then only characters up to the end are copied.

		Returns the number of characters that were copied.

		If copyStartIndex is bigger than the length of the string then OutOfRangeError is thrown.
		*/
	size_t copy(char32_t* pDest, size_t maxCopyLength, size_t copyStartIndex=0) const
	{
		if(copyStartIndex<0 || copyStartIndex>getLength())
			throw OutOfRangeError("String::copy called with invalid start index.");

		Iterator it = _beginIt+copyStartIndex;
		for(size_t i=0; i<maxCopyLength; i++)
		{
			if(it==_endIt)
				return i;

			pDest[i] = *it;

			++it;
		}

		return maxCopyLength;
	}




	/** Checks if the string contains the character \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(char32_t toFind) const
	{
		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const StringImpl& toFind) const
	{
		if(toFind.isEmpty())
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const std::string& toFind) const
	{
		if(toFind.empty())
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const std::wstring& toFind) const
	{
		if(toFind.empty())
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const std::u16string& toFind) const
	{
		if(toFind.empty())
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const std::u32string& toFind) const
	{
		if(toFind.empty())
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const char* toFind) const
	{
		if(toFind[0]==0)
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const wchar_t* toFind) const
	{
		if(toFind[0]==0)
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const char16_t* toFind) const
	{
		if(toFind[0]==0)
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string \c toFind.

		Returns true if \c toFind was found and false otherwise.

		Always returns true if \c toFind is empty.
	*/
	bool contains(const char32_t* toFind) const
	{
		if(toFind[0]==0)
			return true;

		return ( find(toFind, _beginIt) != _endIt );
	}


	/** Checks if the string contains the string specified by the character iterators toFindBegin and toFindEnd.

		Returns true if the string was found and false otherwise.

		Always returns true if the specified string to find is empty.
	*/
	template<class ToFindIteratorType>
	bool contains(const ToFindIteratorType& toFindBegin, const ToFindIteratorType& toFindEnd) const
	{
		if(toFindBegin==toFindEnd)
			return true;

		return ( find<ToFindIteratorType>(toFindBegin, toFindEnd, _beginIt) != _endIt );
	}



	/** Searches for a sequence of characters in this string, starting at the positing indicated by \c searchFromIt.

		Returns an iterator to the first character of the first occurrence of the sequence if it is found.
		Returns end() if the sequence is not found.

		If the sequence of characters between toFindBeginIt and toFindEndIt is empty then searchFromIt is returned.

		If pMatchEndIt is not null and the toFind sequence is found, then *pMatchEndIt is set to the first character
		following the found sequence. If the sequence ends at the end of the string the *pMatchEndIt is set to end().

		If pMatchEndIt is not null and the toFind sequence is not found then *pMatchEndIt is set to end().
	*/
	template<class ToFindIteratorType>
	Iterator find(const ToFindIteratorType& toFindBeginIt, const ToFindIteratorType& toFindEndIt, const Iterator& searchFromIt, Iterator* pMatchEndIt = nullptr) const
	{
		if(pMatchEndIt==nullptr)
		{
			// we can use std::search. We assume that it might be more optimized than our algorithm, so we prefer
			// the standard one.
			return std::search( searchFromIt, _endIt, toFindBeginIt, toFindEndIt );
		}
		else
		{
			Iterator matchBeginIt( searchFromIt );

			while(matchBeginIt!=_endIt)
			{
				Iterator			myIt( matchBeginIt );
				ToFindIteratorType	toFindIt( toFindBeginIt );

				bool matches = true;

				while(toFindIt!=toFindEndIt)
				{
					if(myIt==_endIt)
					{
						// no more occurrences possible.
						*pMatchEndIt = end();
						return end();
					}

					if(*myIt != *toFindIt)
					{
						// no match
						matches = false;
						break;
					}

					++myIt;
					++toFindIt;
				}

				if(matches)
				{
					*pMatchEndIt = myIt;
					return matchBeginIt;
				}

				++matchBeginIt;
			}

			*pMatchEndIt = end();
			return end();
		}
	}


	/** Searches for another string in this string, starting at the positing indicated by \c searchFromIt.

		Returns an iterator to the first character of the first occurrence of \c toFind if it is found.
		Returns end() if \c toFind is not found.

		If \c toFind is empty then searchFromIt is returned.

		If pMatchEndIt is not null and toFind is found, then *pMatchEndIt is set to the first character
		following the end of the match. If the match ends at the end of the string the *pMatchEndIt is set to end().

		If pMatchEndIt is not null and toFind is not found then *pMatchEndIt is set to end().
	*/
	Iterator find(const StringImpl& toFind, const Iterator& searchFromIt, Iterator* pMatchEndIt = nullptr) const
	{
		if(pMatchEndIt==nullptr)
			return std::search( searchFromIt, _endIt, toFind._beginIt, toFind._endIt );
		else
			return find(toFind._beginIt, toFind._endIt, searchFromIt, pMatchEndIt );
	}



	/** Searches for another string in this string.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find(const StringImpl& toFind, size_t searchStartIndex = 0) const noexcept
	{
		if(searchStartIndex>getLength())
			return noMatch;

		if(toFind.isEmpty())
			return searchStartIndex;

		IteratorWithIndex foundIt = std::search( IteratorWithIndex( _beginIt+searchStartIndex, searchStartIndex),
												 IteratorWithIndex( _endIt, getLength() ),
												 toFind._beginIt,
												 toFind._endIt );
		if(foundIt.getInner()==_endIt)
			return noMatch;
		else
			return foundIt.getIndex();
	}


	/** Searches for a sequence of encoded characters in this string.

		encodedToFindBeginIt and encodedToFindEndIt define the beginning and end of the encoded string data
		to search for.

		\c codec is a string codec object (like Utf8Codec, ...) that defines the encoding of the encoded
		string data. The encoding does not have the match the internal encoding of this string. Any encoding
		can be used.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).

		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the character index of the first character of the first occurrence if it is found.
		Returns String::noMatch (String::npos) if the string is not found.

		If \c the string to search for is empty then searchStartIndex is returned.
	*/
	template<class ToFindCodec, class EncodedIt>
	size_t findEncoded(const ToFindCodec& codec, const EncodedIt& encodedToFindBeginIt, const EncodedIt& encodedToFindEndIt, size_t searchStartIndex = 0) const
	{
		if(searchStartIndex>getLength())
			return noMatch;

		if(encodedToFindBeginIt==encodedToFindEndIt)
			return searchStartIndex;

		IteratorWithIndex foundIt = std::search( IteratorWithIndex( _beginIt+searchStartIndex, searchStartIndex),
												 IteratorWithIndex( _endIt, getLength() ),
												 typename ToFindCodec::template DecodingIterator<EncodedIt>(encodedToFindBeginIt, encodedToFindBeginIt, encodedToFindEndIt),
												 typename ToFindCodec::template DecodingIterator<EncodedIt>(encodedToFindEndIt, encodedToFindBeginIt, encodedToFindEndIt) );
		if(foundIt.getInner()==_endIt)
			return noMatch;
		else
			return foundIt.getIndex();
	}


	/** Searches for another string in this string.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find(const std::string& toFind, size_t searchStartIndex = 0) const
	{
		return findEncoded(Utf8Codec(), toFind.begin(), toFind.end(), searchStartIndex);
	}


	/** Searches for another string in this string.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find(const std::wstring& toFind, size_t searchStartIndex = 0) const
	{
		return findEncoded(WideCodec(), toFind.begin(), toFind.end(), searchStartIndex);
	}


	/** Searches for another string in this string.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find(const std::u16string& toFind, size_t searchStartIndex = 0) const
	{
		return findEncoded(Utf16Codec(), toFind.begin(), toFind.end(), searchStartIndex);
	}


	/** Searches for another string in this string.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find(const std::u32string& toFind, size_t searchStartIndex = 0) const
	{
		return findEncoded(Utf32Codec(), toFind.begin(), toFind.end(), searchStartIndex);
	}


	/** Searches for another string in this string.

		If toFindLength is String::toEnd or String::npos then toFind must be a zero-terminated string
		and the whole string is searched for. If toFindLength is not String::toEnd / String::npos then
		toFindLength indicates the length of toFind in encoded bytes.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find (const char* toFind, size_t searchStartIndex = 0, size_t toFindLength = toEnd) const
	{
		return findEncoded(Utf8Codec(), toFind, getStringEndPtr(toFind, toFindLength), searchStartIndex);
	}


	/** Searches for another string in this string.

		If toFindLength is String::toEnd or String::npos then toFind must be a zero-terminated string
		and the whole string is searched for. If toFindLength is not String::toEnd / String::npos then
		toFindLength indicates the length of toFind in wchar_t elements.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find (const wchar_t* toFind, size_t searchStartIndex = 0, size_t toFindLength = toEnd) const
	{
		return findEncoded(WideCodec(), toFind, getStringEndPtr(toFind, toFindLength), searchStartIndex);
	}


	/** Searches for another string in this string.

		If toFindLength is String::toEnd or String::npos then toFind must be a zero-terminated string
		and the whole string is searched for. If toFindLength is not String::toEnd / String::npos then
		toFindLength indicates the length of toFind in char16_t elements.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find (const char16_t* toFind, size_t searchStartIndex = 0, size_t toFindLength = toEnd) const
	{
		return findEncoded(Utf16Codec(), toFind, getStringEndPtr(toFind, toFindLength), searchStartIndex);
	}


	/** Searches for another string in this string.

		If toFindLength is String::toEnd or String::npos then toFind must be a zero-terminated string
		and the whole string is searched for. If toFindLength is not String::toEnd / String::npos then
		toFindLength indicates the length of toFind in char32_t elements.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first character of the first occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned.
	*/
	size_t find (const char32_t* toFind, size_t searchStartIndex = 0, size_t toFindLength = toEnd) const
	{
		return findEncoded(Utf32Codec(), toFind, getStringEndPtr(toFind, toFindLength), searchStartIndex);
	}




	/** Searches for the specified character in this string, starting at the position indicated by the \c searchStartPosIt.

		Returns an Iterator pointing to the first occurrence of \c charToFind, if it is found.
		Returns end() if \c charToFind is not found.
	*/
	Iterator find(char32_t charToFind, const Iterator& searchStartPosIt) const noexcept
	{
		return std::find( searchStartPosIt, _endIt, charToFind );
	}


	/** Searches for the specified character in this string.

		searchStartIndex is the start index in this string, where the search should begin (default is 0).
		If searchStartIndex is bigger than the length of the string then the return value is always String::noMatch
		(which is the same as String::npos).

		Returns the index of the first occurrence of \c charToFind if it is found.
		Returns String::noMatch (String::npos) if \c charToFind is not found.
	*/
	size_t find(char32_t charToFind, size_t searchStartIndex = 0) const noexcept
	{
		if(searchStartIndex>getLength())
			return noMatch;

		IteratorWithIndex foundIt = std::find(	IteratorWithIndex( _beginIt+searchStartIndex, searchStartIndex),
												IteratorWithIndex( _endIt, getLength() ),
												charToFind );
		if(foundIt.getInner()==_endIt)
			return noMatch;
		else
			return foundIt.getIndex();
	}





	/** Searches for the LAST occurrence of a sequence of characters in this string.

		searchFromIt is the position of the last character in the string to be considered as the beginning of a match.
		If searchFromIt is end() then the entire string is searched.

		Returns an iterator to the first character of the last occurrence of the sequence if it is found.
		Returns end() if the sequence is not found.

		If the sequence of characters between toFindBeginIt and toFindEndIt is empty then searchFromIt is returned.

		If pMatchEndIt is not null and the toFind sequence is found, then *pMatchEndIt is set to the first character
		following the found sequence. If the sequence ends at the end of the string then *pMatchEndIt is set to end().

		If pMatchEndIt is not null and the toFind sequence is not found then *pMatchEndIt is set to end().
	*/
	template<class CHARIT>
	Iterator reverseFind(const CHARIT& toFindBeginIt, const CHARIT& toFindEndIt, const Iterator& searchFromIt, Iterator* pMatchEndIt = nullptr) const
	{
		if(toFindBeginIt==toFindEndIt)
		{
			if(pMatchEndIt!=nullptr)
				*pMatchEndIt = searchFromIt;
			return searchFromIt;
		}

		Iterator matchBeginIt( searchFromIt );

		if(matchBeginIt==_endIt && matchBeginIt!=_beginIt)
			--matchBeginIt;

		while(true)
		{
			Iterator myIt( matchBeginIt );
			Iterator toFindIt( toFindBeginIt );

			bool matches = true;

			while(toFindIt!=toFindEndIt)
			{
				if(myIt==_endIt)
				{
					matches = false;
					break;
				}

				if(*myIt != *toFindIt)
				{
					// no match
					matches = false;
					break;
				}

				++myIt;
				++toFindIt;
			}

			if(matches)
			{
				if(pMatchEndIt!=nullptr)
					*pMatchEndIt = myIt;
				return matchBeginIt;
			}

			if(matchBeginIt==_beginIt)
				break;

			--matchBeginIt;
		}

		if(pMatchEndIt!=nullptr)
			*pMatchEndIt = _endIt;
		return _endIt;
	}


	/** Searches for the LAST occurrence of a string in this string.

		searchFromIt is the position of the last character in the string to be considered as the beginning of a match.
		If searchFromIt is end() then the entire string is searched.

		Returns an iterator to the first character of the last occurrence of \c toFind if it is found.
		Returns end() if \c toFind is not found.

		If \c toFind is empty then searchFromIt is returned.

		If pMatchEndIt is not null and toFind is found, then *pMatchEndIt is set to the first character
		following the found sequence. If the match ends at the end of the string then *pMatchEndIt is set to end().

		If pMatchEndIt is not null and toFind is not found then *pMatchEndIt is set to end().
	*/
	Iterator reverseFind(const StringImpl& toFind, const Iterator& searchFromIt, Iterator* pMatchEndIt = nullptr) const
	{
		return reverseFind(toFind._beginIt, toFind._endIt, searchFromIt, pMatchEndIt);
	}



	/** Searches for the LAST occurrence of another string in this string.

		The string to search for is specified as a pair of iterators. They must yield full Unicode
		characters (char32_t).

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If the \c toFind sequence is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	template<class InputIterator>
	size_t reverseFind(const InputIterator& toFindBeginIt, const InputIterator& toFindEndIt, size_t searchStartIndex = npos) const noexcept
	{
		size_t myLength = getLength();

		if(searchStartIndex==npos || searchStartIndex>myLength)
			searchStartIndex = myLength;

		if(toFindBeginIt==toFindEndIt)
			return searchStartIndex;

		size_t toFindLength = 0;
		for(InputIterator it(toFindBeginIt); it!=toFindEndIt; ++it)
			toFindLength++;

		if(myLength<toFindLength)
			return noMatch;

		if(searchStartIndex > myLength-toFindLength)
			searchStartIndex = myLength-toFindLength;

		IteratorWithIndex matchBeginIt( _beginIt+searchStartIndex, searchStartIndex);

		while(true)
		{
			Iterator		myIt( matchBeginIt.getInner() );
			InputIterator	toFindIt( toFindBeginIt );

			bool matches = true;

			while(toFindIt != toFindEndIt )
			{
				// we already know that we have enough characters left in the string for a match.
				// So no need to check myIt boundaries here.

				if(*myIt != *toFindIt)
				{
					// no match
					matches = false;
					break;
				}

				++myIt;
				++toFindIt;
			}

			if(matches)
				return matchBeginIt.getIndex();

			if(matchBeginIt.getInner()==_beginIt)
				break;

			--matchBeginIt;
		}

		return noMatch;
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind(const StringImpl& toFind, size_t searchStartIndex = npos) const noexcept
	{
		return reverseFind(toFind._beginIt, toFind._endIt, searchStartIndex);
	}


	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const StringImpl& toFind, size_t searchStartIndex = npos) const noexcept
	{
		return reverseFind(toFind, searchStartIndex);
	}


	/** Searches for the LAST occurrence of a sequence of encoded characters in this string.

		encodedToFindBeginIt and encodedToFindEndIt define the beginning and end of the encoded string data
		to search for.

		\c codec is a string codec object (like Utf8Codec, ...) that defines the encoding of the encoded
		string data. The encoding does not have the match the internal encoding of this string. Any encoding
		can be used.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		Returns the index of the first character of the last occurrence of the sequence, if it is found.
		Returns String::noMatch (String::npos) if it is not found.

		If \c the toFind sequence is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	template<class ToFindCodec, class EncodedIt>
	size_t reverseFindEncoded(const ToFindCodec& codec, const EncodedIt& encodedToFindBeginIt, const EncodedIt& encodedToFindEndIt, size_t searchStartIndex = npos) const
	{
		return reverseFind( typename ToFindCodec::template DecodingIterator<EncodedIt>(encodedToFindBeginIt, encodedToFindBeginIt, encodedToFindEndIt),
							typename ToFindCodec::template DecodingIterator<EncodedIt>(encodedToFindEndIt, encodedToFindBeginIt, encodedToFindEndIt),
							searchStartIndex );
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind(const std::string& toFind, size_t searchStartIndex = npos) const
	{
		return reverseFindEncoded(Utf8Codec(), toFind.begin(), toFind.end(), searchStartIndex);
	}


	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const std::string& toFind, size_t searchStartIndex = npos) const
	{
		return reverseFind(toFind, searchStartIndex);
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind(const std::wstring& toFind, size_t searchStartIndex = npos) const
	{
		return reverseFindEncoded(WideCodec(), toFind.begin(), toFind.end(), searchStartIndex);
	}

	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const std::wstring& toFind, size_t searchStartIndex = npos) const
	{
		return reverseFind(toFind, searchStartIndex);
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind(const std::u16string& toFind, size_t searchStartIndex = npos) const
	{
		return reverseFindEncoded(Utf16Codec(), toFind.begin(), toFind.end(), searchStartIndex);
	}

	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const std::u16string& toFind, size_t searchStartIndex = npos) const
	{
		return reverseFind(toFind, searchStartIndex);
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind(const std::u32string& toFind, size_t searchStartIndex = npos) const
	{
		return reverseFindEncoded(Utf32Codec(), toFind.begin(), toFind.end(), searchStartIndex);
	}

	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const std::u32string& toFind, size_t searchStartIndex = npos) const
	{
		return reverseFind(toFind, searchStartIndex);
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		If toFindLength is String::toEnd or String::npos then toFind must be a zero-terminated string
		and the whole string is searched for. If toFindLength is not String::toEnd / String::npos then
		toFindLength indicates the length of toFind in encoded bytes.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind (const char* toFind, size_t searchStartIndex = npos, size_t toFindLength = toEnd) const
	{
		return reverseFindEncoded(Utf8Codec(), toFind, getStringEndPtr(toFind, toFindLength), searchStartIndex);
	}

	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const char* toFind, size_t searchStartIndex = npos, size_t toFindLength = toEnd) const
	{
		return reverseFind(toFind, searchStartIndex, toFindLength);
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		If toFindLength is String::toEnd or String::npos then toFind must be a zero-terminated string
		and the whole string is searched for. If toFindLength is not String::toEnd / String::npos then
		toFindLength indicates the length of toFind in encoded wchar_t elements.


		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind (const wchar_t* toFind, size_t searchStartIndex = npos, size_t toFindLength = toEnd) const
	{
		return reverseFindEncoded(WideCodec(), toFind, getStringEndPtr(toFind, toFindLength), searchStartIndex);
	}


	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const wchar_t* toFind, size_t searchStartIndex = npos, size_t toFindLength = toEnd) const
	{
		return reverseFind(toFind, searchStartIndex, toFindLength);
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		If toFindLength is String::toEnd or String::npos then toFind must be a zero-terminated string
		and the whole string is searched for. If toFindLength is not String::toEnd / String::npos then
		toFindLength indicates the length of toFind in encoded char16_t.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind (const char16_t* toFind, size_t searchStartIndex = npos, size_t toFindLength = toEnd) const
	{
		return reverseFindEncoded(Utf16Codec(), toFind, getStringEndPtr(toFind, toFindLength), searchStartIndex);
	}

	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const char16_t* toFind, size_t searchStartIndex = npos, size_t toFindLength = toEnd) const
	{
		return reverseFind(toFind, searchStartIndex, toFindLength);
	}


	/** Searches for the LAST occurrence of another string in this string.

		searchStartIndex is the index of the last character in the string to be considered as the possible beginning of a match.
		If searchStartIndex is npos or bigger or equal to the length of the string then the entire string is searched.

		If toFindLength is String::toEnd or String::npos then toFind must be a zero-terminated string
		and the whole string is searched for. If toFindLength is not String::toEnd / String::npos then
		toFindLength indicates the length of toFind in char32_t elements.

		Returns the index of the first character of the last occurrence of \c toFind if it is found.
		Returns String::noMatch (String::npos) if \c toFind is not found.

		If \c toFind is empty then searchStartIndex is returned, unless it is npos or bigger than the length of the string.
		in which case the length of the string is returned.
	*/
	size_t reverseFind (const char32_t* toFind, size_t searchStartIndex = npos, size_t toFindLength = toEnd) const
	{
		return reverseFindEncoded(Utf32Codec(), toFind, getStringEndPtr(toFind, toFindLength), searchStartIndex);
	}

	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(const char32_t* toFind, size_t searchStartIndex = npos, size_t toFindLength = toEnd) const
	{
		return reverseFind(toFind, searchStartIndex, toFindLength);
	}




	/** Searches for the LAST occurrence of the specified character in this string.

		searchStartPosIt indicates the last position to consider for a possible match.
		If searchStartPosIt is end() then the whole string is searched.

		Returns an Iterator pointing to the last occurrence of \c charToFind, if it is found.
		Returns end() if \c charToFind is not found.
	*/
	Iterator reverseFind(char32_t charToFind, const Iterator& searchStartPosIt) const noexcept
	{
		Iterator myIt( searchStartPosIt );

		if(myIt==_endIt)
		{
			if(myIt==_beginIt)
				return _endIt;

			--myIt;
		}

		while(true)
		{
			if(*myIt == charToFind)
				return myIt;

			if(myIt==_beginIt)
				break;

			--myIt;
		}

		return _endIt;
	}


	/** Searches for the last occurrence of a character in this string.

		searchStartIndex is the index of the last character in this string that is considered as a possible match.
		If searchStartIndex is String::npos or bigger or equal to the length of the string then the entire string is searched.

		Returns the index of the last occurrence of \c charToFind if it is found.
		Returns String::noMatch (String::npos) if \c charToFind is not found.
	*/
	size_t reverseFind(char32_t charToFind, size_t searchStartIndex = npos) const noexcept
	{
		if(_beginIt==_endIt)
			return noMatch;

		size_t myLength = length();

		size_t index = (searchStartIndex==npos || searchStartIndex>myLength-1) ? (myLength-1) : searchStartIndex;

		Iterator myIt( (index==myLength-1) ? (_endIt-1) : (_beginIt+index) );

		while(true)
		{
			if(*myIt == charToFind)
				return index;

			if(myIt==_beginIt)
				break;

			--myIt;
			--index;
		}

		return noMatch;
	}

	/** Same as reverseFind(). Included for compatibility with std::string. */
	size_t rfind(char32_t charToFind, size_t searchStartIndex = npos) const
	{
		return reverseFind(charToFind, searchStartIndex);
	}



	/** Searches for the first position at which the specified condition is true.

		condition must be a callable object (like a function object or a lambda function).
		It must take an Iterator object as its only parameter and return a bool.

		\code
		bool myCondition(const Iterator& it);
		\endcode

		findCondition returns an iterator to the first position at which the condition
		object returned true.
		It returns end() if the condition did not return true for any checked string position.

		searchStartPosIt is an iterator to the position at which the search should start
		(i.e. the first position at which the condition is checked).
	*/
	template<class Predicate>
	Iterator findCondition(Predicate condition, const Iterator& searchStartPosIt ) const
	{
		for( auto it=searchStartPosIt; it!=_endIt; ++it)
		{
			if( condition(it) )
				return it;
		}

		return _endIt;
	}



	/** Searches for the first position at which the specified condition is true.

		condition must be a callable object (like a function object or a lambda function).
		It must take an Iterator object as its only parameter and return a bool.

		\code
		bool myCondition(const Iterator& it);
		\endcode

		findCondition returns an iterator to the first position at which the condition
		object returned true.
		It returns end() if the condition did not return true for any checked string position.

		searchStartIndex is the character index where the search should start
		(i.e. the first position at which the condition is checked).
	*/
	template<class Predicate>
	size_t findCondition(Predicate condition, size_t searchStartIndex=0 ) const
	{
		size_t myLength = getLength();
		if(searchStartIndex==npos || searchStartIndex>=myLength)
			return noMatch;

		IteratorWithIndex it( _beginIt+searchStartIndex, searchStartIndex );

		while( it.getInner()!=_endIt )
		{
			if( condition(it.getInner() ) )
				return it.getIndex();

			++it;
		}

		return noMatch;
	}



	/** Searches backwards from the end of the string for the LAST position at which the specified condition is true.

		condition must be a callable object (like a function object or a lambda function).
		It must take an Iterator object as its only parameter and return a bool.

		\code
		bool myCondition(const Iterator& it);
		\endcode

		reverseFindCondition returns an iterator to the last position at which the condition
		object returned true.
		It returns end() if the condition did not return true for any checked string position.

		searchStartPosIt is an iterator to the position at which the search should start
		(i.e. the first position at which the condition is checked). The search moves backwards through
		the string from the start position.
	*/
	template<class Predicate>
	Iterator reverseFindCondition(Predicate condition, const Iterator& searchStartPosIt ) const
	{
		if(_beginIt == _endIt)
			return _endIt;

		Iterator it( searchStartPosIt );

		if(it==_endIt)
			--it;

		while(true)
		{
			if( condition(it) )
				return it;

			if(it==_beginIt)
				break;

			--it;
		}

		return _endIt;
	}



	/** Searches backwards from the end of the string for the LAST position at which the specified condition is true.

		condition must be a callable object (like a function object or a lambda function).
		It must take an Iterator object as its only parameter and return a bool.

		\code
		bool myCondition(const Iterator& it);
		\endcode

		reverseFindCondition returns an iterator to the last position at which the condition
		object returned true.
		It returns end() if the condition did not return true for any checked string position.

		searchStartIndex is the character index where the search should start
		(i.e. the first position at which the condition is checked).
	*/
	template<class Predicate>
	size_t reverseFindCondition(Predicate condition, size_t searchStartIndex=npos ) const
	{
		size_t myLength = getLength();
		if(myLength==0)
			return noMatch;

		if(searchStartIndex==npos || searchStartIndex>=myLength)
			searchStartIndex = myLength-1;

		IteratorWithIndex it( (searchStartIndex==myLength-1) ? (_endIt-1) : (_beginIt+searchStartIndex), searchStartIndex );

		while(true)
		{
			if( condition(it.getInner()) )
				return it.getIndex();

			if(it.getInner()==_beginIt)
				break;

			--it;
		}

		return noMatch;
	}



	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified with the two iterators charsBeginIt and charsEndIt.

		searchStartPosIt is an iterator to the position where the search should start.

		Returns an iterator to the first position in the string at which any one of the characters is found.
		Return end() if none of the characters is found.
	*/
	template <class InputIterator>
	Iterator findOneOf(const InputIterator& charsBeginIt, const InputIterator& charsEndIt, const Iterator& searchStartPosIt ) const
	{
		return findCondition(   [&charsBeginIt, &charsEndIt](const Iterator& it)
								{
									return (std::find(charsBeginIt, charsEndIt, *it)!=charsEndIt);
								},
								searchStartPosIt
							);
	}



	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified with the two iterators charsBeginIt and charsEndIt.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	template<class InputIterator>
	size_t findOneOf(const InputIterator& charsBeginIt, const InputIterator& charsEndIt, size_t searchStartIndex=0) const noexcept
	{
		return findCondition(   [&charsBeginIt, &charsEndIt](const Iterator& it)
								{
									return (std::find(charsBeginIt, charsEndIt, *it)!=charsEndIt);
								},
								searchStartIndex
							);
	}



	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	size_t findOneOf(const StringImpl& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOf( chars._beginIt, chars._endIt, searchStartIndex);
	}


	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as a sequence of encoded characters (\c encodedCharsBeginIt, \c encodedCharsEndIt).
		The encoding is indicated by the codec object \c codec. It can be any Unicode codec (see Utf8Codec, Utf16Codec, WideCodec, Utf32Codec).

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	template<class InputCodec, class InputIterator>
	size_t findOneOfEncoded(const InputCodec& codec, const InputIterator& encodedCharsBeginIt, const InputIterator& encodedCharsEndIt, size_t searchStartIndex=0) const noexcept
	{
		return findOneOf( typename InputCodec::template DecodingIterator<InputIterator>(encodedCharsBeginIt, encodedCharsBeginIt, encodedCharsEndIt),
						  typename InputCodec::template DecodingIterator<InputIterator>(encodedCharsEndIt, encodedCharsBeginIt, encodedCharsEndIt),
						  searchStartIndex );
	}


	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	size_t findOneOf(const std::string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOfEncoded( Utf8Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	size_t findOneOf(const std::wstring& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOfEncoded( WideCodec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	size_t findOneOf(const std::u16string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOfEncoded( Utf16Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	size_t findOneOf(const std::u32string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOfEncoded( Utf32Codec(), chars.begin(), chars.end(), searchStartIndex);
	}



	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as the C-style UTF-8 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in encoded bytes.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	size_t findOneOf(const char* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findOneOfEncoded( Utf8Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as the C-style wchar_t string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in wchar_t elements.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	size_t findOneOf(const wchar_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findOneOfEncoded( WideCodec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as the C-style UTF-16 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in char16_t elements.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/
	size_t findOneOf(const char16_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findOneOfEncoded( Utf16Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}

	/** Searches of the first occurrence of any character in a set of characters.

		The set of characters is specified as the C-style UTF-32 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in char32_t elements.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index to the first position in the string at which any one of the characters is found.
		Returns String::noMatch (String::npos) if none of the characters is found.
	*/

	size_t findOneOf(const char32_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findOneOfEncoded( Utf32Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}




	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const StringImpl& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOf( chars, searchStartIndex );
	}


	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const std::string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOf( chars, searchStartIndex );
	}


	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const std::wstring& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOf( chars, searchStartIndex );
	}


	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const std::u16string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOf( chars, searchStartIndex );
	}


	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const std::u32string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findOneOf( chars, searchStartIndex );
	}


	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const char* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const wchar_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const char16_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as findOneOf(). Included for compatibility with std::string.*/
	size_t find_first_of(const char32_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as find(). Included for compatibility with std::string.*/
	size_t find_first_of(char32_t toFind, size_t searchStartIndex=0) const noexcept
	{
		return find(toFind, searchStartIndex);
	}



	/** Searches of the first character in the string that is NOT one of the specified character set.

		The set of characters is specified with the two iterators charsBeginIt and charsEndIt.

		searchStartPosIt is an iterator to the position where the search should start.

		Returns an iterator to the first character in the string that is not in the specified set.
		Return end() if no such character is found.
	*/
	template <class InputIterator>
	Iterator findNotOneOf(const InputIterator& charsBeginIt, const InputIterator& charsEndIt, const Iterator& searchStartPosIt ) const
	{
		return findCondition(   [&charsBeginIt, &charsEndIt](const Iterator& it)
								{
									return (std::find(charsBeginIt, charsEndIt, *it)==charsEndIt);
								},
								searchStartPosIt
							);
	}



	/** Searches of the first character in the string that is NOT one of the specified character set.

		The set of characters is specified with the two iterators charsBeginIt and charsEndIt.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	template<class InputIterator>
	size_t findNotOneOf(const InputIterator& charsBeginIt, const InputIterator& charsEndIt, size_t searchStartIndex=0) const noexcept
	{
		return findCondition(   [&charsBeginIt, &charsEndIt](const Iterator& it)
								{
									return (std::find(charsBeginIt, charsEndIt, *it)==charsEndIt);
								},
								searchStartIndex
							);
	}



	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified with the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const StringImpl& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOf( chars._beginIt, chars._endIt, searchStartIndex);
	}


	/** Searches of the first character in the string that is NOT one of the specified character set.

		The set of characters is specified as a sequence of encoded characters (\c encodedCharsBeginIt, \c encodedCharsEndIt).
		The encoding is indicated by the codec object \c codec. It can be any Unicode codec (see Utf8Codec, Utf16Codec, WideCodec, Utf32Codec).

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	template<class InputCodec, class InputIterator>
	size_t findNotOneOfEncoded(const InputCodec& codec, const InputIterator& encodedCharsBeginIt, const InputIterator& encodedCharsEndIt, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOf(typename InputCodec::template DecodingIterator<InputIterator>(encodedCharsBeginIt, encodedCharsBeginIt, encodedCharsEndIt),
							typename InputCodec::template DecodingIterator<InputIterator>(encodedCharsEndIt, encodedCharsBeginIt, encodedCharsEndIt),
							searchStartIndex );
	}


	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified with the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const std::string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOfEncoded( Utf8Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified with the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const std::wstring& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOfEncoded( WideCodec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified with the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const std::u16string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOfEncoded( Utf16Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified with the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const std::u32string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOfEncoded( Utf32Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified as the C-style UTF-8 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in encoded bytes.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const char* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findNotOneOfEncoded( Utf8Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified as the C-style wchar_t string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in wchar_t elements.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const wchar_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findNotOneOfEncoded( WideCodec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified as the C-style UTF-16 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in char16_t elements.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const char16_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findNotOneOfEncoded( Utf16Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches of the first character in the string that is NOT in the specified character set.

		The set of characters is specified as the C-style UTF-32 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in char32_t elements.

		searchStartIndex indicates the index of the position where the search should start.

		Returns the index of the first character in the string that is not in the specified set.
		Returns String::noMatch (String::npos) if no such character is found.
	*/
	size_t findNotOneOf(const char32_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findNotOneOfEncoded( Utf32Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}




	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const StringImpl& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex );
	}

	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const std::string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex );
	}

	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const std::wstring& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex );
	}

	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const std::u16string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex );
	}

	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const std::u32string& chars, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex );
	}

	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const char* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex, charsLength );
	}

	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const wchar_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex, charsLength );
	}

	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const char16_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex, charsLength );
	}

	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(const char32_t* chars, size_t searchStartIndex=0, size_t charsLength=toEnd) const noexcept
	{
		return findNotOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as findNotOneOf(). Included for compatibility with std::string.*/
	size_t find_first_not_of(char32_t toFind, size_t searchStartIndex=0) const noexcept
	{
		return findNotOneOf(&toFind, searchStartIndex, 1);
	}




	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified with the two iterators charsBeginIt and charsEndIt.

		searchStartPosIt is an iterator to the position where the search should start. I.e. the first position
		to be considered for a possible match. If searchStartPosIt is end() then the entire string is searched.

		Returns an iterator to the last position in the string at which any one of the characters is found.
		Return end() if none of the characters is found.
	*/
	template <class InputIterator>
	Iterator reverseFindOneOf(const InputIterator& charsBeginIt, const InputIterator& charsEndIt, const Iterator& searchStartPosIt ) const
	{
		return reverseFindCondition(   [&charsBeginIt, &charsEndIt](const Iterator& it)
								{
									return (std::find(charsBeginIt, charsEndIt, *it)!=charsEndIt);
								},
								searchStartPosIt
							);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified with the two iterators charsBeginIt and charsEndIt.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	template<class InputIterator>
	size_t reverseFindOneOf(const InputIterator& charsBeginIt, const InputIterator& charsEndIt, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindCondition(   [&charsBeginIt, &charsEndIt](const Iterator& it)
								{
									return (std::find(charsBeginIt, charsEndIt, *it)!=charsEndIt);
								},
								searchStartIndex
							);
	}

	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const StringImpl& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOf( chars._beginIt, chars._endIt, searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified as a sequence of encoded characters (\c encodedCharsBeginIt, \c encodedCharsEndIt).
		The encoding is indicated by the codec object \c codec. It can be any Unicode codec (see Utf8Codec, Utf16Codec, WideCodec, Utf32Codec).

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	template<class InputCodec, class InputIterator>
	size_t reverseFindOneOfEncoded(const InputCodec& codec, const InputIterator& encodedCharsBeginIt, const InputIterator& encodedCharsEndIt, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOf(typename InputCodec::template DecodingIterator<InputIterator>(encodedCharsBeginIt, encodedCharsBeginIt, encodedCharsEndIt),
								typename InputCodec::template DecodingIterator<InputIterator>(encodedCharsEndIt, encodedCharsBeginIt, encodedCharsEndIt),
								searchStartIndex );
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const std::string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOfEncoded( Utf8Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const std::wstring& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOfEncoded( WideCodec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const std::u16string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOfEncoded( Utf16Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const std::u32string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOfEncoded( Utf32Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified as the C-style UTF-8 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in encoded bytes.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const char* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindOneOfEncoded( Utf8Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified as the C-style wchar_t string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in wchar_t elements.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const wchar_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindOneOfEncoded( WideCodec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified as the C-style UTF-16 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in char16_t elements.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const char16_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindOneOfEncoded( Utf16Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST occurrence of any character in a set of characters.

		The set of characters is specified as the C-style UTF-32 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in char32_t elements.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindOneOf(const char32_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindOneOfEncoded( Utf32Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}




	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const StringImpl& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const std::string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const std::wstring& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const std::u16string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const std::u32string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const char* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const wchar_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const char16_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as reverseFindOneOf(). Included for compatibility with std::string.*/
	size_t find_last_of(const char32_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as reverseFind(). Included for compatibility with std::string.*/
	size_t find_last_of(char32_t toFind, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFind(toFind, searchStartIndex);
	}



	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified with the two iterators charsBeginIt and charsEndIt.

		searchStartPosIt is an iterator to the position where the search should start. I.e. the first position
		to be considered for a possible match. If searchStartPosIt is end() then the entire string is searched.

		Returns an iterator to the last position in the string at which any one of the characters is found.
		Return end() if none of the characters is found.
	*/
	template <class InputIterator>
	Iterator reverseFindNotOneOf(const InputIterator& charsBeginIt, const InputIterator& charsEndIt, const Iterator& searchStartPosIt ) const
	{
		return reverseFindCondition(   [&charsBeginIt, &charsEndIt](const Iterator& it)
								{
									return (std::find(charsBeginIt, charsEndIt, *it)==charsEndIt);
								},
								searchStartPosIt
							);
	}



	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified with the two iterators charsBeginIt and charsEndIt.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	template<class InputIterator>
	size_t reverseFindNotOneOf(const InputIterator& charsBeginIt, const InputIterator& charsEndIt, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindCondition(   [&charsBeginIt, &charsEndIt](const Iterator& it)
								{
									return (std::find(charsBeginIt, charsEndIt, *it)==charsEndIt);
								},
								searchStartIndex
							);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const StringImpl& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOf( chars._beginIt, chars._endIt, searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified as a sequence of encoded characters (\c encodedCharsBeginIt, \c encodedCharsEndIt).
		The encoding is indicated by the codec object \c codec. It can be any Unicode codec (see Utf8Codec, Utf16Codec, WideCodec, Utf32Codec).

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	template<class InputCodec, class InputIterator>
	size_t reverseFindNotOneOfEncoded(const InputCodec& codec, const InputIterator& encodedCharsBeginIt, const InputIterator& encodedCharsEndIt, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOf( typename InputCodec::template DecodingIterator<InputIterator>(encodedCharsBeginIt, encodedCharsBeginIt, encodedCharsEndIt),
									typename InputCodec::template DecodingIterator<InputIterator>(encodedCharsEndIt, encodedCharsBeginIt, encodedCharsEndIt),
									searchStartIndex );
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const std::string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOfEncoded( Utf8Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const std::wstring& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOfEncoded( WideCodec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const std::u16string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOfEncoded( Utf16Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified by the string object \c chars. The order of the characters in
		\c chars does not matter.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const std::u32string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOfEncoded( Utf32Codec(), chars.begin(), chars.end(), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified as the C-style UTF-8 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in encoded bytes.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const char* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindNotOneOfEncoded( Utf8Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified as the C-style wchar_t string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in wchar_t elements.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const wchar_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindNotOneOfEncoded( WideCodec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified as the C-style UTF-16 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in char16_t elements.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const char16_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindNotOneOfEncoded( Utf16Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}


	/** Searches backwards from the end of the string for the LAST character in the string that is NOT in the specified character set.

		The set of characters is specified as the C-style UTF-32 encoded string
		\c chars. The order of the characters in \c chars does not matter.
		If charsLength is String::toEnd then chars must be zero terminated. Otherwise charsLength
		indicates the length of chars, in char32_t elements.

		searchStartIndex is the character index of the search start position. I.e. the index of the first character
		to be considered for a possible match. If searchStartIndex is String::npos then the entire string is searched.

		Returns the index of the last position in the string at which any one of the characters is found.
		Returns String::noMatch if none of the characters is found.
	*/
	size_t reverseFindNotOneOf(const char32_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindNotOneOfEncoded( Utf32Codec(), chars, getStringEndPtr(chars, charsLength), searchStartIndex);
	}





	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const StringImpl& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const std::string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const std::wstring& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const std::u16string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const std::u32string& chars, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex );
	}


	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const char* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const wchar_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const char16_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex, charsLength );
	}


	/** Same as reverseFindNotOneOf() */
	size_t find_last_not_of(const char32_t* chars, size_t searchStartIndex=npos, size_t charsLength=toEnd) const noexcept
	{
		return reverseFindNotOneOf( chars, searchStartIndex, charsLength );
	}



	/** Searches for the last character in the string that is NOT the specified blackListChar. */
	size_t find_last_not_of(char32_t blackListChar, size_t searchStartIndex=npos) const noexcept
	{
		return reverseFindCondition(	[&blackListChar](const Iterator& it)
										{
											return (*it!=blackListChar);
										},
										searchStartIndex);
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& operator=(const StringImpl& other)
	{
		return assign(other);
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& operator=(const std::string& o)
	{
		return assign(o);
	}


	/** Assigns the value of a zero terminated C-style UTF-8 string to this string. */
	StringImpl& operator=(const char* o)
	{
		return assign(o);
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& operator=(const std::wstring& o)
	{
		return assign(o);
	}


	/** Assigns the value of a zero terminated C-style wide char string to this string. */
	StringImpl& operator=(const wchar_t* o)
	{
		return assign(o);
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& operator=(const std::u16string& o)
	{
		return assign(o);
	}


	/** Assigns the value of a zero terminated C-style UTF-16 string to this string. */
	StringImpl& operator=(const char16_t* o)
	{
		return assign(o);
	}


	/** Assigns the value of another string to this string. */
	StringImpl& operator=(const std::u32string& o)
	{
		return assign(o);
	}


	/** Assigns the value of a zero terminated C-style UTF-32 string to this string. */
	StringImpl& operator=(const char32_t* o)
	{
		return assign(o);
	}


	/** Sets the contents of this string to be a one-character string.
		*/
	StringImpl& operator=(char32_t chr)
	{
		return assign(1, chr);
	}



	/** Sets the contents of this string to be a sequence of character.

		initializerList is automatically created by the compiler when you call this method
		with an initializer list.

		Example:

		\code
		myString = {'a', 'b', 'c' };
		\endcode
		*/
	StringImpl& operator=(std::initializer_list<char32_t> initializerList)
	{
		return assign(initializerList);
	}


	/** Move-operator. "Steals" the contents from another string.
		Sets the contents of this string to the contents of
		\c moveSource. Afterwards moveSource will contain only the empty string.
		*/
	StringImpl& operator=(StringImpl&& moveSource) noexcept
	{
		return assign(std::move(moveSource) );
	}





	/** Appends the specified string to the end of this string.
	*/
	StringImpl& operator+=(const StringImpl& other)
	{
		return append(other);
	}



	/** Appends the specified string to this string.*/
	StringImpl& operator+=(const std::string& other)
	{
		return append(other);
	}


	/** Appends the specified string to this string.
	*/
	StringImpl& operator+=(const char* other)
	{
		return append(other);
	}


	/** Appends the specified string to this string.*/
	StringImpl& operator+=(const std::u16string& o)
	{
		return append(o);
	}


	/** Appends the specified string to this string.
	*/
	StringImpl& operator+=(const char16_t* o)
	{
		return append(o);
	}


	/** Appends the specified string to this string.
	*/
	StringImpl& operator+=(const std::u32string& o)
	{
		return append(o);
	}


	/** Appends the specified string to this string.
	*/
	StringImpl& operator+=(const char32_t* o)
	{
		return append(o);
	}


	/** Appends the specified string to this string.
	*/
	StringImpl& operator+=(const std::wstring& o)
	{
		return append(o);
	}


	/** Appends the specified string to this string.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string in wchar_t elements.
	*/
	StringImpl& operator+=(const wchar_t* o)
	{
		return append(o);
	}


	/** Appends \c numChars occurrences of \c chr to this string.
	*/
	StringImpl& operator+=(char32_t chr)
	{
		return append(chr);
	}


	/** Appends a sequence of characters to this string.

		initializerList is automatically created by the compiler when you call this method
		with an initializer list.

		Example:

		\code
		myString.append( {'a', 'b', 'c' } );
		\endcode
	*/
	StringImpl& operator+=(std::initializer_list<char32_t> initializerList)
	{
		return append(initializerList);
	}




	/** Searches for all occurrences of the character \c toFind and replaces them with
		the character \c replaceWith.

		Returns the number of occurrences that were replaced.
		*/
	int findReplace(char32_t toFind, char32_t replaceWith)
	{
		return findReplace(&toFind, (&toFind)+1, &replaceWith, (&replaceWith)+1 );
	}


	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.
		*/
	int findReplace(const StringImpl& toFind, const StringImpl& replaceWith)
	{
		return findReplace(toFind.begin(), toFind.end(), replaceWith.begin(), replaceWith.end() );
	}



	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.
		*/
	int findReplace(const std::string& toFind, const std::string& replaceWith)
	{
		return findReplaceEncoded(Utf8Codec(), toFind.begin(), toFind.end(), Utf8Codec(), replaceWith.begin(), replaceWith.end() );
	}


	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.
		*/
	int findReplace(const std::wstring& toFind, const std::wstring& replaceWith)
	{
		return findReplaceEncoded(WideCodec(), toFind.begin(), toFind.end(), WideCodec(), replaceWith.begin(), replaceWith.end() );
	}


	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.
		*/
	int findReplace(const std::u16string& toFind, const std::u16string& replaceWith)
	{
		return findReplaceEncoded(Utf16Codec(), toFind.begin(), toFind.end(), Utf16Codec(), replaceWith.begin(), replaceWith.end() );
	}


	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.
		*/
	int findReplace(const std::u32string& toFind, const std::u32string& replaceWith)
	{
		return findReplace(toFind.begin(), toFind.end(), replaceWith.begin(), replaceWith.end() );
	}


	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.
		*/
	int findReplace(const char* toFind, const char* replaceWith)
	{
		return findReplaceEncoded(Utf8Codec(), toFind, getStringEndPtr(toFind), Utf8Codec(), replaceWith, getStringEndPtr(replaceWith) );
	}


	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.
		*/
	int findReplace(const wchar_t* toFind, const wchar_t* replaceWith)
	{
		return findReplaceEncoded(WideCodec(), toFind, getStringEndPtr(toFind), WideCodec(), replaceWith, getStringEndPtr(replaceWith) );
	}


	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.
		*/
	int findReplace(const char16_t* toFind, const char16_t* replaceWith)
	{
		return findReplaceEncoded(Utf16Codec(), toFind, getStringEndPtr(toFind), Utf16Codec(), replaceWith, getStringEndPtr(replaceWith) );
	}


	/** Searches for all occurrences of the String \c toFind and replaces them with
		\c replaceWith.

		Returns the number of occurrences that were replaced.

		If \c toFind is empty then the function does nothing and returns 0.

		*/
	int findReplace(const char32_t* toFind, const char32_t* replaceWith)
	{
		return findReplace(toFind, getStringEndPtr(toFind), replaceWith, getStringEndPtr(replaceWith) );
	}




	/** Searches for all occurrences of the String defined by the iterators \c toFindBegin and \c toFindEnd and replaces them with
		the string defined by \c replaceWithBegin and \c replaceWithEnd.

		Returns the number of occurrences that were replaced.

		If \c toFindBegin equals toFindEnd (i.e. the toFind string is empty) then the function does nothing and returns 0.
		*/
	template<class ToFindIterator, class ReplaceWithIterator>
	int findReplace(const ToFindIterator& toFindBegin, const ToFindIterator& toFindEnd, const ReplaceWithIterator& replaceWithBegin, const ReplaceWithIterator& replaceWithEnd)
	{
		int	matchCount=0;

		if(toFindBegin!=toFindEnd)
		{
			Iterator pos = _beginIt;
			while(pos!=_endIt)
			{
				Iterator matchEnd;
				Iterator matchBegin = find(toFindBegin, toFindEnd, pos, &matchEnd);
				if(matchBegin==_endIt)
				{
					// no more matches.
					break;
				}

				matchCount++;

				size_t encodedLengthAfterMatch = _pData->getEncodedString().length() - (matchEnd.getInner()-_beginIt.getInner());

				replace(matchBegin, matchEnd, replaceWithBegin, replaceWithEnd);

				size_t replacedEndOffset = _pData->getEncodedString().length() - encodedLengthAfterMatch;

				pos = Iterator(_beginIt.getInner() + replacedEndOffset, _beginIt.getInner(), _endIt.getInner());
			}
		}

		return matchCount;
	}



	/** Searches for all occurrences of the String defined by the iterators \c toFindEncodedBegin and \c toFindEncodedEnd and replaces them with
		the string defined by \c replaceWithEncodedBegin and \c replaceWithEncodedEnd.

		The toFind and replaceWith iterators return encoded data, which is encoded using the codec indicated by
		\c toFindCodec and \c replaceWithCodec (see, for example, Utf8Codec, Utf16Codec, WideCodec, etc.).
		The two codecs can be different.

		Returns the number of occurrences that were replaced.

		If \c toFindEncodedBegin equals toFindEncodedBegin (i.e. the toFind string is empty) then the function does nothing and returns 0.
		*/
	template<class ToFindCodec, class ToFindIterator, class ReplaceWithCodec, class ReplaceWithIterator>
	int findReplaceEncoded(	const ToFindCodec& toFindCodec,
							const ToFindIterator& toFindEncodedBegin,
							const ToFindIterator& toFindEncodedEnd,
							const ReplaceWithCodec& replaceWithCodec,
							const ReplaceWithIterator& replaceWithEncodedBegin,
							const ReplaceWithIterator& replaceWithEncodedEnd)
	{
		return findReplace( typename ToFindCodec::template DecodingIterator<ToFindIterator>( toFindEncodedBegin, toFindEncodedBegin, toFindEncodedEnd),
							typename ToFindCodec::template DecodingIterator<ToFindIterator>( toFindEncodedEnd, toFindEncodedBegin, toFindEncodedEnd),
							typename ReplaceWithCodec::template DecodingIterator<ReplaceWithIterator>( replaceWithEncodedBegin, replaceWithEncodedBegin, replaceWithEncodedEnd),
							typename ReplaceWithCodec::template DecodingIterator<ReplaceWithIterator>( replaceWithEncodedEnd, replaceWithEncodedBegin, replaceWithEncodedEnd) );
	}







	/** Can be used to efficiently split the string into parts ("tokens") that are separated by any one character from the given
		set of separator characters.

		splitOffToken searches for any one of the given separators. When a separator is found, the string is changed
		to only contain the part after the separator and the part before the separator is returned.

		If returnEmptyTokens is true then two separators following each other will cause an empty token to be returned.
		If it is false then such empty tokens will be skipped and the first non-empty token is returned.
		By default empty tokens are returned.

		If pSeparator is not null and a token is found then *pSeparator will be set to the separator that was encountered at the end
		of the token. If the token ends at the end of the string, or if no token is found then *pSeparator is set to 0.

		Also see splitOffWord().

		Performance note:

		Some programmers might be afraid that generating a bunch of sub strings during iteration through the tokens might
		be slow. However, the string is implemented in a way so that sub strings are very light weight and do not need
		to copy the string data. So this kind of iteration is actually very fast and efficient.

		Example:

		\code

		// Split a string with comma separated values.

		String   remaining = ...;
		while( !remaining.isEmpty() )
		{
			String token = remaining.splitOffToken(",");
			...
		}

		\endcode
		*/
	StringImpl splitOffToken(const StringImpl& separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffToken(separatorChars.begin(), separatorChars.end(), returnEmptyTokens, pSeparator );
	}


	StringImpl splitOffToken(const std::string& separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffTokenEncoded(Utf8Codec(), separatorChars.begin(), separatorChars.end(), returnEmptyTokens, pSeparator );
	}


	StringImpl splitOffToken(const std::wstring& separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffTokenEncoded(WideCodec(), separatorChars.begin(), separatorChars.end(), returnEmptyTokens, pSeparator );
	}

	StringImpl splitOffToken(const std::u16string& separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffTokenEncoded(Utf16Codec(), separatorChars.begin(), separatorChars.end(), returnEmptyTokens, pSeparator );
	}

	StringImpl splitOffToken(const std::u32string& separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffToken(separatorChars.begin(), separatorChars.end(), returnEmptyTokens, pSeparator );
	}



	StringImpl splitOffToken(const char* separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffTokenEncoded(Utf8Codec(), separatorChars, getStringEndPtr(separatorChars), returnEmptyTokens, pSeparator );
	}


	StringImpl splitOffToken(const wchar_t* separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffTokenEncoded(WideCodec(), separatorChars, getStringEndPtr(separatorChars), returnEmptyTokens, pSeparator );
	}

	StringImpl splitOffToken(const char16_t* separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffTokenEncoded(Utf16Codec(), separatorChars, getStringEndPtr(separatorChars), returnEmptyTokens, pSeparator );
	}

	StringImpl splitOffToken(const char32_t* separatorChars, bool returnEmptyTokens=true, char32_t* pSeparator=nullptr)
	{
		return splitOffToken(separatorChars, getStringEndPtr(separatorChars), returnEmptyTokens, pSeparator );
	}


	template<class InputCodec, class InputIterator>
	StringImpl splitOffTokenEncoded(const InputCodec& codec,
									const InputIterator& separatorCharsBeginIt,
									const InputIterator& separatorCharsEndIt,
									bool returnEmptyTokens=true,
									char32_t* pSeparator=nullptr)
	{
		return splitOffToken( typename InputCodec::template DecodingIterator<InputIterator>(separatorCharsBeginIt, separatorCharsBeginIt, separatorCharsEndIt),
							  typename InputCodec::template DecodingIterator<InputIterator>(separatorCharsEndIt, separatorCharsBeginIt, separatorCharsEndIt),
							  returnEmptyTokens,
							  pSeparator );
	}


	template<class InputIterator>
	StringImpl splitOffToken(	const InputIterator& separatorCharsBeginIt,
								const InputIterator& separatorCharsEndIt,
								bool returnEmptyTokens=true,
								char32_t* pSeparator=nullptr)
	{
		while(_beginIt != _endIt)
		{
			Iterator tokenEndIt = findOneOf(separatorCharsBeginIt, separatorCharsEndIt, _beginIt);
			if(tokenEndIt==_beginIt && !returnEmptyTokens)
			{
				// empty token. Skip over it.
				++_beginIt;

				_lengthIfKnown = -1;
				_pDataInDifferentEncoding = nullptr;
			}
			else
			{
				StringImpl result = subString(_beginIt, tokenEndIt);

				_beginIt = tokenEndIt;
				if(_beginIt != _endIt)
				{
					if(pSeparator!=nullptr)
						*pSeparator = *_beginIt;

					++_beginIt;
				}
				else
				{
					if(pSeparator!=nullptr)
						*pSeparator = U'\0';
				}

				_lengthIfKnown = -1;
				_pDataInDifferentEncoding = nullptr;

				return result;
			}
		}

		if(pSeparator!=nullptr)
			*pSeparator = U'\0';

		return StringImpl();
	}



	/** Can be used to efficiently split the string into words.

		splitOffWord determines for the first word in the string (the end of the word is indicated by a whitespace character).
		The string is changed to contain the part after the first wird and then the first word is returned.

		Returns an empty string when there are no more words. Afterwards the string is always empty.

		If multiple whitespace characters follow each other then they are skipped. I.e. the only time an empty string is
		returned is when there are no more words in the string.

		splitOffWord is equivalent to:
		\code
		splitOffToken( String::getWhitespaceChars(), false);
		\endcode

		Performance note:

		Some programmers might be afraid that generating a bunch of sub strings during iteration through the words might
		be slow. However, the string is implemented in a way so that sub strings are very light weight and do not need
		to copy the string data. So this kind of iteration is actually very fast and efficient.

		Example:

		\code

		String   remaining = ...;
		while(true)
		{
			String word = remaining.splitOffWord();
			if(word.isEmpty())
				break;

			...
		}

		\endcode
		*/
	StringImpl splitOffWord()
	{
		return splitOffToken( getWhitespaceChars(), false);
	}




	/** A special iterator for keeping track of the character index associated with its current position.

		Wraps a normal Iterator.
	*/
	class IteratorWithIndex : public std::iterator<	std::bidirectional_iterator_tag,
													char32_t,
													std::ptrdiff_t,
													char32_t*,
													// this is a bit of a hack. We define Reference to be a value, not
													// an actual reference. That is necessary, because we return values
													// generated on the fly that are not actually stored by the underlying
													// container. While we could return a reference to a member of the iterator,
													// that would only remain valid while the iterator is alive. And parts of
													// the standard library (for example std::reverse_iterator) will create
													// temporary local iterators and return their value references, which would
													// cause a crash.
													// By defining reference as a value, we ensure that the standard library functions
													// return valid objects.
													char32_t
													>
	{
	public:
		/** @param innerIt the iterator to wrap
			@param index the index that corresponds to the current position of \c innerIt.*/
		IteratorWithIndex(const Iterator& innerIt, size_t index)
		{
			_innerIt = innerIt;
			_index = index;
		}

		IteratorWithIndex& operator++()
		{
			++_innerIt;
			++_index;

			return *this;
		}

		IteratorWithIndex operator++(int)
		{
			IteratorWithIndex oldVal = *this;
			operator++();

			return oldVal;
		}

		IteratorWithIndex& operator--()
		{
			--_innerIt;
			--_index;

			return *this;
		}

		IteratorWithIndex operator--(int)
		{
			IteratorWithIndex oldVal = *this;
			operator--();

			return oldVal;
		}

		IteratorWithIndex& operator+=(int val)
		{
			_innerIt += val;
			_index += val;

			return *this;
		}

		IteratorWithIndex& operator-=(int val)
		{
			_innerIt -= val;
			_index -= val;

			return *this;
		}


		IteratorWithIndex operator+(int val) const
		{
			IteratorWithIndex it = *this;
			it+=val;

			return it;
		}

		IteratorWithIndex operator-(int val) const
		{
			IteratorWithIndex it = *this;
			it-=val;

			return it;
		}

		char32_t operator*()
		{
			return *_innerIt;
		}

		bool operator==(const IteratorWithIndex& o) const
		{
			return (_innerIt==o._innerIt);
		}

		bool operator!=(const IteratorWithIndex& o) const
		{
			return !operator==(o);
		}


		/** Returns an iterator to the inner encoded string that the decoding iterator is working on.
			The inner iterator points to the first encoded element of the character, that the decoding
			iterator is currently pointing to.*/
		const Iterator& getInner() const
		{
			return _innerIt;
		}


		/** Returns the character index that the iterator it positioned at.*/
		size_t getIndex() const
		{
			return _index;
		}

	protected:
		Iterator _innerIt;
		size_t   _index;
	};

protected:


	template<class T>
    const typename T::EncodedString& getEncoded(T* dummy) const
	{
		T* p = dynamic_cast<T*>(_pDataInDifferentEncoding.getPtr());
		if (p == nullptr)
		{
			P<T> pNewData = newObj<T>(_beginIt, _endIt);
			_pDataInDifferentEncoding = pNewData;

			p = pNewData;
		}

		return p->getEncodedString();
	}

	const typename MainDataType::EncodedString& getEncoded(MainDataType* dummy) const
	{
		if(_endIt!=_pData->end() || _beginIt!=_pData->begin())
		{
			// we are a sub-slice of another string. Copy it now, so that we can return
			// the object.
			_pData = newObj<MainDataType>(_beginIt, _endIt);
			_beginIt = _pData->begin();
			_endIt = _pData->end();
		}

		return _pData->getEncodedString();
	}

	void setEnd( const Iterator& newEnd, int newLengthIfKnown)
	{
		_endIt = newEnd;
		_lengthIfKnown = newLengthIfKnown;

		// we must throw away any data in different encoding we might have.
		_pDataInDifferentEncoding = nullptr;
	}


	/** Prepares for the string to be modified.
		If we are sharing the string data with anyone then we make a copy and switch to it.
		If we are working on a substring then we throw away the unneeded parts.

		Afterwards the encoded string contains only the data that is valid for us.

		If the string data is replaced then a pointer to the old data is stored
		in pOldData.
		*/
	void beginModification()
	{
		if(_pData->getRefCount()!=1)
		{
			// we are sharing the data => need to copy.

			_pData = newObj<MainDataType>( typename MainDataType::Codec(), _beginIt.getInner(), _endIt.getInner() );

			_beginIt = _pData->begin();
			_endIt = _pData->end();
		}
		else
		{
			typename MainDataType::EncodedString* pStd = &_pData->getEncodedString();

			if(_beginIt.getInner()!=pStd->cbegin()
				|| _endIt.getInner()!=pStd->cend() )
			{
				// we are working on a substring of the data. Throw away the other parts.
				// Note that we want to avoid re-allocation, so we want to do this in place.
				// First we cut off what we do not need from the end, then from the start.
				// Unfortunately, cutting off from the end will invalidate our begin iterator,
				// so we need to save its value as an index.

				int startIndex = _beginIt.getInner() - pStd->cbegin();

				if(_endIt.getInner()!=pStd->cend())
				{
                    // there is a bug in g++ 4.8. Erase only takes normal iterators, instead of const_iterators.
                    // work around it.
                    #if defined(__GNUC__) && __GNUC__ < 5

                    pStd->erase( pStd->begin() + (_endIt.getInner()-pStd->cbegin()), pStd->end() );

                    #else

					pStd->erase( _endIt.getInner(), pStd->cend() );

					#endif
                }

				if(startIndex>0)
					pStd->erase( pStd->begin(), pStd->begin()+startIndex );

				_beginIt = _pData->begin();
				_endIt = _pData->end();
			}
		}
	}


	/** Finishes a modification. Updates the begin end end iterators to the beginning
		and end of the new encoded string.*/
	void endModification()
	{
		// when we started modifying we ensured that we are not working on a substring.
		// Now we can update our start and end iterators to the new start and end of the data.

		_beginIt = _pData->begin();
		_endIt = _pData->end();

		_lengthIfKnown = -1;
		_pDataInDifferentEncoding = nullptr;
	}


	struct Modify
	{
		Modify(StringImpl* pParentArg)
		{
			pParent = pParentArg;

			pParent->beginModification();

			pStd = &pParent->_pData->getEncodedString();
		}

		~Modify()
		{
			pParent->endModification();
		}

		typename MainDataType::EncodedString*   pStd;
		StringImpl*								pParent;
	};
	friend struct Modify;


	mutable P<MainDataType>	_pData;
	mutable Iterator		_beginIt;
	mutable Iterator		_endIt;

	mutable P<Base>			_pDataInDifferentEncoding;

	mutable int				_lengthIfKnown;
};

}


#endif

