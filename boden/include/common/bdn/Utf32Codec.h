#ifndef BDN_Utf32Codec_H_
#define BDN_Utf32Codec_H_

#include <string>

namespace bdn
{

/** The Utf-32 string codec.

	The template parameter EL must be a 32 bit integer type. Usually either char32_t or
	wchar_t (on systems where wchar_t is 32 bit)
*/
template<class EL>
class Utf32Codec
{
public:

	/** The encoded element type. This is a 32 bit integer type, defined by the template parameter EL.*/
	typedef EL EncodedElement;


	/** The std string type for the encoded string.*/
	typedef std::basic_string<EncodedElement> EncodedString;



	/** A character iterator that decodes UTF-32 data into Unicode characters (also char32_t).

		Implementation note:
		This does not actually do any real decoding work, since UTF-32 stores all characters in a plain 1:1 way.
		So this class simply passes the UTF-32 elements (=characters) through.
	*/
	template<class SourceIterator>
	class DecodingIterator : public std::iterator<std::bidirectional_iterator_tag, char32_t>
	{
	public:
		DecodingIterator(const SourceIterator& sourceIt, const SourceIterator& beginSourceIt, const SourceIterator& endSourceIt)
		{
			_sourceIt = sourceIt;

			// we do not need to store the begin or end iterators, since we cannot overshoot them (unless our caller
			// advances us even though the end has been reached).
		}

		DecodingIterator()
		{
		}

		DecodingIterator& operator++()
		{
			++_sourceIt;
			return *this;
		}

		DecodingIterator operator++(int)
		{
			DecodingIterator oldVal(*this);

			operator++();

			return oldVal;			
		}

		DecodingIterator& operator--()
		{
			_sourceIt--;
			return *this;
		}

		DecodingIterator operator--(int)
		{
			DecodingIterator oldVal(*this);

			operator--();

			return oldVal;			
		}

		char32_t operator*()
		{
			return *_sourceIt;
		}

		bool operator==(const DecodingIterator& o) const
		{
			return (_sourceIt==o._sourceIt);
		}

		bool operator!=(const DecodingIterator& o) const
		{
			return !operator==(o);
		}


	protected:
		SourceIterator  _sourceIt;
	};



	/** A decoding iterator that works on the iterator type of the encoded standard string type
		#EncodedString.*/
	typedef DecodingIterator<typename EncodedString::const_iterator> DecodingStringIterator;



	/** Encodes unicode characters to UTF-32.

		Implementation note:
		This does not actually do any real encoding work, since UTF-32 stores all characters in a plain 1:1 way.
		So this class simply passes the UTF-32 elements (=characters) through.
	*/
	template<class SourceIterator>
	class EncodingIterator : public std::iterator<std::bidirectional_iterator_tag, EncodedElement>
	{
	public:
		EncodingIterator(const SourceIterator& sourceIt)
		{
			_sourceIt = sourceIt;
		}

		EncodingIterator()
		{
		}

		EncodingIterator& operator++()
		{
			++_sourceIt;
			return *this;
		}

		EncodingIterator operator++(int)
		{
			EncodingIterator oldVal(*this);

			operator++();

			return oldVal;			
		}

		EncodingIterator& operator--()
		{
			--_sourceIt;
			return *this;
		}

		EncodingIterator operator--(int)
		{
			EncodingIterator oldVal(*this);

			operator--();

			return oldVal;			
		}

		EncodedElement operator*()
		{
			return *_sourceIt;
		}

		bool operator==(const EncodingIterator& o) const
		{
			return (_sourceIt==o._sourceIt);
		}

		bool operator!=(const EncodingIterator& o) const
		{
			return !operator==(o);
		}


	protected:
		SourceIterator  _sourceIt;
	};




};


}

#endif


