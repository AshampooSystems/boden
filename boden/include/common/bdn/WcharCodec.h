#ifndef BDN_WcharCodec_H_
#define BDN_WcharCodec_H_


#include <bdn/Utf16Codec.h>
#include <bdn/Utf32Codec.h>

namespace bdn
{

/** String codec that is used for wchar_t strings / std::wstring.

	This is usually either UTF-16 (if wchar_t is two bytes - i.e. on Windows)
	or UTF-32 (if wchar_t is four bytes - i.e. most other systems).

	Note that WcharCodec is actually implemented as a typedef to the actual
	codec (either Utf16Codec<wchar_t> or Utf32Codec<wchar_t>
*/
#ifdef BDN_GENERATING_DOCS

class WcharCodec
{
public:

	/** The encoded element type. This is a 16 bit integer type.*/
	typedef wchar_t EncodedElement;


	/** The std string type for the encoded string.*/
	typedef std::wstring EncodedString;


	/** A character iterator that decodes wchar data (wchar_t elements) from an
		arbitrary source iterator into Unicode characters (char32_t).
	*/
	template<class SourceIterator>
	class DecodingIterator : public std::iterator<std::bidirectional_iterator_tag, char32_t>
	{
	public:
		/** @param sourceIt the source iterator that provides the UTF-16 data.
			@param beginSourceIt an iterator that points to the beginning of the valid source data.
			The implementation uses this to avoid overshooting the data boundaries if the UTF-16 data is corrupted.
			This is often the same as sourceIt.
			@param endSourceIt an iterator that points to the end of the valid source data
			(the position after the last valid source element).
			The implementation uses this to avoid overshooting the data boundaries if the UTF-16 data is corrupted.
		*/
		DecodingIterator(const SourceIterator& sourceIt, const SourceIterator& beginSourceIt, const SourceIterator& endSourceIt);
		DecodingIterator();
		
		DecodingIterator& operator++();
		DecodingIterator operator++(int);

		DecodingIterator& operator--();
		DecodingIterator operator--(int);

		char32_t operator*();

		bool operator==(const DecodingIterator& o) const;
		bool operator!=(const DecodingIterator& o) const;
	};

	/** A decoding iterator that works on the iterator type of the encoded standard string type
		#EncodedString.*/
	typedef DecodingIterator<typename EncodedString::iterator> DecodingStringIterator;

	/** Encodes unicode characters to wchar_t encoding.*/
	template<class SourceIterator>
	class EncodingIterator : public std::iterator<std::bidirectional_iterator_tag, EncodedElement>
	{
	public:
		EncodingIterator(const SourceIterator& sourceIt);
		EncodingIterator();

		EncodingIterator& operator++();
		EncodingIterator operator++(int);

		EncodingIterator& operator--();
		EncodingIterator operator--(int);

		EncodedElement operator*();

		bool operator==(const EncodingIterator<SourceIterator>& o) const;
		bool operator!=(const EncodingIterator<SourceIterator>& o) const;
	};
};

#else

#if BDN_WCHAR_SIZE==2
typedef Utf16Codec<wchar_t> WcharCodec;

#elif BDN_WCHAR_SIZE==4
typedef Utf32Codec<wchar_t> WcharCodec;

#else
#error Unsupported wchar size

#endif

#endif


}


#endif
