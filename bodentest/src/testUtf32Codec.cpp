#include <bdn/init.h>
#include <bdn/Utf32Codec.h>

#include <bdn/test.h>

#include "codecIteratorUtil.h"

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

	SECTION("decoding")
	{
		SECTION(pCurrData->desc)
		{
			testCodecDecodingIterator< Utf32Codec >(data, data);
		}
	}

	SECTION("encoding")
	{
		SECTION(pCurrData->desc)
		{
			testCodecEncodingIterator< Utf32Codec >(data, data);
		}
	}
}




}


