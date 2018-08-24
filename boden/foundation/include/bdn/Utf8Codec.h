#ifndef BDN_Utf8Codec_H_
#define BDN_Utf8Codec_H_

#include <string>

namespace bdn
{


template<class INPUT_CATEGORY>
struct Utf8CodecImplIteratorCategory_
{
	using Type = std::input_iterator_tag;
};

template<>
struct Utf8CodecImplIteratorCategory_<std::forward_iterator_tag>
{
	using Type = std::forward_iterator_tag;
};

template<>
struct Utf8CodecImplIteratorCategory_<std::bidirectional_iterator_tag>
{
	using Type = std::bidirectional_iterator_tag;
};

template<>
struct Utf8CodecImplIteratorCategory_<std::random_access_iterator_tag>
{
	// when the source iterator is random access then the resulting
	// iterator is bidir (since we do not know the size of the encoded
	// characters ahead of time).
	using Type = std::bidirectional_iterator_tag;
};



/** The Utf-8 string codec.*/
class Utf8Codec
{
public:

	/** The encoded element type. For Utf-8 this is char.*/
	typedef char EncodedElement;


	/** The std string type for the encoded string.*/
	typedef std::basic_string<EncodedElement> EncodedString;


	/** Returns the maximum number of encoded elements that can be needed for a character.*/
	constexpr static int getMaxEncodedElementsPerCharacter()
	{
		return 6;
	}


    /** Decodes a single unicode character from the specified UTf-8 data.

        If the amount of data is empty ( it == endIt ) then the function returns
        0xfffd and does not modify the position of the \c it parameter.

        If the character was successfully decoded then the decoded character is returned
        and the \c it parameter is modified to point to the byte following the decoded Utf-8
        byte sequence.

        If the data is not a valid encoded utf-8 character sequence then the function
        returns the special unicode value 0xfffd and moves the \c it parameter
        a single byte ahead.
                    
        \param it reference to a byte iterator (returning uint8_t elements) pointing to the start
            of the encoded UTF-8 character. This iterator will be modified by the function - see above
            for an explanation of its position after the function completes.

        \param endIt an iterator pointing to the byte after the last one of the encoded data. This is NOT necessarily the
            end of the single character - it is the end of the total encoded data. This is specified
            for safety to ensure that the decode function will not overshoot the valid data
            in case of incorrectly encoded data.

        \return the decoded character or 0xfffd in case of an error (see above for more information).
    */
    template<typename IteratorType>
    static char32_t decodeChar(IteratorType& it, const IteratorType& endIt)
	{
        if(it==endIt)
            return 0xfffd;

		uint8_t firstByte = *it;

		++it;

        if((firstByte & 0x80)==0)
			return firstByte;
		else
		{
			int     mask = 0x40;
			bool    invalid = false;
			int     length=1;

			char32_t chr = 0;

            // safe this iterator position in case the encoded sequence is invalid.
            IteratorType secondByteIt = it;

			while(true)
			{
				if((firstByte & mask)==0)
				{
					// end of sequence
					break;
				}

				if(it==endIt)
				{
					// we should have one more byte, but we don't.
					invalid = true;
					break;
				}

				uint8_t val = *it;
				if((val & 0xc0)!=0x80)
				{
					// invalid sequence. We should abort right away, because the current
					// value could be part of a valid sequence that follows the broken one.
					invalid = true;
					break;
				}

				chr <<= 6;
				chr |= (val & 0x3f);

				mask >>= 1;

				++it;
				++length;
			}

			if(invalid || length==1 || length>6)
			{
				// use the unicode "replacement character".
				chr = 0xfffd;
				// only consume a single byte.
				it = secondByteIt;
			}
			else
			{
				chr |= (((char32_t)firstByte) & (mask-1)) << ((length-1)*6);
			}

            return chr;
		}
	}


	/** A character iterator that decodes UTF-8 data (char elements) from an
		arbitrary source iterator into Unicode characters (char32_t).

		The decoder handles invalid / corrupted sequences gracefully. When a bad
		sequence is encountered then each bad byte(!) is replaced by the Unicode
		"replacement character" (which has the value 0xfffd).
	*/
	template<class SourceIterator>
	class DecodingIterator
	{
	public:

		
		using iterator_category = typename Utf8CodecImplIteratorCategory_< typename std::iterator_traits<SourceIterator>::iterator_category >::Type;
		using value_type = char32_t;
		using difference_type = std::ptrdiff_t;
		using pointer = const char32_t*;

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
		// Note that defining reference as a value is allowed for input iterators,
		// but not for forward or bidirectional iterators, according to the standard.
		// However, input iterators are single pass only, so if we were to define our
		// category as input iterator then a lot of algorithms would be less efficient.
		// For example, in string construction the memory would be reallocated several times
		// because the string constructor thinks that it can only go over the data once.
		// So, to avoid that, we define our category as forward or bidir (depending on the source
		// iterator category) and break standard compliance by defining reference as a value.
		// In practice this is usually not a problem.
		using reference = char32_t;


		/** @param sourceIt the source iterator that provides the UTF-8 data.
			@param beginSourceIt an iterator that points to the beginning of the valid source data.
				The implementation uses this to avoid overshooting the data boundaries if the UTF-8 data is corrupted.
				This is often the same as sourceIt.
			@param endSourceIt an iterator that points to the end of the valid source data
				(the position after the last valid source element).
				The implementation uses this to avoid overshooting the data boundaries if the UTF-8 data is corrupted.
				*/
		DecodingIterator(const SourceIterator& sourceIt, const SourceIterator& beginSourceIt, const SourceIterator& endSourceIt)
		{
			_sourceIt = sourceIt;
			_beginSourceIt = beginSourceIt;
			_endSourceIt = endSourceIt;

			_chr = (char32_t)-1;
		}

		DecodingIterator()
		{
			_chr = (char32_t)-1;
		}


		DecodingIterator& operator++()
		{
			if(_chr==(char32_t)-1)
				decode();
			_sourceIt = _nextIt;
			_chr = (char32_t)-1;

			return *this;
		}

		DecodingIterator operator++(int)
		{
			DecodingIterator oldVal = *this;
			operator++();

			return oldVal;
		}

		DecodingIterator& operator--()
		{
			SourceIterator startIt = _sourceIt;

			// iterate backwards until we find the beginning of
			// a sequence.
			bool invalid = false;

			--_sourceIt;
			for(int i=0; ((*_sourceIt) & 0xc0)==0x80; i++ )
			{
				if(_sourceIt==_beginSourceIt || i==5)
				{
					// cannot advance further back. So we have an invalid
					// sequence at the start of our data.
					invalid = true;
					break;
				}

				--_sourceIt;
			}

			if(!invalid)
			{
				decode();

				if(_nextIt!=startIt)
					invalid = true;
			}

			if(invalid)
			{
				// the sequence was invalid and not all the bytes we skipped
				// over were consumed. When we decode the "next character" forwards from
				// here, the next bytes until the end of the invalid sequence
				// will be decoded as a single replacement character each.
				// Since we want decoding forwards and backwards to yield exactly the same
				// result we only go back a single byte.
				_nextIt = startIt;
				_sourceIt = startIt;
				--_sourceIt;

				_chr = 0xfffd;
			}

			return *this;
		}

		DecodingIterator operator--(int)
		{
			DecodingIterator oldVal = *this;
			operator--();

			return oldVal;
		}

		char32_t operator*() const
		{
			if(_chr==(char32_t)-1)
				decode();

			return _chr;
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
			The inner iterator points to the first encoded element of the character that the decoding
			iterator is currently pointing to.*/
		const SourceIterator& getInner() const
		{
			return _sourceIt;
		}

	protected:
		void decode() const
		{
			_nextIt = _sourceIt;

            _chr = Utf8Codec::decodeChar(_nextIt, _endSourceIt);			
		}


		SourceIterator  _sourceIt;
		SourceIterator  _beginSourceIt;
		SourceIterator  _endSourceIt;

		mutable char32_t		_chr;
		mutable SourceIterator  _nextIt;
	};


	/** A decoding iterator that works on the iterator type of the encoded standard string type
		#EncodedString.*/
	typedef DecodingIterator<EncodedString::const_iterator> DecodingStringIterator;


	/** Encodes unicode characters (char32_t) from an arbitary source iterator to UTF-8 (char elements).*/
	template<class SourceIterator>
	class EncodingIterator
	{
	public:
		
		using iterator_category = typename Utf8CodecImplIteratorCategory_< typename std::iterator_traits<SourceIterator>::iterator_category >::Type;
		using value_type = char;
		using difference_type = std::ptrdiff_t;
		using pointer = const char*;
		using reference = char;


		EncodingIterator(const SourceIterator& sourceIt)
		{
			_sourceIt = sourceIt;

			// we cannot call encode yet, because sourceIt might be
			// at an invalid position (i.e. end()). So instead we use an
			// invalid offset and encode the first time we dereference.
			_offset=-1;
		}

		EncodingIterator()
		{
			_offset = -1;
		}


		EncodingIterator& operator++()
		{
			if(_offset==-1)
				encode();

			++_offset;
			if(_offset>6)
			{
				++_sourceIt;
				_offset=-1;
			}

			return *this;
		}

		EncodingIterator operator++(int)
		{
			EncodingIterator oldVal = *this;

			operator++();

			return oldVal;
		}

		EncodingIterator& operator--()
		{
			if(_offset==-1 || _encoded[_offset-1]==0xff)
			{
				// _offset==-1 means that this is the starting position
				// of the iterator, i.e. the current position is the first byte of the first character..
				// _encoded[_offset-1]==0xff means that the current byte is the
				// first byte of the encoded sequence.

				_sourceIt--;
				encode();

				// move to the last byte of the sequence. Note that that is always index 6
				// (the first byte offset changes depending on the sequence size - the end index stays the same).
				_offset = 6;
			}
			else
				_offset--;

			return *this;
		}

		EncodingIterator operator--(int)
		{
			EncodingIterator oldVal = *this;

			operator--();

			return oldVal;
		}

		char operator*() const
		{
			if(_offset==-1)
				encode();

			return (char)_encoded[_offset];
		}

		/** Returns true if the iterator points to the last byte of a encoded UTF-8 character byte sequence.
		*/
		bool isEndOfCharacter() const
		{
			if(_offset==-1)
				encode();

			return (_offset==6);
		}

		bool operator==(const EncodingIterator& o) const
		{
			if(_sourceIt!=o._sourceIt)
				return false;

			if(_offset==o._offset)
				return true;

			// -1 means that the iterator is at the beginning
			// of the character, but has not encoded it yet.
			// So it means "first byte of encoded character".
			// Which actual valid _offset value -1 ends up representing
			// is unknown, since we store the encoded character in
			// at the end of our encoded array.
			if(_offset==-1)
			{
				// We know that o._offset is not -1 (otherwise we would have already returned true).
				// See if o is at the first byte.
				return (o._encoded[o._offset-1]==0xff);
			}
			else if(o._offset==-1)
			{
				// We know that o._offset is not -1 (otherwise we would have already returned true).
				// See if o is at the first byte.
				return (_encoded[_offset-1]==0xff);
			}

			return false;
		}

		bool operator!=(const EncodingIterator& o) const
		{
			return !operator==(o);
		}


	protected:
		void encode() const
		{
			char32_t chr = *_sourceIt;

			if(chr>0x7FFFFFFF)
			{
				// invalid unicode character. Use 'replacement character'
				chr = 0xfffd;
			}

			_offset=6;

			if(chr<=0x7f)
			{
				// ascii range
				_encoded[_offset] = (uint8_t)chr;
			}
			else
			{
				int firstBytePayloadMask = 0x3f;

				do
				{
					firstBytePayloadMask >>= 1;

					_encoded[_offset] = (uint8_t)(0x80 | (chr & 0x3f));

					chr>>=6;
					_offset--;
				}
				while((chr & firstBytePayloadMask)!=chr);

				_encoded[_offset] = (uint8_t) ( ((~firstBytePayloadMask)<<1 ) | (chr & firstBytePayloadMask) );
			}

			// end marker
			_encoded[_offset-1] = 0xff;
		}


		SourceIterator  _sourceIt;
		mutable int     _offset = 0;
		mutable uint8_t	_encoded[7];

	};




};


}

#endif
