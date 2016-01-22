#include <bdn/init.h>
#include <bdn/WcharCodec.h>

#include <bdn/test.h>

#include "codecIteratorUtil.h"

namespace bdn
{

TEST_CASE( "WcharCodec", "[string]" )
{
	struct SubTestData
	{
		const wchar_t*	wdata;
		const char32_t*	expectedDecoded;
		const char*		desc;
	};

	// we do not know if wchar_t is UTF-16 or UTF-32. So we only test some generic strings.
	// We cannote really test incorrect encoding without knowledge of the encoding.
	// Note that this is not a problem because WcharCodec is only a typedef for one
	// of the other codecs, which are all tested more in-depth.

	SubTestData allData[] = {	{ L"", U"", "empty" },
								{ L"\u0000", U"\u0000", "zero char" },
								{ L"h", U"h", "ascii char" },
								{ L"hx", U"hx", "ascii 2 chars" },
								{ L"\u0345", U"\u0345", "non-ascii below surrogate range" },
								{ L"\U00010437", U"\U00010437", "surrogate range A" },
								{ L"\U00024B62", U"\U00024B62", "surrogate range B" },
								{ L"\uE000", U"\uE000", "above surrogate range A" },
								{ L"\uF123", U"\uF123", "above surrogate range B" },
								{ L"\uFFFF", U"\uFFFF", "above surrogate range C" },
	};

	int dataCount = std::extent<decltype(allData)>().value;

	SubTestData* pCurrData = GENERATE( between( allData, &allData[dataCount-1] ) );

	std::wstring	encoded(pCurrData->wdata);
	std::u32string	expectedDecoded(pCurrData->expectedDecoded);

	SECTION("decoding")
	{
		SECTION(pCurrData->desc)
		{	
			testCodecDecodingIterator< WcharCodec >(encoded, expectedDecoded);
		}
	}

	SECTION("encoding")
	{
		SECTION(pCurrData->desc)
		{	
			testCodecEncodingIterator< WcharCodec >(expectedDecoded, encoded);
		}
	}
}


}


