#include <bdn/init.h>
#include <bdn/Utf8Codec.h>

#include <bdn/test.h>


#include "codecIteratorUtil.h"

namespace bdn
{

TEST_CASE( "Utf8Codec.decoding", "[string]" )
{
	struct SubTestData
	{
		const char*		utf8;
		const char32_t*	expectedDecoded;
		const char*		desc;
	};

	SubTestData allData[] = {	{ "", U"", "empty" },
								{ "\u0000", U"\u0000", "zero char" },
								{ "h", U"h", "ascii char" },
								{ "hi", U"hi", "ascii 2 chars" },
								{ u8"\u0181", U"\u0181", "2 byte" },								
								{ u8"\u0810", U"\u0810", "3 bytes"},
								{ u8"\U00010010", U"\U00010010", "4 bytes"},
								// C++ literals do not recognize characters in the 5 and 6 byte range.
								// So we specify the encoded version manually
								{ "\xF8\x88\x80\x80\x90", U"\x00200010", "5 bytes"},
								{ "\xFC\x84\x80\x80\x80\x90", U"\x04000010", "6 bytes"},

								{ "\xc2", U"\ufffd", "byte missing in 2 byte sequence" },
								{ "\xe2", U"\ufffd", "two bytes missing in 3 byte sequence" },
								{ "\xe2\x82", U"\ufffd\ufffd", "byte missing in 3 byte sequence" },
								{ "\x82\xa2", U"\ufffd\ufffd", "only top bit set in start byte" },
								{ "\xfe\xa2\xa2\xa2\xa2\xa2\xa2", U"\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd", "top 7 bits set in start byte" },
								{ "\xff\xa2\xa2\xa2\xa2\xa2\xa2\xa2", U"\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd\ufffd", "all bits set in start byte" },
								{ "\xc2hello", U"\ufffdhello", "byte missing at start of string" },
								{ "hello\xc2", U"hello\ufffd", "byte missing at end of string" },
								{ "hell\xc2o", U"hell\ufffdo", "byte missing in middle of string" },

								{ "\xE0\xA4\x92", U"\u0912", "valid 3 byte sequence" },
								{ "\xE0\xA4\x12", U"\ufffd\ufffd\u0012", "last byte ascii in 3 byte sequence" },
								{ "\xE0\xA4\xd2", U"\ufffd\ufffd\ufffd", "last byte two top bits set in 3 byte sequence" },
								{ "\xE0\x24\x92", U"\ufffd\u0024\ufffd", "middle byte ascii in 3 byte sequence" },
								{ "\xE0\xe4\x92", U"\ufffd\ufffd\ufffd", "middle byte two top bits set in 3 byte sequence" },

								{ "\xc2\xe0\xa0\x90\xc2", U"\ufffd\u0810\ufffd", "valid sequence sandwiched between bad sequences"},
	};

	int dataCount = std::extent<decltype(allData)>().value;

	SubTestData* pCurrData = GENERATE( between( allData, &allData[dataCount-1] ) );

	std::string encoded(pCurrData->utf8);
	std::u32string expectedDecoded(pCurrData->expectedDecoded);

	// start a section here so that we will know which subtest failed
	SECTION(pCurrData->desc)
	{
		testCodecDecodingIterator<Utf8Codec>(encoded, expectedDecoded);
	}
}



TEST_CASE( "Utf8Codec.encoding", "[string]" )
{
	struct SubTestData
	{		
		const char32_t*	input;
		const char*		expectedUtf8;
		const char*		desc;
	};

	SubTestData allData[] = {	{ U"", "", "empty" },		
								{ U"\u0000", "\x00", "zero char" },
								{ U"h", "h", "ascii char" },
								{ U"hi", "hi", "ascii 2 chars" },
								{ U"\u0181", u8"\u0181", "2 byte" },								
								{ U"\u0810", u8"\u0810", "3 bytes"},
								{ U"\U00010010", u8"\U00010010", "4 bytes"},
									// C++ literals do not recognize characters in the 5 and 6 byte range.
									// So we specify the encoded version manually
								{ U"\x00200010", "\xF8\x88\x80\x80\x90", "5 bytes"},
								{ U"\x04000010", "\xFC\x84\x80\x80\x80\x90", "6 bytes"},
								{ U"\x80000000", u8"\ufffd", "range exceeded"},
	};

	int dataCount = std::extent<decltype(allData)>().value;

	SubTestData* pCurrData = GENERATE( between( allData, &allData[dataCount-1] ) );

	std::u32string input(pCurrData->input);
	std::string expectedUtf8(pCurrData->expectedUtf8);

	// start a section here so that we will know which subtest failed
	SECTION(pCurrData->desc)
	{
		testCodecEncodingIterator<Utf8Codec>( input, expectedUtf8);		
	}
}


}


