#ifndef BDN_Utf32CodecImpl_H_
#define BDN_Utf32CodecImpl_H_

#include <string>

namespace bdn
{

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
	static int getMaxEncodedElementsPerCharacter()
	{
		return 1;
	}


	/** A character iterator that decodes UTF-32 data into Unicode characters (also char32_t).

		Implementation note:
		This does not actually do any real decoding work, since UTF-32 stores all characters in a plain 1:1 way.
		So this class simply passes the UTF-32 elements (=characters) through.
	*/
	template<class SourceIterator>
	class DecodingIterator : public std::iterator<	std::bidirectional_iterator_tag,
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
													char32_t >
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
	class EncodingIterator : public std::iterator<std::bidirectional_iterator_tag,
													EncodedElement,
													std::ptrdiff_t,
													EncodedElement*,
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
													EncodedElement >
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

		EncodedElement operator*() const
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


