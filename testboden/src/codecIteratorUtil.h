#ifndef BDN_codecIteratorUtil_H_
#define BDN_codecIteratorUtil_H_


template<class CODEC, class ENCSTRING>
void testCodecDecodingIterator(const ENCSTRING& encoded, const std::u32string& expectedDecoded )
{
	typename CODEC::template DecodingIterator<typename ENCSTRING::const_iterator> begin(encoded.begin(), encoded.begin(), encoded.end());
	typename CODEC::template DecodingIterator<typename ENCSTRING::const_iterator> end(encoded.end(), encoded.begin(), encoded.end());

	// forward then backward iteration
	{
		typename CODEC::template DecodingIterator<typename ENCSTRING::const_iterator> it = begin;

		for( auto expectedIt = expectedDecoded.begin(); expectedIt!=expectedDecoded.end(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, end, false) );
			REQUIRE( *it == *expectedIt );
			it++;
		}

		REQUIRE( checkEquality(it, end, true) );

		for( auto expectedIt = expectedDecoded.rbegin(); expectedIt!=expectedDecoded.rend(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, begin, false) );
			it--;

			REQUIRE( *it == *expectedIt );
		}

		REQUIRE( checkEquality(it, begin, true) );
	}


	// backward then forward iteration
	{
		typename CODEC::template DecodingIterator<typename ENCSTRING::const_iterator> it = end;

		for( auto expectedIt = expectedDecoded.rbegin(); expectedIt!=expectedDecoded.rend(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, begin, false) );
			it--;

			REQUIRE( *it == *expectedIt );
		}

		REQUIRE( checkEquality(it, begin, true) );

		for( auto expectedIt = expectedDecoded.begin(); expectedIt!=expectedDecoded.end(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, end, false) );
			REQUIRE( *it == *expectedIt );
			it++;
		}

		REQUIRE( checkEquality(it, end, true) );
	}
}


template <class CODEC, class ENCSTRING>
void testCodecEncodingIterator(const std::u32string& input, const ENCSTRING& expectedEncoded)
{
	typename CODEC::template EncodingIterator<std::u32string::const_iterator> begin(input.begin());
	typename CODEC::template EncodingIterator<std::u32string::const_iterator> end(input.end());

	// forward then backward iteration
	{
		typename CODEC::template EncodingIterator<std::u32string::const_iterator> it = begin;

		bool lastWasEndOfCharacter = true;  // we want this to be true for empty strings.
		int  endOfCharacterCount=0;
		for( auto expectedIt = expectedEncoded.begin(); expectedIt!=expectedEncoded.end(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, end, false) );

			REQUIRE( *it == *expectedIt );

			lastWasEndOfCharacter = it.isEndOfCharacter();
			if(lastWasEndOfCharacter)
				endOfCharacterCount++;

			it++;
		}

		REQUIRE( checkEquality(it, end, true) );

		REQUIRE( lastWasEndOfCharacter );
		REQUIRE( endOfCharacterCount==input.length() );

		bool firstElement = true;
		endOfCharacterCount = 0;

		for( auto expectedIt = expectedEncoded.rbegin(); expectedIt!=expectedEncoded.rend(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, begin, false) );
			it--;

			if(firstElement)
			{
				REQUIRE( it.isEndOfCharacter() );
				firstElement=false;
			}

			REQUIRE( *it == *expectedIt );

			if(it.isEndOfCharacter())
				endOfCharacterCount++;
		}

		REQUIRE( checkEquality(it, begin, true) );

		REQUIRE( endOfCharacterCount==input.length() );
	}


	// backward then forward iteration
	{
		typename CODEC::template EncodingIterator<std::u32string::const_iterator> it = end;

		bool firstElement = true;
		int endOfCharacterCount = 0;

		for( auto expectedIt = expectedEncoded.rbegin(); expectedIt!=expectedEncoded.rend(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, begin, false) );
			it--;

			if(firstElement)
			{
				REQUIRE( it.isEndOfCharacter() );
				firstElement=false;
			}

			REQUIRE( *it == *expectedIt );

			if(it.isEndOfCharacter())
				endOfCharacterCount++;
		}

		REQUIRE( checkEquality(it, begin, true) );

		REQUIRE( endOfCharacterCount==input.length() );

		endOfCharacterCount=0;

		bool lastWasEndOfCharacter = true; // we want this to be true for empty strings.
		for( auto expectedIt = expectedEncoded.begin(); expectedIt!=expectedEncoded.end(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, end, false) );

			REQUIRE( *it == *expectedIt );

			lastWasEndOfCharacter = it.isEndOfCharacter();
			if(lastWasEndOfCharacter)
				endOfCharacterCount++;

			it++;
		}

		REQUIRE( checkEquality(it, end, true) );

		REQUIRE( lastWasEndOfCharacter );
		REQUIRE( endOfCharacterCount==input.length() );
	}
}


#endif
