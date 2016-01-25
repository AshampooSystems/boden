#ifndef BDN_StringImpl_H_
#define BDN_StringImpl_H_

#include <bdn/StringData.h>

#include <iterator>
#include <list>

namespace bdn
{


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



	/** Included for compatibility with std::string only.*/
	static const size_t npos = -1;

	
	StringImpl()
		: StringImpl( MainDataType::getEmptyData() )
	{
	}

	StringImpl(const StringImpl& s)
	{
		_pData = s._pData;
		_pDataInDifferentEncoding = s._pDataInDifferentEncoding;

		_beginIt = s._beginIt;
		_endIt = s._endIt;

		_lengthIfKnown = s._lengthIfKnown;
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


	/** Returns a pointer to a zero terminated c-style string in UTF-8 encoding.

		This operation might invalidate existing iterators.
	*/
	const char* getCString_UTF8() const
	{
		Utf8StringData* pTypedData = getTypedData<Utf8StringData>( !isZeroTerminated() );

		return pTypedData->getData(_beginIt.getInnerIt());
	}


	/** Same as #getCString_UTF8. This function is included for compatibility with std::string.*/
	const char* c_str() const
	{
		return getCString_UTF8();
	}


	/** Returns a pointer to a zero terminated c-style string in the locale's multibyte encoding.

		This operation might invalidate existing iterators.
	*/
	const char* getCString_Locale() const
	{
		return getLocaleData()->getData();			
	}


	/** Returns a pointer to a zero terminated c-style string in "wide char" encoding
		(either UTF-16 or UTF-32, depending on the size of wchar_t).
	
		This operation might invalidate existing iterators.
	*/
	const wchar_t* getCWString() const
	{
		WStringData* pTypedData = getTypedData<WStringData>( !isZeroTerminated() );

		return pTypedData->getData(_beginIt.getInnerIt());
	}


	/** Returns a pointer to a zero terminated c-style string in UTF-16 encoding.

		This operation might invalidate existing iterators.
	*/
	const char16_t* getC16String() const
	{
		Utf16StringData* pTypedData = getTypedData<Utf16StringData>( !isZeroTerminated() );

		return pTypedData->getData(_beginIt.getInnerIt());
	}


	/** Returns a pointer to a zero terminated c-style string in UTF-32 encoding.

		This operation might invalidate existing iterators.
	*/
	const char32_t* getC32String() const
	{
		Utf32StringData* pTypedData = getTypedData<Utf32StringData>( !isZeroTerminated() );

		return pTypedData->getData(_beginIt.getInnerIt());
	}


	/** Returns a reference to a std::string object in UTF-8 encoding.

		This operation might invalidate existing iterators.
	*/
	const std::string& getStdString_Utf8() const
	{
		Utf8StringData* pTypedData = getTypedData<Utf8StringData>( isSubString() );

		return *pTypedData;
	}


	/** Returns a reference to a std::string object in in the locale's multibyte encoding.

		This operation might invalidate existing iterators.
	*/
	const std::string& getStdString_Locale() const
	{
		return *getLocaleData();
	}


	/** Returns a reference to a std::wstring object in "wide char" encoding
		(either UTF-16 or UTF-32, depending on the size of wchar_t).

		This operation might invalidate existing iterators.
	*/
	const std::wstring& getStdWString() const
	{
		WStringData* pTypedData = getTypedData<WStringData>( isSubString() )

			return *pTypedData;
	}


	/** Returns a reference to a std::u16string object in UTF-16 encoding.

		This operation might invalidate existing iterators.
	*/
	const std::u16string& getStdU16String() const
	{
		Utf16StringData* pTypedData = getTypedData<Utf16StringData>( isSubString() )

			return *pTypedData;
	}


	/** Returns a reference to a std::u32string object in UTF-32 encoding.

		This operation might invalidate existing iterators.
	*/
	const std::u32string& getStdU32String() const
	{
		Utf32StringData* pTypedData = getTypedData<Utf32StringData>( isSubString() )

			return *pTypedData;
	}


	int compare(const StringImpl& o) const
	{
		Iterator myIt = _beginIt;
		Iterator otherIt = o._beginIt;

		while(true)
		{
			if(myIt==_endIt)
				return (otherIt==o._endIt) ? 0 : -1;

			else if(otherIt==o._endIt)
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

	bool operator==(const StringImpl& o) const
	{
		return compare(o)==0;
	}

	bool operator!=(const StringImpl& o) const
	{
		return compare(o)!=0;
	}

	bool operator<(const StringImpl& o) const
	{
		return compare(o) < 0;
	}

	bool operator>(const StringImpl& o) const
	{
		return compare(o) > 0;
	}

	bool operator<=(const StringImpl& o) const
	{
		return compare(o) <= 0;
	}

	bool operator>=(const StringImpl& o) const
	{
		return compare(o) >= 0;
	}


	char32_t operator[](size_t index)
	{
		if(index<0 || index>=getCharCount())
			throw InvalidArgError("String::operator[]", "Invalid index");

		return *(_beginIt+index);
	}




	Iterator find(char32_t chr) const
	{
		return std::find(_beginIt, _endIt, chr);
	}

	Iterator find(char32_t chr, const Iterator& searchBeginIt) const
	{
		return std::find(searchBeginIt, _endIt, chr);
	}


	template<class ToFindCharIterator>
	Iterator find(	const ToFindCharIterator& toFindBegin,
					const ToFindCharIterator& toFindEnd,
					const Iterator& searchBeginIt,
					Iterator* pMatchEnd = nullptr) const
	{
		for (Iterator matchBegin = searchBeginIt; matchBegin != end(); matchBegin++)
		{
			ToFindCharIterator toFindCurr = toFindBegin;
			Iterator		   matchCurr = matchBegin;
			while (true)
			{
				if (toFindCurr == toFindEnd)
				{
					if (pMatchEnd != nullptr)
						*pMatchEnd = matchCurr;
					return matchBegin;
				}

				if (matchCurr == end())
				{
					// remainder of string is shorter than string we search for.
					// => there can not be any more matches.
					if (pMatchEnd != nullptr)
						*pMatchEnd = end();
					return end();
				}

				if ((*matchCurr) != (*toFindCurr))
					break;

				++toFindCurr;
				++matchCurr;
			}
		}

		if (pMatchEnd != nullptr)
			*pMatchEnd = end();

		return end();
	}



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


protected:
	bool isZeroTerminated() const
	{
		return ( _endIt==pData->end() );
	}

	/** Returns true if this string is a substring of another string. I.e. if we only
	work on PART of the internal string data.*/
	bool isSubString() const
	{
		return (_beginIt!=_pData->begin() || _endIt!=_pData->end() );
	}


	template<class StringDataType>
	StringDataType* getTypedData(bool forceCopy) const
	{
		if (!forceCopy)
		{
			StringDataType* pTypedData = dynamic_cast<StringDataType*>(_pDataInDifferentEncoding);
			if (pTypedData != nullptr)
				return pTypedData;
		}

		_pDataInDifferentEncoding = newObj<StringDataType>(_beginIt, _endIt);

		return _pDataInDifferentEncoding;
	}


	template<>
	MainDataType* getTypedData(bool forceCopy) const
	{
		if(forceCopy)
		{
			// this usually means that we want the data to be zero-terminated, but it currently is not.
			_pData = newObj<MainDataType>(_beginIt, _endIt);
			_beginIt = _pData->begin();
			_endIt = _pData->end();
		}

		return _pData;		
	}


	mutable P<MainDataType>	_pData;
	mutable Iterator		_beginIt;
	mutable Iterator		_endIt;

	mutable P<Base>			_pDataInDifferentEncoding;

	mutable int				_lengthIfKnown;
};

}


#endif

