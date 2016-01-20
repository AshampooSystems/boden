#ifndef BDN_StringData_H_
#define BDN_StringData_H_

#include <string>

namespace bdn
{


template<class Codec, class Alloc = std::allocator<typename Codec::EncodedElement> >
class StringData : public Base
{
public:

	typedef typename Codec::DecodingStringIterator Iterator;

	StringData()
		: StringData("", 0)
	{
	}

	StringData(const char* s, int lengthElements = -1)
		: StringData(	Utf8Codec(),
						s,
						makeEndIt(s, lengthElements) )
	{
	}

	StringData(const std::string& s)
		: StringData(	Utf8Codec(),
						s.begin(),
						s.end() )
	{
	}

	StringData(const char16_t* s, int lengthElements = -1)
		: StringData(	Utf16Codec(),
						s,
						makeEndIt(s, lengthElements) )
	{
	}

	StringData(const std::u16string& s)
		: StringData(	Utf16Codec(),
						s.begin(),
						s.end() )
	{
	}

	StringData(const wchar_t* s, int lengthElements = -1)
		: StringData(	WcharCodec(),
						s,
						makeEndIt(s, lengthElements) )
	{
	}

	StringData(const std::wstring& s)
		: StringData(	WcharCodec(),
						s.begin(),
						s.end() )
	{
	}

	

	StringData(const char32_t* s, int lengthElements = -1)
		: StringData(	s,
						makeEndIt(s, lengthElements))
	{
	}

	StringData(const std::u32string& s)
		: StringData(s.begin(), s.end())
	{
	}


	template<class InputDecodedCharIterator>
	StringData(InputDecodedCharIterator beginIt, InputDecodedCharIterator endIt)
	{
		Codec::EncodingIterator<InputDecodedCharIterator> encodingBeginIt(beginIt);
		Codec::EncodingIterator<InputDecodedCharIterator> encodingEndIt(endIt);

		_encodedString = typename Codec::EncodedString(encodingBeginIt, encodingEndIt );

		// unknown character count
		_charCountIfKnown = -1;
	}



	template<class InputCodec, class InputEncodedIterator>
	StringData(const InputCodec& codec, InputEncodedIterator inputEncodedBeginIt, InputEncodedIterator inputEncodedEndIt)
	{
		InputCodec::DecodingIterator<InputEncodedIterator> inputBeginIt(inputEncodedBeginIt, inputEncodedBeginIt, inputEncodedEndIt);
		InputCodec::DecodingIterator<InputEncodedIterator> inputEndIt(inputEncodedEndIt, inputEncodedBeginIt, inputEncodedEndIt);
		
		Codec::EncodingIterator<InputCodec::DecodingIterator<InputEncodedIterator> > encodingBeginIt(inputBeginIt);
		Codec::EncodingIterator<InputCodec::DecodingIterator<InputEncodedIterator> > encodingEndIt(inputEndIt);

		_encodedString = typename Codec::EncodedString(encodingBeginIt, encodingEndIt);

		// unknown character count
		_charCountIfKnown = -1;
	}

	template<>
	StringData(const Codec& codec, typename Codec::EncodedString::iterator inputEncodedBeginIt, typename Codec::EncodedString::iterator inputEncodedEndIt)
	{
		_encodedString = typename Codec::EncodedString(inputEncodedBeginIt, inputEncodedBeginIt);

		// unknown character count
		_charCountIfKnown = -1;
	}

	template<>
	StringData(const Codec& codec, typename Codec::EncodedElement* pBegin, typename Codec::EncodedElement* pEnd)
	{
		_encodedString = typename Codec::EncodedString(pBegin, (pEnd-pBegin) );

		// unknown character count
		_charCountIfKnown = -1;
	}

	
	int getCharCount() const
	{
		if (_charCountIfKnown == -1)
		{
			// character count is unknown. We need to count it first.
			int c = 0;
			Iterator it = begin();
			Iterator end = end();
			while (it != end)
			{
				++c;
				++it;
			}

			_charCountIfKnown = c;
		}

		return _charCount;
	}

	bool isEmpty() const
	{
		return (_charCountIfKnown == 0 || (_charCountIfKnown == -1 && begin() == end()));
	}


	operator const typename Codec::EncodedString&() const
	{
		return _encodedString;
	}

	const typename Codec::EncodedString& toStd() const
	{
		return _encodedString;
	}

	const typename Codec::EncodedElement* c_str() const
	{
		return _encodedString.c_str();
	}



	Iterator begin() const
	{
		return Iterator(_encodedString.begin(), _encodedString.begin(), _encodedString.end() );
	}

	Iterator end() const
	{
		return Iterator(_encodedString.end(), _encodedString.begin(), _encodedString.end() );
	}


	StringData& operator=(const StringData& o)
	{
		_encodedString = o._encodedString;
		return *this;
	}


	static P<StringData> getEmptyData()
	{
		static SafeInit<StringData> init;

		return init.get();
	}

protected:
	template<class IT>
	static IT makeEndIt(IT beginIt, int lengthElements)
	{
		IT endIt = beginIt;

		if (lengthElements < 0)
		{
			while ((*endIt) != 0)
				++endIt;
		}
		else
		{
			while (lengthElements > 0)
			{
				++endIt;
				lengthElements--;
			}
		}
	}

	typename Codec::EncodedString	_encodedString;
	int								_charCountIfKnown;
};

}

#endif
