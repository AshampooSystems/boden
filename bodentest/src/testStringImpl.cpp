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
void verifyMultiByteResult(const StringImpl<DATATYPE>& in, const std::wstring& outWide)
{
	auto inIt = in.begin();
	auto outIt = outWide.begin();

	while(inIt!=in.end())
	{
		REQUIRE( outIt != outWide.end() );

		char32_t	inChr = *inIt;
		char32_t	outChr = (char32_t)(*outIt);

		if(inChr<0x80)
		{
			// ascii characters should be representable
			REQUIRE( outChr==inChr );
		}
		else
		{
			// non-ASCII characters could have been replaced with a replacement character
			REQUIRE( (outChr==inChr || outChr==U'\ufffd' || outChr==U'?') );
		}

		++inIt;
		++outIt;
	}

	REQUIRE( outIt == outWide.end() );
}

template<class DATATYPE>
inline void testConversion()
{
	StringImpl<DATATYPE> s(U"he\u0218\u0777\uffffllo");

	SECTION("utf8")
	{
		const char* p = s.asUtf8Ptr();
		REQUIRE( std::string(p)==u8"he\u0218\u0777\uffffllo" );
		
		const char* p2( s );
		REQUIRE( std::string(p2)==u8"he\u0218\u0777\uffffllo" );

		// must be the exact same pointer
		REQUIRE( p2==p );

		const std::string& o = s.asUtf8();
		REQUIRE( o==u8"he\u0218\u0777\uffffllo" );
		const std::string& o2 = s.asUtf8();
		REQUIRE( o2==u8"he\u0218\u0777\uffffllo" );

		const std::string& o3 = s;
		REQUIRE( o3==u8"he\u0218\u0777\uffffllo" );

		// must be the same object
		REQUIRE( &o==&o2 );
		REQUIRE( &o==&o3 );
	}

	SECTION("utf16")
	{
		const char16_t* p = s.asUtf16Ptr();
		REQUIRE( std::u16string(p)==u"he\u0218\u0777\uffffllo" );

		const char16_t* p2( s );
		REQUIRE( std::u16string(p2)==u"he\u0218\u0777\uffffllo" );

		// must be the exact same pointer
		REQUIRE( p2==p );

		const std::u16string& o = s.asUtf16();
		REQUIRE( o==u"he\u0218\u0777\uffffllo" );
		const std::u16string& o2 = s.asUtf16();
		REQUIRE( o2==u"he\u0218\u0777\uffffllo" );

		const std::u16string& o3 = s;
		REQUIRE( o3==u"he\u0218\u0777\uffffllo" );

		// must be the same object
		REQUIRE( &o==&o2 );
		REQUIRE( &o==&o3 );
	}

	SECTION("utf32")
	{
		const char32_t* p = s.asUtf32Ptr();
		REQUIRE( std::u32string(p)==U"he\u0218\u0777\uffffllo" );

		const char32_t* p2( s );
		REQUIRE( std::u32string(p2)==U"he\u0218\u0777\uffffllo" );

		// must be the exact same pointer
		REQUIRE( p2==p );

		const std::u32string& o = s.asUtf32();
		REQUIRE( o==U"he\u0218\u0777\uffffllo" );
		const std::u32string& o2 = s.asUtf32();
		REQUIRE( o2==U"he\u0218\u0777\uffffllo" );

		const std::u32string& o3 = s;
		REQUIRE( o3==U"he\u0218\u0777\uffffllo" );

		// must be the same object
		REQUIRE( &o==&o2 );
		REQUIRE( &o==&o3 );
	}

	SECTION("wide")
	{
		const wchar_t* p = s.asWidePtr();
		REQUIRE( std::wstring(p)==L"he\u0218\u0777\uffffllo" );

		const wchar_t* p2( s );
		REQUIRE( std::wstring(p2)==L"he\u0218\u0777\uffffllo" );

		// must be the exact same pointer
		REQUIRE( p2==p );

		const std::wstring& o = s.asWide();
		REQUIRE( o==L"he\u0218\u0777\uffffllo" );
		const std::wstring& o2 = s.asWide();
		REQUIRE( o2==L"he\u0218\u0777\uffffllo" );

		const std::wstring& o3 = s;
		REQUIRE( o3==L"he\u0218\u0777\uffffllo" );

		// must be the same object
		REQUIRE( &o==&o2 );
		REQUIRE( &o==&o3 );
	}

	SECTION("multibyte")
	{
		SECTION("default")
		{
			std::string m = s.toLocaleEncoding();
			verifyMultiByteResult(s, localeEncodingToWide(m) );
		}

		SECTION("global")
		{
			std::locale loc;
			std::string m = s.toLocaleEncoding(loc);
			verifyMultiByteResult(s, localeEncodingToWide(m, loc) );
		}

		SECTION("classic")
		{
			std::locale loc = std::locale::classic();
			std::string m = s.toLocaleEncoding(loc);
			verifyMultiByteResult(s, localeEncodingToWide(m, loc) );
		}
	}
}


template<class STRING, class OTHER>
void verifyComparison(const STRING& s, OTHER o, int expectedResult)
{
	int result = s.compare(o);
	REQUIRE(result==expectedResult);

	REQUIRE( (s==o)==(expectedResult==0) );
	REQUIRE( (s!=o)==(expectedResult!=0) );

	REQUIRE( (s<o)==(expectedResult<0) );
	REQUIRE( (s<=o)==(expectedResult<=0) );

	REQUIRE( (s>o)==(expectedResult>0) );
	REQUIRE( (s>=o)==(expectedResult>=0) );
}


template<class DATATYPE>
void testComparisonWith(const StringImpl<DATATYPE>& s, const StringImpl<DATATYPE>& other, int expectedResult)
{
	SECTION("String")
	{
		verifyComparison(s, other, expectedResult);
	}

	SECTION("utf8")
	{
		verifyComparison(s, other.asUtf8(), expectedResult);
	}

	SECTION("utf8Ptr")
	{
		verifyComparison(s, other.asUtf8Ptr(), expectedResult);
	}

	SECTION("utf16")
	{
		verifyComparison(s, other.asUtf16(), expectedResult);
	}

	SECTION("utf16Ptr")
	{
		verifyComparison(s, other.asUtf16Ptr(), expectedResult);
	}

	SECTION("utf32")
	{
		verifyComparison(s, other.asUtf32(), expectedResult);
	}

	SECTION("utf32Ptr")
	{
		verifyComparison(s, other.asUtf32Ptr(), expectedResult);
	}

	SECTION("wide")
	{
		verifyComparison(s, other.asWide(), expectedResult);
	}

	SECTION("widePtr")
	{
		verifyComparison(s, other.asWidePtr(), expectedResult);
	}	
}

template<class DATATYPE>
inline void testComparison()
{
	StringImpl<DATATYPE> s("HeLLo");

	SECTION("empty")
		testComparisonWith<DATATYPE>(s, "", 1);
	
	SECTION("shorter")
		testComparisonWith<DATATYPE>(s, "HeLL", 1);

	SECTION("smaller")
		testComparisonWith<DATATYPE>(s, "AbCDe", 1);

	SECTION("smallerAndLonger")
		testComparisonWith<DATATYPE>(s, "AbCDef", 1);

	SECTION("smallerB")
		testComparisonWith<DATATYPE>(s, "HELLo", 1);

	SECTION("same")
		testComparisonWith<DATATYPE>(s, "HeLLo", 0);
	
	SECTION("bigger")
		testComparisonWith<DATATYPE>(s, "heLLo", -1);

	SECTION("biggerB")
		testComparisonWith<DATATYPE>(s, "Hello", -1);

	SECTION("biggerAndShorter")
		testComparisonWith<DATATYPE>(s, "hell", -1);

	SECTION("longer")
		testComparisonWith<DATATYPE>(s, "HeLLox", -1);
}

template<class DATATYPE>
inline void verifyCharAccess(const StringImpl<DATATYPE>& s)
{
	SECTION("operator[]")
	{
		REQUIRE(s[0]==U'h');
		REQUIRE(s[4]==U'o');
		REQUIRE(s[5]==U'\0');

		REQUIRE_THROWS_AS(s[6], OutOfRangeError);
		REQUIRE_THROWS_AS(s[-1], OutOfRangeError);
	}

	SECTION("at")
	{
		REQUIRE(s.at(0)==U'h');
		REQUIRE(s.at(4)==U'o');
		REQUIRE(s.at(5)==U'\0');
	
		REQUIRE_THROWS_AS(s.at(6), OutOfRangeError);
	}

	SECTION("getLastChar,back")
	{
		REQUIRE(s.getLastChar()=='o');
		REQUIRE(s.back()=='o');

		StringImpl<DATATYPE> empty;
		REQUIRE_THROWS_AS(empty.getLastChar(), OutOfRangeError);
		REQUIRE_THROWS_AS(empty.back(), OutOfRangeError);
	}

	SECTION("getFirstChar,front")
	{
		REQUIRE(s.getFirstChar()=='h');
		REQUIRE(s.front()=='h');

		StringImpl<DATATYPE> empty;
		REQUIRE_THROWS_AS(empty.getFirstChar(), OutOfRangeError);
		REQUIRE_THROWS_AS(empty.front(), OutOfRangeError);
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

	SECTION("comparison")
	{
		testComparison<DATATYPE>();
	}

	SECTION("charAccess")
	{
		SECTION("normal")
		{
			StringImpl<DATATYPE> s("hello");

			verifyCharAccess(s);			
		}

		SECTION("slice")
		{
			StringImpl<DATATYPE> bigger("hihelloworld");
			StringImpl<DATATYPE> s = bigger.subString(2, 5);

			verifyCharAccess(s);
		}
	}

	SECTION("back")
	{

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
		testStringImpl<WideStringData>();
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

void verifyWideLocaleEncodingConversion(const std::wstring& inWide)
{
	std::string  multiByte;
	std::wstring outWide;

	SECTION("defaultLocale")
	{
		multiByte = wideToLocaleEncoding(inWide);
		outWide = localeEncodingToWide(multiByte);

		verifyWideMultiByteConversion(inWide, multiByte, outWide);
	}

	SECTION("globalLocale")
	{
		multiByte = wideToLocaleEncoding(inWide, std::locale());
		outWide = localeEncodingToWide(multiByte, std::locale());

		verifyWideMultiByteConversion(inWide, multiByte, outWide);
	}

	SECTION("classicLocale")
	{
		multiByte = wideToLocaleEncoding(inWide, std::locale::classic());
		outWide = localeEncodingToWide(multiByte, std::locale::classic());

		verifyWideMultiByteConversion(inWide, multiByte, outWide);
	}

}


TEST_CASE("wideLocaleEncodingConversion")
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
		verifyWideLocaleEncodingConversion( pCurrData->wide );
	}

	SECTION(std::string(pCurrData->desc) +" mixed")
	{
		verifyWideLocaleEncodingConversion( L"hello" + std::wstring(pCurrData->wide)
										+ L"wo" + std::wstring(pCurrData->wide)+std::wstring(pCurrData->wide)
										+ L"rld");
	}
}



