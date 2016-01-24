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

	typedef typename MainDataType::Iterator Iterator;

	typedef Iterator iterator;

	
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

		_charCountIfKnown = s._charCountIfKnown;
	}

	StringImpl(const StringImpl& s, const Iterator& beginIt, const Iterator& endIt )
	{
		_pData = s._pData;

		// cannot copy _pDataInDifferentEncoding because we only want a substring of it.

		_beginIt = beginIt;
		_endIt = endIt;

		_charCountIfKnown = -1;
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
		_charCountIfKnown = -1;
	}


	bool isEmpty() const
	{
		return (_beginIt == _endIt);
	}


	/** Returns the number of characters in this string.*/
	int getCharCount() const
	{
		if (_charCountIfKnown == -1)
		{
			// character count is unknown. We need to count it first.
			int c = 0;
			Iterator it = _beginIt;
			while (it != _endIt)
			{
				++c;
				++it;
			}

			_charCountIfKnown = c;
		}

		return _charCountIfKnown;
	}


	/** Same as #getCharCount.*/
	int length() const
	{
		return getCharCount();
	}


	Iterator begin()
	{
		return _beginIt;
	}


	Iterator end()
	{
		return _endIt;
	}


	/** Returns a sub string of this string, starting at the character that beginIt points to
		and ending at the character before the position pointed to by endIt.
	*/
	StringImpl subString(const Iterator& beginIt, const Iterator& endIt) const
	{
		return StringImpl(*this, beginIt, endIt);
	}


	/** Returns a sub string of this string, starting at startIndex and including charCount characters
	from that point. If the string has less than charCount characters then the sub string up to the end
	is returned.

	charCount can be -1, in which case the rest of the string up to the end is returned.
	*/
	StringImpl subString(int startIndex, int charCount) const
	{
		int myCharCount = getCharCount();

		if (startIndex<0 || startIndex>myCharCount)
			throw InvalidArgumentError("String::subString: Invalid start index: "+std::to_string(startIndex) );
		if (charCount>=0 && startIndex+charCount>myCharCount)
			charCount = myCharCount-startIndex;

		Iterator startIt = _beginIt+startIndex;
		Iterator endIt = (charCount<0) ? _endIt : (startIt+charCount);

		return StringImpl(*this, startIt, endIt);
	}


	/** Same as #subString. This function is included for compatibility with std::string.*/
	StringImpl substr(int startIndex, int charCount) const
	{
		return subString(startIndex, charCount);
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

	mutable int				_charCountIfKnown;
};

}


#endif

