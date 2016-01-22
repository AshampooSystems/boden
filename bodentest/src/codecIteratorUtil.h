#ifndef BDN_codecIteratorUtil_H_
#define BDN_codecIteratorUtil_H_


template<class CODEC, class ENCSTRING>
void testCodecDecodingIterator(const ENCSTRING& encoded, const std::u32string& expectedDecoded )
{
	typename CODEC::DecodingIterator<typename ENCSTRING::const_iterator> begin(encoded.begin(), encoded.begin(), encoded.end());
	typename CODEC::DecodingIterator<typename ENCSTRING::const_iterator> end(encoded.end(), encoded.begin(), encoded.end());

	// forward then backward iteration
	{
		typename CODEC::DecodingIterator<typename ENCSTRING::const_iterator> it = begin;

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
		typename CODEC::DecodingIterator<typename ENCSTRING::const_iterator> it = end;

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
	typename CODEC::EncodingIterator<std::u32string::const_iterator> begin(input.begin());
	typename CODEC::EncodingIterator<std::u32string::const_iterator> end(input.end());

	// forward then backward iteration
	{
		typename CODEC::EncodingIterator<std::u32string::const_iterator> it = begin;

		for( auto expectedIt = expectedEncoded.begin(); expectedIt!=expectedEncoded.end(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, end, false) );

			REQUIRE( *it == *expectedIt );
			it++;
		}

		REQUIRE( checkEquality(it, end, true) );

		for( auto expectedIt = expectedEncoded.rbegin(); expectedIt!=expectedEncoded.rend(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, begin, false) );
			it--;

			REQUIRE( *it == *expectedIt );
		}

		REQUIRE( checkEquality(it, begin, true) );
	}


	// backward then forward iteration
	{
		typename CODEC::EncodingIterator<std::u32string::const_iterator> it = end;

		for( auto expectedIt = expectedEncoded.rbegin(); expectedIt!=expectedEncoded.rend(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, begin, false) );
			it--;

			REQUIRE( *it == *expectedIt );
		}

		REQUIRE( checkEquality(it, begin, true) );

		for( auto expectedIt = expectedEncoded.begin(); expectedIt!=expectedEncoded.end(); ++expectedIt)
		{
			REQUIRE( checkEquality(it, end, false) );
			REQUIRE( *it == *expectedIt );
			it++;
		}

		REQUIRE( checkEquality(it, end, true) );
	}
}


#endif
