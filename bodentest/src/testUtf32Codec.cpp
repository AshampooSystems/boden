#include <bdn/init.h>
#include <bdn/Utf32Codec.h>

#include <bdn/test.h>

namespace bdn
{

TEST_CASE( "Utf32Codec", "[string]" )
{
	struct SubTestData
	{
		const char32_t* utf32;
		const char*		desc;
	};

	SubTestData allData[] = {	{ U"", "empty" },
								{ U"\u0000", "zero char" },
								{ U"h", "ascii char" },
								{ U"hx", "ascii 2 chars" },
								{ U"\U00024B62", "non-ascii" },
								{ U"\U00010437\U00024B62", "2 non-ascii" },
							};

	int dataCount = std::extent<decltype(allData)>().value;

	SubTestData* pCurrData = GENERATE( between( allData, &allData[dataCount-1] ) );

	std::u32string data(pCurrData->utf32);

	// Utf 32 decoding and encoding is very straight forward. Both decoder and encoder
	// should simply pass each character through

	SECTION("encoding")
	{
		SECTION(pCurrData->desc)
		{
			Utf32Codec<char32_t>::EncodingIterator<std::u32string::iterator> begin(data.begin());
			Utf32Codec<char32_t>::EncodingIterator<std::u32string::iterator> end(data.end());

			// forward then backward iteration
			{
				Utf32Codec<char32_t>::EncodingIterator<std::u32string::iterator> it = begin;

				for( auto expectedIt = data.begin(); expectedIt!=data.end(); ++expectedIt)
				{
					REQUIRE( checkEquality(it, end, false) );

					REQUIRE( *it == *expectedIt );
					it++;
				}

				REQUIRE( checkEquality(it, end, true) );

				for( auto expectedIt = data.rbegin(); expectedIt!=data.rend(); ++expectedIt)
				{
					REQUIRE( checkEquality(it, begin, false) );
					it--;

					REQUIRE( *it == *expectedIt );
				}

				REQUIRE( checkEquality(it, begin, true) );
			}
		}
	}

	SECTION("decoding")
	{
		SECTION(pCurrData->desc)
		{
			Utf32Codec<char32_t>::DecodingIterator<std::u32string::iterator> begin(data.begin(), data.begin(), data.end());
			Utf32Codec<char32_t>::DecodingIterator<std::u32string::iterator> end(data.end(), data.begin(), data.end());

			// forward then backward iteration
			{
				Utf32Codec<char32_t>::DecodingIterator<std::u32string::iterator> it = begin;

				for( auto expectedIt = data.begin(); expectedIt!=data.end(); ++expectedIt)
				{
					REQUIRE( checkEquality(it, end, false) );
					REQUIRE( *it == *expectedIt );
					it++;
				}

				REQUIRE( checkEquality(it, end, true) );

				for( auto expectedIt = data.rbegin(); expectedIt!=data.rend(); ++expectedIt)
				{
					REQUIRE( checkEquality(it, begin, false) );
					it--;

					REQUIRE( *it == *expectedIt );
				}

				REQUIRE( checkEquality(it, begin, true) );
			}


			// backward then forward iteration
			{
				Utf32Codec<char32_t>::DecodingIterator<std::u32string::iterator> it = end;

				for( auto expectedIt = data.rbegin(); expectedIt!=data.rend(); ++expectedIt)
				{
					REQUIRE( checkEquality(it, begin, false) );
					it--;

					REQUIRE( *it == *expectedIt );
				}

				REQUIRE( checkEquality(it, begin, true) );

				for( auto expectedIt = data.begin(); expectedIt!=data.end(); ++expectedIt)
				{
					REQUIRE( checkEquality(it, end, false) );
					REQUIRE( *it == *expectedIt );
					it++;
				}

				REQUIRE( checkEquality(it, end, true) );
			}
		}
	}
}




}


