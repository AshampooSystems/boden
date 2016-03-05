#ifndef BDN_StringImpl_H_
#define BDN_StringImpl_H_

#include <bdn/StringData.h>

#include <iterator>
#include <list>

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


/** Provides an implementation of a String class with the internal encoding being
	controlled by the template parameter MainDataType. MainDataType must be a StringData object
	(or one that provides the same interface)

	StringImpl provides the implementation for the String class (which is a typedef for StringImpl<NativeDataType>).
	See the String documentation for an explanation of how StringImpl objects work.
*/
template<class MainDataType>
class StringImpl : public Base
{
public:


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



	/** Included for compatibility with std::string only.*/
	static const size_t npos = -1;

	
	StringImpl()
		: StringImpl( MainDataType::getEmptyData() )
	{
		_lengthIfKnown = 0;
	}

	StringImpl(const StringImpl& s)
	{
		_pData = s._pData;
		_pDataInDifferentEncoding = s._pDataInDifferentEncoding;

		_beginIt = s._beginIt;
		_endIt = s._endIt;

		_lengthIfKnown = s._lengthIfKnown;
	}

	StringImpl(const StringImpl&& s) noexcept
	{
		_lengthIfKnown = -1;

		assign( std::move(s) );
	}


	StringImpl(const StringImpl& s, const Iterator& beginIt, const Iterator& endIt )
	{
		_pData = s._pData;

		// cannot copy _pDataInDifferentEncoding because we only want a substring of it.

		_beginIt = beginIt;
		_endIt = endIt;

		_lengthIfKnown = -1;
	}


	/** Initializes the object from a C-style UTF-8 encoded string.

		To initialize with data in the locale-dependent multibyte encoding
		see #fromLocale.
	*/
	StringImpl(const char* s, int lengthElements=-1)
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


	/** Static construction method. Initializes the String object from a C-style
		string in the locale-dependent multibyte encoding.*/
	static StringImpl fromLocale(const char* s, int lengthElements=-1)
	{
		return StringImpl( MainDataType::fromLocale(s, lengthElements) );
	}

	/** Static construction method. Initializes the String object from a std::string
	in the locale-dependent multibyte encoding.*/
	static StringImpl fromLocale(const std::string& s)
	{
		return StringImpl( MainDataType::fromLocale(s) );
	}


	StringImpl(const wchar_t* s, int lengthElements=-1)
		: StringImpl(newObj<MainDataType>(s, lengthElements))
	{
	}

	StringImpl(const std::wstring& s)
		: StringImpl(newObj<MainDataType>(s))
	{
	}

	StringImpl(const char16_t* s, int lengthElements=-1)
		: StringImpl(newObj<MainDataType>(s, lengthElements))
	{
	}

	StringImpl(const std::u16string& s)
		: StringImpl(newObj<MainDataType>(s))
	{
	}

	StringImpl(const char32_t* s, int lengthElements=-1)
		: StringImpl( newObj<MainDataType>(s, lengthElements) )
	{
	}

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


	/** Initializes the object with the data between two iterators whose data is encoded
		according to the specified InputCodec codec type.

		The data is first automatically decoded and then re-encoded to this String's data codec.
		The only exception is if InputCodec is the same codec that this String uses. In that
		case the data will simply be copied as is.
	*/
	template<class InputCodec, class InputEncodedIterator>
	StringImpl(const InputCodec& codec, InputEncodedIterator inputEncodedBeginIt, InputEncodedIterator inputEncodedEndIt)
		: StringImpl( newObj<MainDataType>(codec, inputEncodedBeginIt, inputEncodedEndIt) )
	{
	}

	
	StringImpl(MainDataType* pData)
		: _pData(pData)
		, _beginIt( pData->begin() )
		, _endIt( pData->end() )
	{
		_lengthIfKnown = -1;
	}


	/** Returns true if the string is empty (i.e. if its length is 0).*/
	bool isEmpty() const
	{
		return (_beginIt == _endIt);
	}

	/** Same as isEmpty. This is included for compatibility with std::string.*/
	bool empty() const
	{
		return isEmpty();
	}


	/** Returns the number of characters in this string.*/
	int getLength() const
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

		return _lengthIfKnown;
	}


	/** Same as getLength. This is included for compatibility with std::string.*/
	size_t length() const
	{
		return getLength();
	}

	/** Same as getLength. This is included for compatibility with std::string.*/
	size_t size() const
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
			MainDataType::EncodedString* pStd = &_pData->getEncodedString();

			int excessCapacity = pStd->capacity()-pStd->length();
			if(excessCapacity<0)
				excessCapacity = 0;

			excessCapacity += pStd->end() - _endIt.getInner();
		
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
	Iterator begin() const
	{
		return _beginIt;
	}

	/** Retuns an iterator that points to the position just after the last character of the string.*/
	Iterator end() const
	{
		return _endIt;
	}


	/** Same as begin(). This is included for compatibility with std::string.*/
	Iterator cbegin() const
	{
		return begin();
	}

	/** Same as end(). This is included for compatibility with std::string.*/
	Iterator cend() const
	{
		return end();
	}



	/** Returns an iterator that iterates over the characters of the string in reverse order.

		The iterator starts at the last character of the string. Advancing it with ++ moves it
		to the previous character.

		Use this together with rend() to check for the end of the iteration.
		
		Example:

		\code
		s = "hello";
		for(auto it = s.rbegin(); it!=s.rend(); it++)
			print(*it);
		\endcode
		
		This will print out "olleh" (the reverse of "hello").

		*/
	ReverseIterator rbegin() const
	{
		return std::reverse_iterator<Iterator>( end() );
	}


	/** Returns an iterator that points to the end of a reverse iteration.
		See rbegin().*/
	ReverseIterator rend() const
	{
		return std::reverse_iterator<Iterator>( begin() );
	}


	/** Same as rbegin(). This is included for compatibility with std::string.*/
	ReverseIterator crbegin() const
	{
		return rbegin();
	}

	/** Same as rend(). This is included for compatibility with std::string.*/
	ReverseIterator crend() const
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

		charCount can be -1, in which case the rest of the string up to the end is returned.

		If startIndex is invalid (<0 or >length) then an OutOfRangeError (which is the same as
		std::out_of_range) is thrown.
		startIndex can equal the string length - in that case the resulting sub string is always empty.
	*/
	StringImpl subString(int startIndex, int charCount) const
	{
		int myCharCount = getLength();

		if (startIndex<0 || startIndex>myCharCount)
			throw OutOfRangeError("String::subString: Invalid start index: "+std::to_string(startIndex) );
		if (charCount<0 || startIndex+charCount>myCharCount)
			charCount = myCharCount-startIndex;

		Iterator startIt = _beginIt+startIndex;
		Iterator endIt = (charCount<0) ? _endIt : (startIt+charCount);

		return StringImpl(*this, startIt, endIt);
	}


	/** Similar to subString. This function is included for compatibility with std::string.
	
		The only difference to subString is that charCount must be String::npos (instead of
		the -1) if you want	the rest of the string up to the end.
	*/
	StringImpl substr(size_t startIndex, size_t charCount) const
	{
		return subString(startIndex, charCount==npos ? -1 : (int)charCount);
	}


	/** Returns a pointer to the string as a zero terminated c-style string in UTF-8 encoding.

		This operation might invalidate existing iterators.

		The pointer remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const char* asUtf8Ptr() const
	{
		return getEncoded<Utf8StringData>().c_str();
	}

	
	/** Returns a reference to the string as a std::string object in UTF-8 encoding.

		This operation might invalidate existing iterators. The returned object reference
		remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const std::string& asUtf8() const
	{
		return getEncoded<Utf8StringData>();
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

	
	/** Same as asUtf8Ptr(). This function is included for compatibility with std::string.*/
	const char* c_str() const
	{
		return asUtf8Ptr();
	}


	/** Same as asUtf8Ptr(). This function is included for compatibility with std::string.*/
	const char* data() const
	{
		return asUtf8Ptr();
	}


	/** Returns a pointer to the string as a zero terminated c-style string in "wide char" encoding
		(either UTF-16 or UTF-32, depending on the size of wchar_t).

		This operation might invalidate existing iterators.

		The pointer remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const wchar_t* asWidePtr() const
	{
		return getEncoded<WideStringData>().c_str();
	}
	
	
	/** Returns a reference to the string as a std::wstring object in in "wide char" encoding
		(either UTF-16 or UTF-32, depending on the size of wchar_t).

		This operation might invalidate existing iterators. The returned object reference
		remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const std::wstring& asWide() const
	{
		return getEncoded<WideStringData>();
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
		return getEncoded<Utf16StringData>().c_str();
	}


	/** Returns a reference to the string as a std::u16string object in UTF-16 encoding

		This operation might invalidate existing iterators. The returned object reference
		remains valid at least until one of the other asXYZ conversion functions is called
		or the entire string object is destroyed.
	*/
	const std::u16string& asUtf16() const
	{
		return getEncoded<Utf16StringData>();
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
		return getEncoded<Utf32StringData>().c_str();
	}


	/** Returns a reference to the string as a std::u16string object in UTF-32 encoding

	This operation might invalidate existing iterators. The returned object reference
	remains valid at least until one of the other asXYZ conversion functions is called
	or the entire string object is destroyed.
	*/
	const std::u32string& asUtf32() const
	{
		return getEncoded<Utf32StringData>();
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
		Iterator myIt = _beginIt;

		while(true)
		{
			if(myIt==_endIt)
				return (otherIt==otherEnd) ? 0 : -1;

			else if(otherIt==otherEnd)
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

		return 0;
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
	int compare(const char* o) const
	{
		const char* oEnd = getStringEndPtr(o);

		return compare( Utf8Codec::DecodingIterator<const char*>(o, o, oEnd),
						Utf8Codec::DecodingIterator<const char*>(oEnd, o, oEnd) );
	}

	/** See compare()
	*/
	int compare(const std::u16string& o) const
	{
		return compare( Utf16Codec<char16_t>::DecodingStringIterator(o.begin(), o.begin(), o.end()),
					    Utf16Codec<char16_t>::DecodingStringIterator(o.end(), o.begin(), o.end()) );
	}

	/** See compare()
	*/
	int compare(const char16_t* o) const
	{
		const char16_t* oEnd = getStringEndPtr(o);

		return compare( Utf16Codec<char16_t>::DecodingIterator<const char16_t*>(o, o, oEnd),
						Utf16Codec<char16_t>::DecodingIterator<const char16_t*>(oEnd, o, oEnd) );
	}

	/** See compare()
	*/
	int compare(const std::u32string& o) const
	{
		return compare( Utf32Codec<char32_t>::DecodingStringIterator(o.begin(), o.begin(), o.end()),
						Utf32Codec<char32_t>::DecodingStringIterator(o.end(), o.begin(), o.end()) );
	}

	/** See compare()
	*/
	int compare(const char32_t* o) const
	{
		const char32_t* oEnd = getStringEndPtr(o);

		return compare( o, oEnd );
	}

	/** See compare()
	*/
	int compare(const std::wstring& o) const
	{
		return compare( WideCodec::DecodingStringIterator(o.begin(), o.begin(), o.end()),
						WideCodec::DecodingStringIterator(o.end(), o.begin(), o.end()) );
	}

	/** See compare()
	*/
	int compare(const wchar_t* o) const
	{
		const wchar_t* oEnd = getStringEndPtr(o);

		return compare( WideCodec::DecodingIterator<const wchar_t*>(o, o, oEnd),
						WideCodec::DecodingIterator<const wchar_t*>(oEnd, o, oEnd) );
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
		if(index<0 || index>=len)
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
		if(index<0 || index>=len)
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
							MainDataType::Codec::EncodingIterator<InputIterator>(replaceWithBegin),
							MainDataType::Codec::EncodingIterator<InputIterator>(replaceWithEnd) );

		return *this;
	}


	/** Replaces a section of the string (defined by two iterators) with the data between two
		other iterators.
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	template<>
	StringImpl& replace<Iterator>(	const Iterator& rangeBegin,
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

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
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
		
		Iterator rangeEnd( (rangeLength==npos || rangeStartIndex+rangeLength>=myLength)
							? _endIt
							: (rangeStart+rangeLength) );

		return replace(rangeStart, rangeEnd, replaceWithBegin, replaceWithEnd);
	}


	/** Replaces a section of this string (defined by two iterators) with the contents of replaceWith.

		If replaceWithStartIndex is specified then only the part of \c replaceWith starting from that index is
		used. If replaceWithStartIndex is bigger than the length of \c replaceWith then an OutOfRangeError is thrown.
		
		If \c replaceWithLength is specified then at most this number of characters is used from \c replaceWith.
		If \c replaceWith is not long enough for \c replaceWithLength characters to be copied, or if \c replaceWithLength is String::npos,
		then only the part replaceWith up to its end is used.
	
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(const Iterator& rangeBegin,
						const Iterator& rangeEnd,
						const StringImpl& replaceWith,
						size_t replaceWithStartIndex = 0,
						size_t replaceWithLength = npos)
	{
		if(replaceWithStartIndex==0 && replaceWithLength==npos)
			return replace(rangeBegin, rangeEnd, replaceWith.begin(), replaceWith.end() );
		else
		{
			size_t actualReplaceWithLength = replaceWith.getLength();
			if(replaceWithStartIndex>actualReplaceWithLength)
				throw OutOfRangeError("Invalid start index passed to String::replace");

			Iterator replaceWithStart = replaceWith.begin()+replaceWithStartIndex;

			Iterator replaceWithEnd( (replaceWithLength==npos || replaceWithStartIndex+replaceWithLength>=actualReplaceWithLength)
									? replaceWith.end()
									: (replaceWithStart+replaceWithLength) );

			return replace(rangeBegin, rangeEnd, replaceWithStart, replaceWithEnd);
		}
	}

	
	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.

		If replaceWithStartIndex is specified then only the part of \c replaceWith starting from that index is
		used. If replaceWithStartIndex is bigger than the length of \c replaceWith then an OutOfRangeError is thrown.
		
		If \c replaceWithLength is specified then at most this number of characters is used from \c replaceWith.
		If \c replaceWith is not long enough for \c replaceWithLength characters to be copied, or if \c replaceWithLength is String::npos,
		then only the part replaceWith up to its end is used.

		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(size_t rangeStartIndex,
						size_t rangeLength,
						const StringImpl& replaceWith,
						size_t replaceWithStartIndex = 0,
						size_t replaceWithLength = npos)
	{
		if(replaceWithStartIndex==0 && replaceWithLength==npos)
			return replace(rangeStartIndex, rangeLength, replaceWith.begin(), replaceWith.end() );
		else
		{
			size_t actualReplaceWithLength = replaceWith.getLength();
			if(replaceWithStartIndex>actualReplaceWithLength)
				throw OutOfRangeError("Invalid start index passed to String::replace");

			Iterator replaceWithStart = replaceWith.begin()+replaceWithStartIndex;

			Iterator replaceWithEnd( (replaceWithLength==npos || replaceWithStartIndex+replaceWithLength>=actualReplaceWithLength)
									? replaceWith.end()
									: (replaceWithStart+replaceWithLength) );

			return replace(rangeStartIndex, rangeLength, replaceWithStart, replaceWithEnd);
		}
	}


	/** Replaces a section of this string (defined by a start index and length) with the data between iterators that provide
		encoded string data in the format indicated by \c codec.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.
			
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	template<class CODEC, class InputIterator>
	StringImpl& replace(	const CODEC& codec,
							size_t rangeStartIndex,
							size_t rangeLength,
							InputIterator encodedReplaceWithBegin,
							InputIterator encodedReplaceWithEnd )
	{
		return replace( rangeStartIndex,
						rangeLength,						
						CODEC::DecodingIterator<InputIterator>( encodedReplaceWithBegin, encodedReplaceWithBegin, encodedReplaceWithEnd),
						CODEC::DecodingIterator<InputIterator>( encodedReplaceWithEnd, encodedReplaceWithBegin, encodedReplaceWithEnd) );
	}


	/** Replaces a section of this string (defined by two iterators) with the data between iterators that provide
		encoded string data in the format indicated by \c codec.
					
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	template<class CODEC, class InputIterator>
	StringImpl& replace(	const CODEC& codec,
							const Iterator& rangeStart,
							const Iterator& rangeEnd,
							InputIterator encodedReplaceWithBegin,
							InputIterator encodedReplaceWithEnd )
	{
		return replace( rangeStart,
						rangeEnd, 
						CODEC::DecodingIterator<InputIterator>( encodedReplaceWithBegin, encodedReplaceWithBegin, encodedReplaceWithEnd),
						CODEC::DecodingIterator<InputIterator>( encodedReplaceWithEnd, encodedReplaceWithBegin, encodedReplaceWithEnd) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-8 format.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.
		
		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in bytes.
			
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const char* replaceWith,
							size_t replaceWithLength = npos )
	{
		return replace( Utf8Codec(),
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
							size_t replaceWithLength = npos )
	{
		return replace( Utf8Codec(),
						rangeStart,
						rangeEnd,						
						replaceWith,
						getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-8 format.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.
					
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const std::string& replaceWith )
	{
		return replace( Utf8Codec(),
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
		return replace( Utf8Codec(),
						rangeStart,
						rangeEnd,						
						replaceWith.begin(),
						replaceWith.end() );
	}



	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-16 format.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.
		
		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in 16 bit elements.
			
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const char16_t* replaceWith,
							size_t replaceWithLength = npos )
	{
		return replace( Utf16Codec<char16_t>(),
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
							size_t replaceWithLength = npos )
	{
		return replace( Utf16Codec<char16_t>(),
						rangeStart,
						rangeEnd,						
						replaceWith,
						getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-16 format.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.
			
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const std::u16string& replaceWith )
	{
		return replace( Utf16Codec<char16_t>(),
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
		return replace( Utf16Codec<char16_t>(),
						rangeStart,
						rangeEnd,						
						replaceWith.begin(),
						replaceWith.end() );
	}



	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-32 format (i.e. unencoded Unicode characters).
		
		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.
		
		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in 32 bit elements.
			
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const char32_t* replaceWith,
							size_t replaceWithLength = npos )
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
							size_t replaceWithLength = npos )
	{
		return replace( rangeStart,
						rangeEnd,						
						replaceWith,
						getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.
		replaceWith must be in UTF-32 format (i.e. unencoded Unicode characters).
		
		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
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

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.
		
		If replaceWithLength is not specified then replaceWith must be a zero terminated string.
		If it is specified then it indicates the length of the string in wchar_t elements.
			
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const wchar_t* replaceWith,
							size_t replaceWithLength = npos )
	{
		return replace( WideCodec(),
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
							size_t replaceWithLength = npos )
	{
		return replace( WideCodec(),
						rangeStart,
						rangeEnd,						
						replaceWith,
						getStringEndPtr(replaceWith, replaceWithLength) );
	}


	/** Replaces a section of this string (defined by a start index and length) with the contents of replaceWith.

		If rangeStartIndex is bigger than the length of the string then an OutOfRange error is thrown.

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
		of the string.
					
		Use findReplace() instead, if you want to search for and replace a certain substring.*/
	StringImpl& replace(	size_t rangeStartIndex,
							size_t rangeLength,
							const std::wstring& replaceWith )
	{
		return replace( WideCodec(),
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
		return replace( WideCodec(),
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

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
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
		MainDataType::Codec::EncodingIterator<const char32_t*> encodedBegin( &chr );
		MainDataType::Codec::EncodingIterator<const char32_t*> encodedEnd( (&chr)+1 );

		// get the size of the encoded character
		int								encodedCharSize = 0;
		MainDataType::EncodedElement	lastEncodedElement=0;
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

		If rangeLength is String::npos or exceeds the end of the string then the end of the range is the end
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
		
		Iterator rangeEnd( (rangeLength==npos || rangeStartIndex+rangeLength>=myLength)
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
	StringImpl& append(const StringImpl& other, size_t otherSubStartIndex=0, size_t otherSubLength=npos)
	{
		return replace( _endIt, _endIt, other, otherSubStartIndex, otherSubLength);
	}
	

	/** Appends the string data between the specified two iterators to the end of this string.*/	
	template<class IT>
	StringImpl& append(const IT& beginIt, const IT& endIt)
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
	StringImpl& append(const char* other, size_t length=npos)
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
	StringImpl& append(const char16_t* o, size_t length=npos)
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
	StringImpl& append(const char32_t* o, size_t length=npos)
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
	StringImpl& append(const wchar_t* o, size_t length=npos)
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
	StringImpl& insert(size_t atIndex, const StringImpl& other, size_t otherSubStartIndex = 0, size_t otherSubLength = npos)
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
	StringImpl& insert(const Iterator& atIt, const StringImpl& other, size_t otherSubStartIndex = 0, size_t otherSubLength = npos)
	{
		return replace(atIt, atIt, other, otherSubStartIndex, otherSubLength);
	}


	/** Inserts the specified string at the specified character index.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 8 bit elements.
	*/
	StringImpl& insert(size_t atIndex, const char* o, size_t length=npos)
	{
		return insert(begin() + atIndex, o, length);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 8 bit elements.
	*/
	StringImpl& insert(const Iterator& atIt, const char* o, size_t length = npos)
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
	StringImpl& insert(size_t atIndex, const wchar_t* o, size_t length = npos)
	{
		return insert(begin() + atIndex, o, length);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded wchar_t elements.
	*/
	StringImpl& insert(const Iterator& atIt, const wchar_t* o, size_t length = npos)
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
		return replace(WideCodec(), atIt, atIt, other.begin(), other.end());
	}


	/** Inserts the specified string at the specified character index.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 16 bit elements.	*/
	StringImpl& insert(size_t atIndex, const char16_t* o, size_t length = npos)
	{
		return insert(begin() + atIndex, o, length);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 16 bit elements.
	*/
	StringImpl& insert(const Iterator& atIt, const char16_t* o, size_t length = npos)
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
		return replace(Utf16Codec<char16_t>(), atIt, atIt, other.begin(), other.end());
	}



	/** Inserts the specified string at the specified character index.

		If \c length is not specified then other must be a zero terminated string.
		If it is specified then it indicates the length of the string to insert in encoded 32 bit elements.	*/
	StringImpl& insert(size_t atIndex, const char32_t* other, size_t length = npos)
	{
		return insert(begin() + atIndex, other, length);
	}


	/** Inserts the specified string at the position corresponding to the \c atIt iterator.

	If \c length is not specified then other must be a zero terminated string.
	If it is specified then it indicates the length of the string to insert in encoded 32 bit elements.
	*/
	StringImpl& insert(const Iterator& atIt, const char32_t* other, size_t length = npos)
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
		return replace(Utf32Codec<char32_t>(), atIt, atIt, other.begin(), other.end());
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
		
		If cutLength is String::npos or cutPos+cutLength exceed the lengths of the
		string then the remainder of the string up to the end is removed.
	*/
	StringImpl& erase(size_t cutIndex=0, size_t cutLength=npos)
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
	

	/** Assigns the value of another string to this string.
		
		If otherSubStartIndex is specified then only the part of \c other starting from that index is
		assigned. If otherSubStartIndex is bigger than the length of \c other then an OutOfRangeError is thrown.

		If otherSubLength is specified then at most this number of characters is copied from \c other.
		If \c other is not long enough for \c otherSubLength characters to be copied then only the available
		characters up to the end of \c other are copied.
		*/
	StringImpl& assign(const StringImpl& other, size_t otherSubStartIndex=0, size_t otherSubLength=npos)
	{
		// just copy a reference to the source string's data
		_pData = other._pData;

		_beginIt = other._beginIt;

		if(otherSubStartIndex>=0)
			_beginIt += otherSubStartIndex;
		
		if(otherSubLength==npos || otherSubStartIndex+otherSubLength>=other.length() )
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


	/** Assigns the value of a C-style UTF8 string to this string. If length is not npos
		then it must be the length of the encoded UTF-8 string in bytes.
		If length is npos then the other string must be zero-terminated.
		*/
	StringImpl& assign(const char* o, size_t length=npos)
	{
		return replace(_beginIt, _endIt, o, length );
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& assign(const std::wstring& o)
	{
		return replace(_beginIt, _endIt, o);
	}


	/** Assigns the value of a C-style wide char string to this string. If length is not npos
		then it must be the number of encoded wchar_t elements.
		If length is npos then the other string must be zero-terminated.
		*/
	StringImpl& assign(const wchar_t* o, size_t length=npos)
	{
		return replace(_beginIt, _endIt, o, length );
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& assign(const std::u16string& o)
	{
		return replace(_beginIt, _endIt, o );
	}


	/** Assigns the value of a C-style UTF-16 string to this string. If length is not npos
		then it must be the number of encoded 16 bit UTF-16 elements.
		If length is npos then the other string must be zero-terminated.
		*/
	StringImpl& assign(const char16_t* o, size_t length=npos)
	{
		return replace(_beginIt, _endIt, o, length );
	}


	/** Assigns the value of another string to this string. 	*/
	StringImpl& assign(const std::u32string& o)
	{
		return replace(_beginIt, _endIt, o );
	}


	/** Assigns the value of a C-style UTF-32 string to this string. If length is not npos
		then it must be the number of encoded 32 bit UTF-32 elements.
		If length is npos then the other string must be zero-terminated.
		*/
	StringImpl& assign(const char32_t* o, size_t length=npos)
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
	template <class IT>
	StringImpl& assign(IT beginIt, IT endIt)
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






	/*


	Iterator find(const StringImpl& s) const
	{
		return find(s._beginIt, s._endIt, _beginIt )
	}

	Iterator find(const StringImpl& s, const Iterator& searchBeginIt) const
	{
		return find(s._beginIt, s._endIt, searchBeginIt )
	}


	void replaceAll(char32_t oldChr, char32_t newChr) const
	{
		ReplacingCharIterator beginIt(begin(), oldChar, newChr);
		ReplacingCharIterator endIt(end(), oldChar, newChr);

		*this = String(beginIt, endIt);
	}

	
	template<class ToFindIterator>
	void replaceAll(	const ToFindIterator& toFindBegin,
						const ToFindIterator& toFindEnd,
						const Iterator& replaceWithBegin,
						const Iterator& replaceWithEnd,
						const Iterator& searchBeginIt )
	{
		IteratorCombiner result;

		Iterator posIt = searchBeginIt;
		while (posIt != _endIt)
		{
			Iterator matchEnd;
			Iterator matchBegin = find(toFindBegin, toFindEnd, posIt, &matchEnd);

			if (matchBegin == matchEnd)
			{
				// no more matches.

				if (posIt == searchBeginIt)
				{
					// text not found and nothing has been replaced. we are done.
					return;
				}
				else
				{
					result.add( posIt, _endIt );
					break;
				}
			}

			result.add( posIt, matchBegin );
			result.add( replaceWithBegin, replaceWithEnd );

			posIt = matchEnd;
		}


		*this = StringImpl( IteratorCombiner.begin(), IteratorCombiner.end() );
	}


	
	void replaceAll(const StringImpl& toFind, const StringImpl& replaceWith)
	{
		return replaceAll(toFind.begin(), toFind.end(), replaceWith.begin(), replaceWith.end());
	}
	*/

protected:

	template<class T>
	typename const T::EncodedString& getEncoded() const
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

	template<>
	typename const MainDataType::EncodedString& getEncoded<MainDataType>() const
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
			
			_pData = newObj<MainDataType>( MainDataType::Codec(), _beginIt.getInner(), _endIt.getInner() );

			_beginIt = _pData->begin();
			_endIt = _pData->end();			
		}
		else
		{
			MainDataType::EncodedString* pStd = &_pData->getEncodedString();

			if(_beginIt.getInner()!=pStd->begin()
				|| _endIt.getInner()!=pStd->end() )
			{
				// we are working on a substring of the data. Throw away the other parts.
				// Note that we want to avoid re-allocation, so we want to do this in place.
				// First we cut off what we do not need from the end, then from the start.
				// Unfortunately, cutting off from the end will invalidate our begin iterator,
				// so we need to save its value as an index.

				int startIndex = _beginIt.getInner() - pStd->begin();

				if(_endIt.getInner()!=pStd->end())
					pStd->erase( _endIt.getInner(), pStd->end() );

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

