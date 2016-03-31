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
		const std::u32string    utf32;
		const char*		        desc;
	};

	SubTestData allData[] = {	{ U"", "empty" },
                                // note that gcc has a bug. \u0000 is represented as 1, not 0.
                                // Use \0 instead.
								{ std::u32string(U"\0", 1), "zero char" },
								{ U"h", "ascii char" },
								{ U"hx", "ascii 2 chars" },
								{ U"\U00024B62", "non-ascii" },
								{ U"\U00010437\U00024B62", "2 non-ascii" },
							};

	int dataCount = std::extent<decltype(allData)>().value;
	for(int dataIndex=0; dataIndex<dataCount; dataIndex++)
	{
		SubTestData* pCurrData = &allData[dataIndex];

		SECTION( pCurrData->desc )
		{
			std::u32string data(pCurrData->utf32);

			// Utf 32 decoding and encoding is very straight forward. Both decoder and encoder
			// should simply pass each character through

			SECTION("decoding")
				testCodecDecodingIterator< Utf32Codec >(data, data);

			SECTION("encoding")
				testCodecEncodingIterator< Utf32Codec >(data, data);
		}
	}
}




}


