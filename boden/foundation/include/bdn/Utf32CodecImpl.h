#ifndef BDN_Utf32CodecImpl_H_
#define BDN_Utf32CodecImpl_H_

#include <string>

namespace bdn
{



template<class INPUT_CATEGORY>
struct Utf32CodecImplIteratorCategory_
{
	using Type = std::input_iterator_tag;
};

template<>
struct Utf32CodecImplIteratorCategory_<std::forward_iterator_tag>
{
	using Type = std::forward_iterator_tag;
};

template<>
struct Utf32CodecImplIteratorCategory_<std::bidirectional_iterator_tag>
{
	using Type = std::bidirectional_iterator_tag;
};

template<>
struct Utf32CodecImplIteratorCategory_<std::random_access_iterator_tag>
{
	// we COULD implement a random access UTF-32 iterator. However,
	// for consistency with the other codecs we do not. We want the codecs
	// to be interchangeable, so we do not support random access for utf-32 either.
	using Type = std::bidirectional_iterator_tag;
};



/** Implements the Utf-32 string codec.

	\tparam ElementType specifies the type of the encoded elements. It
		must be a 32 bit integer type. Usually either char32_t or wchar_t (on systems
		where wchar_t is 32 bit). The default is char32_t.
*/
template<class ElementType = char32_t>
class Utf32CodecImpl
{
public:

	/** The encoded element type. This is a 32 bit integer type, defined by the template parameter ElementType.*/
	typedef ElementType EncodedElement;


	/** The std string type for the encoded string.*/
	typedef std::basic_string<EncodedElement> EncodedString;


	/** Returns the maximum number of encoded elements that can be needed for a character.*/
	constexpr static int getMaxEncodedElementsPerCharacter()
	{
		return 1;
	}


	/** A character iterator that decodes UTF-32 data into Unicode characters (also char32_t).

		Implementation note:
		This does not actually do any real decoding work, since UTF-32 stores all characters in a plain 1:1 way.
		So this class simply passes the UTF-32 elements (=characters) through.
	*/
	template<class SourceIterator>
	class DecodingIterator
	{
	public:

		// note: we could pretty much pass the traits of the source iterator
		// through directly. However, for consistency with the other codecs we do
		// not do this. We want the codecs to be interchangeable, so we restrict the
		// Utf32Codec to the same implementation that we use for the other codecs
		
		using iterator_category = typename Utf32CodecImplIteratorCategory_< typename std::iterator_traits<SourceIterator>::iterator_category >::Type;
		using value_type = char32_t;
		using difference_type = std::ptrdiff_t;
		using pointer = const char32_t*;
		using reference = char32_t;


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

		char32_t operator*() const
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


		/** Returns an iterator to the inner encoded string that the decoding iterator is working on.
			The inner iterator points to the first encoded element of the character, that the decoding
			iterator is currently pointing to.*/
		const SourceIterator& getInner() const
		{
			return _sourceIt;
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
	class EncodingIterator
	{
	public:
		
		using iterator_category = typename Utf32CodecImplIteratorCategory_< typename std::iterator_traits<SourceIterator>::iterator_category >::Type;
		using value_type = EncodedElement;
		using difference_type = std::ptrdiff_t;
		using pointer = const EncodedElement*;
		using reference = EncodedElement;


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

		EncodedElement operator*() const
		{
			return *_sourceIt;
		}

		/** Returns true if a character ends with the current encoded element.
			Since UTF-32 encodes every character as exactly one element this always
			returns true.*/
		bool isEndOfCharacter() const
		{
			return true;
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


