#ifndef BDN_StringData_H_
#define BDN_StringData_H_

#include <string>

#include <bdn/Utf8Codec.h>
#include <bdn/Utf16Codec.h>
#include <bdn/WideCodec.h>

namespace bdn
{

/** Returns a pointer to the element just after the last element of the string.

	It is usually not necessary to specify the template parameter explicitly.

	Example:
	\code
	const char* pEnd = getStringEndPtr("hello");
	// pEnd points to the element after the 'o'

	const char* pEnd = getStringEndPtr("hello", 4);
	// pEnd points to the 'o'
	\endcode

	@param p string pointer
	@param length If this is -1 then the string must be zero-terminated. The zero terminator
		is not considered part of the string (i.e. the result will be a pointer to the zero terminator).
		If this not -1 then it is the length of the string in elements (e.g. in wchars for a wchar_t*, chars for a char*, etc.).

	\tparam PTR a string pointer type (for example char32_*, const char*, ...)
	*/
template<class PTR>
PTR getStringEndPtr(PTR p, size_t length=std::string::npos)
{
	if(length<0)
	{
		while( (*p) != 0)
			p++;
	}
	else
		p+=length;

	return p;
}


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
	typedef typename Codec::EncodedElement EncodedElement;


	/** The allocator type that is used to allocate the internal encoded data.*/
	typedef typename EncodedString::allocator_type Allocator;


	/** Iterator type for the string data. The iterator returns fully decoded 32 bit Unicode characters (char32_t).*/
	class Iterator : public Codec::DecodingStringIterator
	{
	public:
		Iterator(	const typename EncodedString::const_iterator& encodedIt,
					const typename EncodedString::const_iterator& encodedBegin,
					const typename EncodedString::const_iterator& encodedEnd)
			: Codec::template DecodingIterator<typename EncodedString::const_iterator>(encodedIt, encodedBegin, encodedEnd)
		{
		}

		Iterator()
		{
		}

		Iterator& operator+=(std::ptrdiff_t val)
		{
			for(std::ptrdiff_t i=0; i<val; i++)
				this->operator++();

			return *this;
		}

		Iterator& operator-=(std::ptrdiff_t val)
		{
			for(std::ptrdiff_t i=0; i<val; i++)
				this->operator--();

			return *this;
		}
        

		Iterator operator+(std::ptrdiff_t val) const
		{
			Iterator it = *this;
			it+=val;

			return it;
		}

		Iterator operator-(std::ptrdiff_t val) const
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
	
		@param s UTF-8 encoded string data. If lengthElements is std::string::npos then this must be zero terminated.
		@param lengthBytes length of the string data in bytes. If this is std::string::npos then the length is
			auto detected and the string data must be zero-terminated.*/
	StringData(const char* s, size_t lengthBytes = std::string::npos)
		: StringData(	Utf8Codec(),
						s,
						getStringEndPtr(s, lengthBytes) )
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

		@param s UTF-16 encoded string data. If lengthElements is std::string::npos then this must be zero terminated.
		@param lengthElements length of the string data in encoded 16 bit elements. If this is std::string::npos
			then the length is auto detected and the string data must be zero-terminated.*/
	StringData(const char16_t* s, size_t lengthElements = std::string::npos)
		: StringData(	Utf16Codec(),
						s,
						getStringEndPtr(s, lengthElements) )
	{
	}


	/** Initializes the object from the specified UTF-16 encoded string.*/
	StringData(const std::u16string& s)
		: StringData(	Utf16Codec(),
						s.begin(),
						s.end() )
	{
	}


	/** Initializes the object from the specified wchar string.

		@param s wchar string data. This must be either UTF-16 or UTF-32 encoded (depending on whether wchar_t is
			2 bytes long or 4 bytes long).
			If lengthElements is std::string::npos then this must be zero terminated.
		@param lengthElements length of the string data in encoded wchar_t elements. If this is std::string::npos
			then the length is auto detected and the string data must be zero-terminated.*/
	StringData(const wchar_t* s, size_t lengthElements = std::string::npos)
		: StringData(	WideCodec(),
						s,
						getStringEndPtr(s, lengthElements) )
	{
	}


	/** Initializes the object from the specified wstring string. This must be either UTF-16 or
		UTF-32 encoded (depending on whether wchar_t is	2 bytes long or 4 bytes long).*/
	StringData(const std::wstring& s)
		: StringData(	WideCodec(),
						s.begin(),
						s.end() )
	{
	}

	
	/** Initializes the object from the specified UTF-32 encoded string.

		@param s UTF-32 encoded string data. If lengthElements is std::string::npos then this must be zero terminated.
		@param lengthElements length of the string data in encoded 32 bit elements. If this is std::string::npos
			then the length is auto detected and the string data must be zero-terminated.*/
	StringData(const char32_t* s, size_t lengthElements = std::string::npos)
		: StringData(	s,
						getStringEndPtr(s, lengthElements))
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
		typename Codec::template EncodingIterator<InputDecodedCharIterator> encodingBeginIt(beginIt);
		typename Codec::template EncodingIterator<InputDecodedCharIterator> encodingEndIt(endIt);

		_encodedString = typename Codec::EncodedString(encodingBeginIt, encodingEndIt );
	}
    
    
    
    /** Initializes the object with the data between two iterators whose data is encoded with the same codec
        as the StringData object.
    */
    StringData(const Codec& codec, typename Codec::EncodedString::iterator inputEncodedBeginIt, typename Codec::EncodedString::iterator inputEncodedEndIt)
    {
        _encodedString = typename Codec::EncodedString(inputEncodedBeginIt, inputEncodedEndIt);
    }
    
    
    /** Initializes the object with the data between two data pointers whose data is encoded with the same codec
        as the StringData object.
    */
    StringData(const Codec& codec, typename Codec::EncodedElement* pBegin, typename Codec::EncodedElement* pEnd)
    {
        _encodedString = typename Codec::EncodedString(pBegin, (pEnd-pBegin) );
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
		typename InputCodec::template DecodingIterator<InputEncodedIterator> inputBeginIt(inputEncodedBeginIt, inputEncodedBeginIt, inputEncodedEndIt);
		typename InputCodec::template DecodingIterator<InputEncodedIterator> inputEndIt(inputEncodedEndIt, inputEncodedBeginIt, inputEncodedEndIt);
		
		typename Codec::template EncodingIterator<typename InputCodec::template DecodingIterator<InputEncodedIterator> > encodingBeginIt(inputBeginIt);
		typename Codec::template EncodingIterator<typename InputCodec::template DecodingIterator<InputEncodedIterator> > encodingEndIt(inputEndIt);

		_encodedString = typename Codec::EncodedString(encodingBeginIt, encodingEndIt);
	}



	
	/** Conversion operator that returns a reference to the internal encoded data
		(as a std::basic_string).*/
	operator const typename Codec::EncodedString&() const
	{
		return _encodedString;
	}

	/** Returns a reference to the internal encoded data (std::basic_string).*/
	const typename Codec::EncodedString& getEncodedString() const
	{
		return _encodedString;
	}


	/** Returns a reference to the internal encoded data (std::basic_string).*/
	typename Codec::EncodedString& getEncodedString()
	{
		return _encodedString;
	}


	/** Returns a pointer to a C-style zero terminated string. The string is encoded according
		to the Codec used by this StringData.*/
	const typename Codec::EncodedElement* asPtr() const
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
	
	typename Codec::EncodedString	_encodedString;
};

}

#endif
