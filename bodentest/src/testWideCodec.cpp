#include <bdn/init.h>
#include <bdn/WideCodec.h>

#include <bdn/test.h>

#include "codecIteratorUtil.h"

namespace bdn
{

TEST_CASE( "WideCodecType", "[string]" )
{
#if BDN_PLATFORM_WINDOWS || BDN_PLATFORM_DOTNET
    REQUIRE( typeid(WideCodec) == typeid(Utf16CodecImpl<wchar_t>) );

#else
    REQUIRE( typeid(WideCodec) == typeid(Utf32CodecImpl<wchar_t>) );

#endif
}


TEST_CASE( "WideCodec", "[string]" )
{
	struct SubTestData
	{
		std::wstring	wdata;
		std::u32string	expectedDecoded;
		const char*		desc;
	};

	// we do not know if wchar_t is UTF-16 or UTF-32. So we only test some generic strings.
	// We cannote really test incorrect encoding without knowledge of the encoding.
	// Note that this is not a problem because WideCodec is only a typedef for one
	// of the other codecs, which are all tested more in-depth.

	SubTestData allData[] = {	{ L"", U"", "empty" },
                                // note that gcc has a bug. \u0000 is represented as 1, not 0.
                                // Use \0 instead.
								{ std::wstring(L"\0", 1), std::u32string(U"\0", 1), "zero char" },
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

	for(int dataIndex=0; dataIndex<dataCount; dataIndex++)
	{
		SubTestData* pCurrData = &allData[dataIndex];

		SECTION( pCurrData->desc )
		{
			std::wstring	encoded(pCurrData->wdata);
			std::u32string	expectedDecoded(pCurrData->expectedDecoded);

			SECTION("decoding")
				testCodecDecodingIterator< WideCodec >(encoded, expectedDecoded);

			SECTION("encoding")
				testCodecEncodingIterator< WideCodec >(expectedDecoded, encoded);
		}
	}
}


}


