#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Utf16StringData.h>
#include <bdn/Utf32StringData.h>

using namespace bdn;



template<class STRINGIMPL>
inline void verifyContents(STRINGIMPL& s, const std::u32string& expectedResult)
{
	REQUIRE( checkEquality(s.begin(), s.end(), expectedResult.empty() ) );

	std::u32string result( s.begin(), s.end() );
	REQUIRE( result==expectedResult );

	// also test reverse iteration
	std::u32string	reverseResult;
	auto			it = s.end();

	while(it!=s.begin())
	{
		it--;
		reverseResult += *it;
	}

	std::u32string expectedReverseResult = expectedResult;
	std::reverse( expectedReverseResult.begin(), expectedReverseResult.end() );

	REQUIRE( reverseResult==expectedReverseResult );
}

template<class DATATYPE>
void testConstruct()
{
	SECTION("empty")
	{
		StringImpl<DATATYPE> s;

		verifyContents(s, U"");
	}

	SECTION("fromUTF8-noLength")
	{
		StringImpl<DATATYPE> s("hello");

		verifyContents(s, U"hello");
	}

	SECTION("fromUTF8-withLength")
	{
		StringImpl<DATATYPE> s("helloxyz", 5);
		verifyContents(s, U"hello");
	}

	SECTION("fromUTF8-stdString")
	{
		StringImpl<DATATYPE> s( std::string("hello") );
		verifyContents(s, U"hello");
	}

	SECTION("fromUTF16-noLength")
	{
		StringImpl<DATATYPE> s(u"hello");
		verifyContents(s, U"hello");
	}

	SECTION("fromUTF16-withLength")
	{
		StringImpl<DATATYPE> s(u"helloxyz", 5);
		verifyContents(s, U"hello");
	}

	SECTION("fromUTF16-stdString")
	{
		StringImpl<DATATYPE> s( std::u16string(u"hello") );
		verifyContents(s, U"hello");
	}

	SECTION("fromUTF32-noLength")
	{
		StringImpl<DATATYPE> s(U"hello");
		verifyContents(s, U"hello");
	}

	SECTION("fromUTF32-withLength")
	{
		StringImpl<DATATYPE> s(U"helloxyz", 5);
		verifyContents(s, U"hello");
	}

	SECTION("fromUTF32-stdString")
	{
		StringImpl<DATATYPE> s( std::u32string(U"hello") );
		verifyContents(s, U"hello");
	}


	SECTION("fromUnicodeIterators")
	{
		std::u32string input = U"hello";

		StringImpl<DATATYPE> s( input.begin(), input.end() );
		verifyContents(s, U"hello");
	}


	SECTION("fromEncodedIterators-Utf8")
	{
		std::string input = "hello";

		StringImpl<DATATYPE> s(Utf8Codec(), input.begin(), input.end() );
		verifyContents(s, U"hello");
	}

	SECTION("fromEncodedIterators-Utf16")
	{
		std::u16string input = u"hello";

		StringImpl<DATATYPE> s(Utf16Codec<char16_t>(), input.begin(), input.end() );
		verifyContents(s, U"hello");
	}

	SECTION("fromEncodedIterators-Utf32")
	{
		std::u32string input = U"hello";

		StringImpl<DATATYPE> s(Utf32Codec<char32_t>(), input.begin(), input.end() );
		verifyContents(s, U"hello");
	}


	SECTION("fromEncodedPtrs-Utf8")
	{
		const char* input = "hello";

		StringImpl<DATATYPE> s(Utf8Codec(), input, input+5 );
		verifyContents(s, U"hello");
	}

	SECTION("fromEncodedPtrs-Utf16")
	{
		const char16_t* input = u"hello";

		StringImpl<DATATYPE> s(Utf16Codec<char16_t>(), input, input+5 );
		verifyContents(s, U"hello");
	}

	SECTION("fromEncodedPtrs-Utf32")
	{
		const char32_t* input = U"hello";

		StringImpl<DATATYPE> s(Utf32Codec<char32_t>(), input, input+5 );
		verifyContents(s, U"hello");
	}

}


template<class DATATYPE>
inline void testLength()
{
	SECTION("nonEmpty")
	{
		SECTION("initWithLiteral")
		{
			StringImpl<DATATYPE> s("helloworld");

			REQUIRE( s.getLength()==10 );
			REQUIRE( s.length()==10 );
			REQUIRE( s.size()==10 );
		}

		SECTION("initWithLiteralAndLength")
		{
			StringImpl<DATATYPE> s("helloworld", 7);

			REQUIRE( s.getLength()==7 );
			REQUIRE( s.length()==7 );
			REQUIRE( s.size()==7 );
		}

		SECTION("initWithIterators")
		{
			StringImpl<DATATYPE> source("helloworld");

			StringImpl<DATATYPE> s( source.begin(), source.end() );

			REQUIRE( s.getLength()==10 );
			REQUIRE( s.length()==10 );
			REQUIRE( s.size()==10 );
		}
	}

	SECTION("empty")
	{
		SECTION("defaultConstruct")
		{
			StringImpl<DATATYPE> s;

			REQUIRE(s.getLength()==0);
			REQUIRE( s.length()==0 );
			REQUIRE( s.size()==0 );
		}

		SECTION("initWithLiteral")
		{
			StringImpl<DATATYPE> s("");

			REQUIRE(s.getLength()==0);
			REQUIRE( s.length()==0 );
			REQUIRE( s.size()==0 );
		}
	}
}


template<class DATATYPE>
inline void testSubString()
{
	StringImpl<DATATYPE> s("helloworld");

	SECTION("empty")
	{
		SECTION("atStart")
		{
			StringImpl<DATATYPE> s2 = s.subString(0, 0);
			REQUIRE( s2=="" );				

			StringImpl<DATATYPE> s3 = s.subString(s.begin(), s.begin() );
			REQUIRE( s3=="" );

			StringImpl<DATATYPE> s4 = s.substr(0, 0);
			REQUIRE( s4=="" );
		}

		SECTION("atLast")
		{
			StringImpl<DATATYPE> s2 = s.subString(9, 0);
			REQUIRE( s2=="" );

			StringImpl<DATATYPE> s3 = s.subString( s.end()-1, s.end()-1);
			REQUIRE( s3=="" );

			StringImpl<DATATYPE> s4 = s.substr(9, 0);
			REQUIRE( s4=="" );
		}

		SECTION("atEnd")
		{
			StringImpl<DATATYPE> s2 = s.subString(10, 0);
			REQUIRE( s2=="" );

			StringImpl<DATATYPE> s3 = s.subString( s.end(), s.end() );
			REQUIRE( s3=="" );

			StringImpl<DATATYPE> s4 = s.substr(10, 0);
			REQUIRE( s4=="" );
		}

		SECTION("inMiddle")
		{
			StringImpl<DATATYPE> s2 = s.subString(5, 0);
			REQUIRE( s2=="" );

			StringImpl<DATATYPE> s3 = s.subString( s.begin()+5, s.begin()+5 );
			REQUIRE( s3=="" );

			StringImpl<DATATYPE> s4 = s.substr(5, 0);
			REQUIRE( s4=="" );
		}
	}

	SECTION("full")
	{
		SECTION("explicitLength")
		{
			StringImpl<DATATYPE> s2 = s.subString(0, 10);
			REQUIRE( s2=="helloworld" );

			StringImpl<DATATYPE> s3 = s.subString( s.begin(), s.end() );
			REQUIRE( s3=="helloworld" );
		}

		SECTION("toEnd")
		{
			StringImpl<DATATYPE> s2 = s.subString(0, -1);
			REQUIRE( s2=="helloworld" );
		}
	}

	SECTION("partial")
	{
		SECTION("fromStart")
		{
			StringImpl<DATATYPE> s2 = s.subString(0, 5);
			REQUIRE( s2=="hello" );

			StringImpl<DATATYPE> s3 = s.subString( s.begin(), s.begin()+5 );
			REQUIRE( s3=="hello" );
		}

		SECTION("fromMiddleToMiddle")
		{
			StringImpl<DATATYPE> s2 = s.subString(3, 5);
			REQUIRE( s2=="lowor" );

			StringImpl<DATATYPE> s3 = s.subString( s.begin()+3, s.begin()+8 );
			REQUIRE( s3=="lowor" );
		}

		SECTION("fromMiddleToEnd")
		{
			SECTION("explicitLength")
			{
				StringImpl<DATATYPE> s2 = s.subString(5, 5);
				REQUIRE( s2=="world" );

				StringImpl<DATATYPE> s3 = s.subString( s.begin()+5, s.end() );
				REQUIRE( s3=="world" );
			}
			SECTION("toEnd")
			{
				StringImpl<DATATYPE> s2 = s.subString(5, -1);
				REQUIRE( s2=="world" );
			}
		}
	}

	SECTION("invalidStart")
	{
		REQUIRE_THROWS_AS( s.subString(-1, 0), OutOfRangeError );
		REQUIRE_THROWS_AS( s.subString(-1, 1), OutOfRangeError );
		REQUIRE_THROWS_AS( s.subString(11, 0), OutOfRangeError );
		REQUIRE_THROWS_AS( s.subString(11, 1), OutOfRangeError );
	}

	SECTION("lengthExceeded")
	{
		StringImpl<DATATYPE> b = s.subString(5, 6);
		REQUIRE( b=="world" );
		REQUIRE( b.getLength()==5 );

		b = s.subString(9, 6);
		REQUIRE( b=="d" );
		REQUIRE( b.getLength()==1 );

		b = s.subString(10, 6);
		REQUIRE( b=="" );
		REQUIRE( b.getLength()==0 );
		REQUIRE( b.isEmpty() );
	}		

}


template <class IT>
inline void verifyIterators(IT begin, IT end, const std::u32string& expected)
{
	IT it = begin;

	for( auto expectedIt = expected.begin(); expectedIt!=expected.end(); expectedIt++)
	{
		REQUIRE( checkEquality(it, end, false) );
		REQUIRE( (*it)==(*expectedIt));
		++it;
	}

	REQUIRE( checkEquality(it, end, true) );

	// do reverse iteration

	for( auto expectedIt = expected.rbegin(); expectedIt!=expected.rend(); expectedIt++)
	{
		REQUIRE( checkEquality(it, begin, false) );
		--it;

		REQUIRE( checkEquality(it, end, false) );
		REQUIRE( *it==*expectedIt);
	}

	REQUIRE( checkEquality(it, begin, true) );
}

template<class DATATYPE>
inline void testIterators()
{
	SECTION("empty")
	{
		StringImpl<DATATYPE> s;
		
		REQUIRE( checkEquality(s.begin(), s.end(), true) );
		REQUIRE( checkEquality(s.cbegin(), s.cend(), true) );
		REQUIRE( checkEquality(s.rbegin(), s.rend(), true) );
		REQUIRE( checkEquality(s.crbegin(), s.crend(), true) );
	}

	SECTION("nonEmpty")
	{
		StringImpl<DATATYPE> s("hello");

		REQUIRE( checkEquality(s.begin(), s.end(), false) );
		REQUIRE( checkEquality(s.cbegin(), s.cend(), false) );
		REQUIRE( checkEquality(s.rbegin(), s.rend(), false) );
		REQUIRE( checkEquality(s.crbegin(), s.crend(), false) );

		verifyIterators( s.begin(), s.end(), U"hello" );
		verifyIterators( s.cbegin(), s.cend(), U"hello" );
		verifyIterators( s.rbegin(), s.rend(), U"olleh" );
		verifyIterators( s.crbegin(), s.crend(), U"olleh" );		
	}
}


template<class DATATYPE>
inline void testConversion()
{
	StringImpl<DATATYPE> s("hello");

	SECTION("utf8")
	{
		const char* p = s.asUtf8Ptr();
		REQUIRE( std::string(p)=="hello" );
		
		const char* p2( s );
		REQUIRE( std::string(p2)=="hello" );

		// must be the exact same pointer
		REQUIRE( p2==p );

		const std::string& o = s.asUtf8();
		REQUIRE( o=="hello" );
		const std::string& o2 = s.asUtf8();
		REQUIRE( o2=="hello" );

		// must be the same object
		REQUIRE( &o==&o2 );
	}

	SECTION("utf8")
	{
		const char* p = s.asUtf8Ptr();
		REQUIRE( strcmp(p, "hello")==0 );
	}
}

template<class DATATYPE>
inline void testStringImpl()
{
	SECTION("construct")
	{
		testConstruct<DATATYPE>();
	}

	SECTION("equality")
	{
		StringImpl<DATATYPE> s("helloworld");
		StringImpl<DATATYPE> s2("helloworld");
		StringImpl<DATATYPE> s3("bla");

		REQUIRE( checkEquality(s, s2, true) );
		REQUIRE( checkEquality(s, s3, false) );

		REQUIRE( s=="helloworld" );
		REQUIRE( s!="helloworl" );
		REQUIRE( s!="elloworld" );
		REQUIRE( s!="helgoworld" );
		REQUIRE( s!="helloWorld" );		
	}

	SECTION("isEmpty")
	{
		StringImpl<DATATYPE> s("helloworld");
		StringImpl<DATATYPE> s2("");
		StringImpl<DATATYPE> s3;
		StringImpl<DATATYPE> s4(s.begin(), s.begin());;

		REQUIRE( !s.isEmpty() );
		REQUIRE( s2.isEmpty() );
		REQUIRE( s3.isEmpty() );
		REQUIRE( s4.isEmpty() );
	}

	SECTION("length")
	{
		testLength<DATATYPE>();
	}

	SECTION("subString")
	{
		testSubString<DATATYPE>();		
	}

	SECTION("iterators")
	{
		testIterators<DATATYPE>();		
	}

	SECTION("conversion")
	{
		testConversion<DATATYPE>();
	}
}


TEST_CASE("StringImpl")
{
	SECTION("utf8")
	{
		testStringImpl<Utf8StringData>();
	}

	SECTION("utf16")
	{
		testStringImpl<Utf16StringData>();
	}

	SECTION("utf32")
	{
		testStringImpl<Utf32StringData>();
	}

	SECTION("WString")
	{
		testStringImpl<WStringData>();
	}

	SECTION("native")
	{
		testStringImpl<NativeStringData>();
	}
}

void verifyWideMultiByteConversion( const std::wstring& inWide,
									const std::string& multiByte,
									const std::wstring& outWide)
{
	// the back-converted string should have the same length at least. Unencodable character
	// should have been replaced with a replacement character.

	// note that the wide versions should have the same lengths
	for(size_t i=0; i<inWide.length(); i++)
	{
		wchar_t inChr = inWide[i];

		REQUIRE( i<outWide.length() );

		wchar_t outChr = outWide[i];

		// we assume that ASCII characters are representable in the multibyte encoding.
		if(inChr<0x80)
		{
			REQUIRE( outChr==inChr );
		}
		else
		{
			// non-ASCII characters might have been replaced with one of the replacement characters
			REQUIRE( (outChr==inChr || outChr==U'\ufffd' || outChr==U'?') );
		}
	}

	REQUIRE(outWide.length() == inWide.length());
}

void verifyWideMultiByteConversion(const std::wstring& inWide)
{
	std::string  multiByte;
	std::wstring outWide;

	SECTION("defaultLocale")
	{
		multiByte = wideToLocaleMultiByte(inWide);
		outWide = localeMultiByteToWide(multiByte);

		verifyWideMultiByteConversion(inWide, multiByte, outWide);
	}

	SECTION("globalLocale")
	{
		multiByte = wideToLocaleMultiByte(inWide, std::locale());
		outWide = localeMultiByteToWide(multiByte, std::locale());

		verifyWideMultiByteConversion(inWide, multiByte, outWide);
	}

	SECTION("classicLocale")
	{
		multiByte = wideToLocaleMultiByte(inWide, std::locale::classic());
		outWide = localeMultiByteToWide(multiByte, std::locale::classic());

		verifyWideMultiByteConversion(inWide, multiByte, outWide);
	}

}


TEST_CASE("wideMultiByteConversion")
{

	struct SubTestData
	{
		const wchar_t*	wide;
		const char*		desc;
	};

	SubTestData allData[] = {	{ L"", "empty" },
								{ L"\u0000", "zero char" },
								{ L"h", "ascii char" },
								{ L"hx", "ascii 2 chars" },
								{ L"\u0345", "non-ascii below surrogate range" },
								{ L"\U00010437", "surrogate range A" },
								{ L"\U00024B62", "surrogate range B" },
								{ L"\uE000", "above surrogate range A" },
								{ L"\uF123", "above surrogate range B" },
								{ L"\uFFFF", "above surrogate range C" }
	};

	int dataCount = std::extent<decltype(allData)>().value;

	SubTestData* pCurrData = GENERATE( between( allData, &allData[dataCount-1] ) );

	SECTION(pCurrData->desc)
	{
		verifyWideMultiByteConversion( pCurrData->wide );
	}

	SECTION(std::string(pCurrData->desc) +" mixed")
	{
		verifyWideMultiByteConversion( L"hello" + std::wstring(pCurrData->wide)
										+ L"wo" + std::wstring(pCurrData->wide)+std::wstring(pCurrData->wide)
										+ L"rld");
	}
}

