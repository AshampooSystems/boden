#ifndef BDN_StringData_H_
#define BDN_StringData_H_

#include <string>

namespace bdn
{


/** Stores encoded string data, according to the codec specified as the template parameter.

	The StringData object hides the particularities internal encoding (UTF-8, UTF-16, ...)
	and exposes the data as a sequence of decoded 32 bit unicode characters.
	
	\tparam Codec must be a string codec class. The class must contain inner classes DecodingIterator and EncodingIterator
		that match the following prototype:
		
		\code
		template<class SourceIterator>
		class DecodingIterator
		{
		public:
			DecodingIterator(const SourceIterator& sourceIt, const SourceIterator& beginSourceIt, const SourceIterator& endSourceIt);
		};

		template<class SourceIterator>
		class EncodingIterator
		{
		public:
			EncodingIterator(const SourceIterator& sourceIt);
		}
		\endcode

		These two inner iterator classes must transcode between the internal encoded format and decoded 32 bit unicode characters (char32_t).
		See #Utf8Codec for an example and a description of the parameters.

*/
template<class CODEC>
class StringData : public Base
{
public:

	typedef CODEC Codec;
	typedef typename Codec::EncodedString EncodedString;


	/** Iterator type for the string data. The iterator returns fully decoded 32 bit Unicode characters (char32_t).*/
	class Iterator : public Codec::DecodingStringIterator
	{
	public:
		Iterator(	const typename EncodedString::const_iterator& encodedIt,
					const typename EncodedString::const_iterator& encodedBegin,
					const typename EncodedString::const_iterator& encodedEnd)
			: Codec::DecodingIterator<EncodedString::const_iterator>(encodedIt, encodedBegin, encodedEnd)
		{
		}

		Iterator()
		{
		}

		Iterator& operator+=(int val)
		{
			for(int i=0; i<val; i++)
				operator++();

			return *this;
		}

		Iterator& operator-=(int val)
		{
			for(int i=0; i<val; i++)
				operator--();

			return *this;
		}


		Iterator operator+(int val) const
		{
			Iterator it = *this;
			it+=val;

			return it;
		}

		Iterator operator-(int val) const
		{
			Iterator it = *this;
			it-=val;

			return it;
		}
	};

	StringData()
		: StringData("", 0)
	{
	}


	/** Initializes the object from the specified UTF-8 encoded string.
	
		@param s UTF-8 encoded string data. If lengthElements is -1 then this must be zero terminated.
		@param lengthElements length of the string data in bytes. If this is -1 then the length is
			auto detected and the string data must be zero-terminated.*/
	StringData(const char* s, int lengthBytes = -1)
		: StringData(	Utf8Codec(),
						s,
						makeEndIt(s, lengthBytes) )
	{
	}


	/** Initializes the object from the specified UTF-8 encoded string.*/
	StringData(const std::string& s)
		: StringData(	Utf8Codec(),
						s.begin(),
						s.end() )
	{
	}


	/** Initializes the object from the specified UTF-16 encoded string.

		@param s UTF-16 encoded string data. If lengthElements is -1 then this must be zero terminated.
		@param lengthElements length of the string data in encoded 16 bit elements. If this is -1
			then the length is auto detected and the string data must be zero-terminated.*/
	StringData(const char16_t* s, int lengthElements = -1)
		: StringData(	Utf16Codec<char16_t>(),
						s,
						makeEndIt(s, lengthElements) )
	{
	}


	/** Initializes the object from the specified UTF-16 encoded string.*/
	StringData(const std::u16string& s)
		: StringData(	Utf16Codec<char16_t>(),
						s.begin(),
						s.end() )
	{
	}


	/** Initializes the object from the specified wchar string.

		@param s wchar string data. This must be either UTF-16 or UTF-32 encoded (depending on whether wchar_t is
			2 bytes long or 4 bytes long).
			If lengthElements is -1 then this must be zero terminated.
		@param lengthElements length of the string data in encoded wchar_t elements. If this is -1
			then the length is auto detected and the string data must be zero-terminated.*/
	StringData(const wchar_t* s, int lengthElements = -1)
		: StringData(	WcharCodec(),
						s,
						makeEndIt(s, lengthElements) )
	{
	}


	/** Initializes the object from the specified wstring string. This must be either UTF-16 or
		UTF-32 encoded (depending on whether wchar_t is	2 bytes long or 4 bytes long).*/
	StringData(const std::wstring& s)
		: StringData(	WcharCodec(),
						s.begin(),
						s.end() )
	{
	}

	
	/** Initializes the object from the specified UTF-32 encoded string.

		@param s UTF-32 encoded string data. If lengthElements is -1 then this must be zero terminated.
		@param lengthElements length of the string data in encoded 32 bit elements. If this is -1
			then the length is auto detected and the string data must be zero-terminated.*/
	StringData(const char32_t* s, int lengthElements = -1)
		: StringData(	s,
						makeEndIt(s, lengthElements))
	{
	}


	/** Initializes the object from the specified UTF-32 encoded string.*/
	StringData(const std::u32string& s)
		: StringData(s.begin(), s.end())
	{
	}


	/** Initializes the object with the data between two character iterators.
		The iterators must return fully decoded 32 bit Unicode characters.*/
	template<class InputDecodedCharIterator>
	StringData(InputDecodedCharIterator beginIt, InputDecodedCharIterator endIt)
	{
		Codec::EncodingIterator<InputDecodedCharIterator> encodingBeginIt(beginIt);
		Codec::EncodingIterator<InputDecodedCharIterator> encodingEndIt(endIt);

		_encodedString = typename Codec::EncodedString(encodingBeginIt, encodingEndIt );
	}


	/** Initializes the object with the data between two iterators whose data is encoded
		according to the specified InputCodec codec type.
		
		The data is first automatically decoded and then re-encoded with the Codec used by this
		StringData object.
		The only exception is if InputCodec is the same codec that this StringData uses. In that
		case the data will simply be copied as is.
		*/
	template<class InputCodec, class InputEncodedIterator>
	StringData(const InputCodec& codec, InputEncodedIterator inputEncodedBeginIt, InputEncodedIterator inputEncodedEndIt)
	{
		InputCodec::DecodingIterator<InputEncodedIterator> inputBeginIt(inputEncodedBeginIt, inputEncodedBeginIt, inputEncodedEndIt);
		InputCodec::DecodingIterator<InputEncodedIterator> inputEndIt(inputEncodedEndIt, inputEncodedBeginIt, inputEncodedEndIt);
		
		Codec::EncodingIterator<InputCodec::DecodingIterator<InputEncodedIterator> > encodingBeginIt(inputBeginIt);
		Codec::EncodingIterator<InputCodec::DecodingIterator<InputEncodedIterator> > encodingEndIt(inputEndIt);

		_encodedString = typename Codec::EncodedString(encodingBeginIt, encodingEndIt);
	}


	/** Specialized version of the general encoded iterator constructor for the case in which the specified
		codec matches the one used by this StringData object. This is an implementation optimization - users can ignore this.*/
	template<>
	StringData(const Codec& codec, typename Codec::EncodedString::iterator inputEncodedBeginIt, typename Codec::EncodedString::iterator inputEncodedEndIt)
	{
		_encodedString = typename Codec::EncodedString(inputEncodedBeginIt, inputEncodedEndIt);
	}


	/** Specialized version of the general encoded iterator constructor for the case in which the specified
		codec matches the one used by this StringData object and the begin and end iterators are pointers to
		raw encoded data. This is an implementation optimization - users can ignore this.*/
	template<>
	StringData(const Codec& codec, typename Codec::EncodedElement* pBegin, typename Codec::EncodedElement* pEnd)
	{
		_encodedString = typename Codec::EncodedString(pBegin, (pEnd-pBegin) );
	}

	
	/** Conversion operator that returns a reference to the internal encoded data
		(as a std::basic_string).*/
	operator const typename Codec::EncodedString&() const
	{
		return _encodedString;
	}

	/** Returns the internal encoded data as a std::basic_string.*/
	const typename Codec::EncodedString& toStd() const
	{
		return _encodedString;
	}


	/** Returns a pointer to a C-style zero terminated string. The string is encoded according
		to the Codec used by this StringData.*/
	const typename Codec::EncodedElement* getCString() const
	{
		return _encodedString.c_str();
	}


	/** Returns an iterator that points to the first character of the string data.

		The iterator returns fully decoded 32 bit Unicode characters (char32_t).
	*/
	Iterator begin() const
	{
		return Iterator(_encodedString.begin(), _encodedString.begin(), _encodedString.end() );
	}


	/** Returns an iterator that points to the position just after the last character of the string data.
	
		The iterator returns fully decoded 32 bit Unicode characters (char32_t).
	*/
	Iterator end() const
	{
		return Iterator(_encodedString.end(), _encodedString.begin(), _encodedString.end() );
	}


	/** Copies the encoded data from the specified StringData object into this object.*/
	StringData& operator=(const StringData& o)
	{
		_encodedString = o._encodedString;
		return *this;
	}


	/** Returns a pointer to a global StringData object that represents an empty string.*/
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

		return endIt;
	}

	typename Codec::EncodedString	_encodedString;
};

}

#endif
