#include <bdn/init.h>
#include <bdn/Utf16Codec.h>

#include <bdn/test.h>

namespace bdn
{

TEST_CASE( "Utf16Codec.decoding", "[string]" )
{
	struct SubTestData
	{
		const char16_t*	utf16;
		const char32_t*	expectedDecoded;
		const char*		desc;
	};

	SubTestData allData[] = {	{ u"", U"", "empty" },
								{ u"\u0000", U"\u0000", "zero char" },
								{ u"h", U"h", "ascii char" },
								{ u"hx", U"hx", "ascii 2 chars" },
								{ u"\u0345", U"\u0345", "non-ascii below surrogate range" },
								{ u"\U00010437", U"\U00010437", "surrogate pair A" },
								{ u"\U00024B62", U"\U00024B62", "surrogate pair B" },
								{ u"\uE000", U"\uE000", "above surrogate range A" },
								{ u"\uF123", U"\uF123", "above surrogate range B" },
								{ u"\uFFFF", U"\uFFFF", "above surrogate range C" },
								
								{ u"\xD801", U"\ufffd", "only high surrogate" },
								{ u"\xDC37", U"\ufffd", "only low surrogate" },

								{ u"\xD801\U00024B62", U"\ufffd\U00024B62", "high surrogate at start, pair follows" },
								{ u"\xD801\u0345", U"\ufffd\u0345", "high surrogate at start, single follows" },
								{ u"\U00024B62\xD801", U"\U00024B62\ufffd", "pair, high surrogate at end" },
								{ u"\u0345\xD801", U"\u0345\ufffd", "single, high surrogate at end" },
								{ u"\U00024B62\xD801\U00010437", U"\U00024B62\ufffd\U00010437", "high surrogate between pairs" },
								{ u"\u0345\xD801\u0567", U"\u0345\ufffd\u0567", "high surrogate between singles" },

								{ u"\xDC37\U00024B62", U"\ufffd\U00024B62", "low surrogate at start, pair follows" },
								{ u"\xDC37\u0345", U"\ufffd\u0345", "low surrogate at start, single follows" },
								{ u"\U00024B62\xDC37", U"\U00024B62\ufffd", "pair, low surrogate at end" },
								{ u"\u0345\xDC37", U"\u0345\ufffd", "single, low surrogate at end" },
								{ u"\U00024B62\xDC37\U00010437", U"\U00024B62\ufffd\U00010437", "low surrogate between pairs" },
								{ u"\u0345\xDC37\u0567", U"\u0345\ufffd\u0567", "low surrogate between singles" },
	};

	int dataCount = std::extent<decltype(allData)>().value;

	SubTestData* pCurrData = GENERATE( between( allData, &allData[dataCount-1] ) );

	std::u16string encoded(pCurrData->utf16);
	std::u32string expectedDecoded(pCurrData->expectedDecoded);

	// start a section here so that we will know which subtest failed
	SECTION(pCurrData->desc)
	{
		Utf16Codec<char16_t>::DecodingIterator<std::u16string::iterator> begin(encoded.begin(), encoded.begin(), encoded.end());
		Utf16Codec<char16_t>::DecodingIterator<std::u16string::iterator> end(encoded.end(), encoded.begin(), encoded.end());

		// forward then backward iteration
		{
			Utf16Codec<char16_t>::DecodingIterator<std::u16string::iterator> it = begin;

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
			Utf16Codec<char16_t>::DecodingIterator<std::u16string::iterator> it = end;

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
}



TEST_CASE( "Utf16Codec.encoding", "[string]" )
{
	struct SubTestData
	{		
		const char32_t*	input;
		const char16_t*	expectedUtf16;
		const char*		desc;
	};

	SubTestData allData[] = {	{ U"", u"", "empty" },		
								{ U"\u0000", u"\x00", "zero char" },
								{ U"h", u"h", "ascii char" },
								{ U"hx", u"hx", "ascii 2 chars" },
								{ U"\u0181", u"\u0181", "non-ascii single" },								
								{ U"\u0181\u0810", u"\u0181\u0810", "two singles" },
								{ U"\U0000e000", u"\U0000e000", "single above surrogate range A" },
								{ U"\U0000ffff", u"\U0000ffff", "single above surrogate range B" },

								// MSVC in Visual Studio 2015 has a bug that causes it to NOT encode
								// U00010000 as a pair. Instead it encodes it as 0x0001 0x0000.
								// So we have to specify the encoded version manually
								{ U"\U00010000", u"\xd800\xdc00", "pair A" },

								{ U"\U00054321", u"\U00054321", "pair B" },

								{ U"\U0010FFFF", u"\U0010FFFF", "pair C" },

								{ U"\xd800", u"\ufffd", "high surrogate A" },
								{ U"\xdbff", u"\ufffd", "high surrogate B" },
								{ U"\xdc00", u"\ufffd", "low surrogate A" },
								{ U"\xdfff", u"\ufffd", "low surrogate B" },

								{ U"\x00110000", u"\ufffd", "unencodable A" },
								{ U"\x00234567", u"\ufffd", "unencodable B" },
								{ U"\xffffffff", u"\ufffd", "unencodable C" },
	};

	int dataCount = std::extent<decltype(allData)>().value;

	SubTestData* pCurrData = GENERATE( between( allData, &allData[dataCount-1] ) );

	std::u32string input(pCurrData->input);
	std::u16string expectedUtf16(pCurrData->expectedUtf16);

	// start a section here so that we will know which subtest failed
	SECTION(pCurrData->desc)
	{
		Utf16Codec<char16_t>::EncodingIterator<std::u32string::iterator> begin(input.begin());
		Utf16Codec<char16_t>::EncodingIterator<std::u32string::iterator> end(input.end());

		// forward then backward iteration
		{
			Utf16Codec<char16_t>::EncodingIterator<std::u32string::iterator> it = begin;

			for( auto expectedIt = expectedUtf16.begin(); expectedIt!=expectedUtf16.end(); ++expectedIt)
			{
				REQUIRE( checkEquality(it, end, false) );
				REQUIRE( *it == *expectedIt );
				it++;
			}

			REQUIRE( checkEquality(it, end, true) );

			for( auto expectedIt = expectedUtf16.rbegin(); expectedIt!=expectedUtf16.rend(); ++expectedIt)
			{
				REQUIRE( checkEquality(it, begin, false) );
				it--;

				REQUIRE( *it == *expectedIt );
			}

			REQUIRE( checkEquality(it, begin, true) );		
		}


		// backward then forward iteration
		{
			Utf16Codec<char16_t>::EncodingIterator<std::u32string::iterator> it = end;

			for( auto expectedIt = expectedUtf16.rbegin(); expectedIt!=expectedUtf16.rend(); ++expectedIt)
			{
				REQUIRE( checkEquality(it, begin, false) );
				it--;

				REQUIRE( *it == *expectedIt );
			}

			REQUIRE( checkEquality(it, begin, true) );

			for( auto expectedIt = expectedUtf16.begin(); expectedIt!=expectedUtf16.end(); ++expectedIt)
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


