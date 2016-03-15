#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Utf16StringData.h>
#include <bdn/Utf32StringData.h>

#ifdef _MSC_VER
// disable "function may be unsafe" warning in Visual C++. We have to test these
// "unsafe" functions as well.
#pragma warning(disable: 4996)
#endif

using namespace bdn;



template<class DATATYPE>
inline void testTypes()
{
#if BDN_WINDOWS
	REQUIRE( typeid(StringImpl<DATATYPE>::NativeEncodedString) == typeid(std::wstring) );
	REQUIRE( typeid(StringImpl<DATATYPE>::NativeEncodedElement) == typeid(wchar_t) );

#else
	REQUIRE( typeid(StringImpl<DATATYPE>::NativeEncodedString) == typeid(std::string) );
	REQUIRE( typeid(StringImpl<DATATYPE>::NativeEncodedElement) == typeid(char) );

#endif

	REQUIRE( typeid(StringImpl<DATATYPE>::Iterator) == typeid(DATATYPE::Iterator) );
	REQUIRE( typeid(StringImpl<DATATYPE>::iterator) == typeid(DATATYPE::Iterator) );
	REQUIRE( typeid(StringImpl<DATATYPE>::const_iterator) == typeid(DATATYPE::Iterator) );

	REQUIRE( typeid(StringImpl<DATATYPE>::ReverseIterator) == typeid( std::reverse_iterator<StringImpl<DATATYPE>::Iterator> ) );
	REQUIRE( typeid(StringImpl<DATATYPE>::reverse_iterator) == typeid( StringImpl<DATATYPE>::ReverseIterator ) );
	REQUIRE( typeid(StringImpl<DATATYPE>::const_reverse_iterator) == typeid( StringImpl<DATATYPE>::ReverseIterator ) );
	
	REQUIRE( typeid(StringImpl<DATATYPE>::Allocator) == typeid(DATATYPE::Allocator) );
	REQUIRE( typeid(StringImpl<DATATYPE>::allocator_type) == typeid(DATATYPE::Allocator) );

	REQUIRE( typeid(StringImpl<DATATYPE>::value_type) == typeid(char32_t) );
	REQUIRE( typeid(StringImpl<DATATYPE>::traits_type) == typeid( std::char_traits<char32_t> ) );

	REQUIRE( typeid(StringImpl<DATATYPE>::pointer) == typeid(char32_t*) );
	REQUIRE( typeid(StringImpl<DATATYPE>::const_pointer) == typeid(const char32_t*) );

	REQUIRE( typeid(StringImpl<DATATYPE>::reference) == typeid(char32_t&) );
	REQUIRE( typeid(StringImpl<DATATYPE>::const_reference) == typeid(const char32_t&) );

	REQUIRE( typeid(StringImpl<DATATYPE>::size_type) == typeid(size_t) );
}



template<class DATATYPE>
inline void testConstants()
{
	REQUIRE( StringImpl<DATATYPE>::npos == std::string::npos);
	REQUIRE( StringImpl<DATATYPE>::noMatch == StringImpl<DATATYPE>::npos );
	REQUIRE( StringImpl<DATATYPE>::toEnd == StringImpl<DATATYPE>::npos );
}


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

		SECTION("slice")
		{
			StringImpl<DATATYPE> source("xyhelloworldabc");

			StringImpl<DATATYPE> s = source.subString(2, 10);

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
		SECTION("asPtr")
		{
			const char* p = s.asUtf8Ptr();
			REQUIRE( std::string(p)==u8"he\u0218\u0777\uffffllo" );
		}

		SECTION("as")
		{
			const std::string& o = s.asUtf8();
			REQUIRE( o==u8"he\u0218\u0777\uffffllo" );

			const std::string& o2 = s.asUtf8();
			REQUIRE( o2==u8"he\u0218\u0777\uffffllo" );

			// must be the same object
			REQUIRE( &o==&o2 );
		}

		SECTION("constPtrConversion")
		{		
			const char* p( s );
			REQUIRE( std::string(p)==u8"he\u0218\u0777\uffffllo" );
			
			// must be the exact same pointer
			REQUIRE( p==s.asUtf8Ptr() );
		}

		SECTION("stdStringConversion")
		{
			const std::string& o = s;
			REQUIRE( o==u8"he\u0218\u0777\uffffllo" );

			const std::string& o2 = s.asUtf8();
			REQUIRE( o2==u8"he\u0218\u0777\uffffllo" );

			// must be the same object
			REQUIRE( &o==&o2 );
		}
	}

	SECTION("utf16")
	{
		SECTION("asPtr")
		{
			const char16_t* p = s.asUtf16Ptr();
			REQUIRE( std::u16string(p)==u"he\u0218\u0777\uffffllo" );
		}

		SECTION("as")
		{
			const std::u16string& o = s.asUtf16();
			REQUIRE( o==u"he\u0218\u0777\uffffllo" );

			const std::u16string& o2 = s.asUtf16();
			REQUIRE( o2==u"he\u0218\u0777\uffffllo" );

			// must be the same object
			REQUIRE( &o==&o2 );
		}

		SECTION("constPtrConversion")
		{		
			const char16_t* p( s );
			REQUIRE( std::u16string(p)==u"he\u0218\u0777\uffffllo" );
			
			// must be the exact same pointer
			REQUIRE( p==s.asUtf16Ptr() );
		}

		SECTION("stdStringConversion")
		{
			const std::u16string& o = s;
			REQUIRE( o==u"he\u0218\u0777\uffffllo" );

			const std::u16string& o2 = s.asUtf16();
			REQUIRE( o2==u"he\u0218\u0777\uffffllo" );

			// must be the same object
			REQUIRE( &o==&o2 );
		}
	}

	SECTION("utf32")
	{
		SECTION("asPtr")
		{
			const char32_t* p = s.asUtf32Ptr();
			REQUIRE( std::u32string(p)==U"he\u0218\u0777\uffffllo" );
		}

		SECTION("as")
		{
			const std::u32string& o = s.asUtf32();
			REQUIRE( o==U"he\u0218\u0777\uffffllo" );

			const std::u32string& o2 = s.asUtf32();
			REQUIRE( o2==U"he\u0218\u0777\uffffllo" );

			// must be the same object
			REQUIRE( &o==&o2 );
		}

		SECTION("constPtrConversion")
		{		
			const char32_t* p( s );
			REQUIRE( std::u32string(p)==U"he\u0218\u0777\uffffllo" );
			
			// must be the exact same pointer
			REQUIRE( p==s.asUtf32Ptr() );
		}

		SECTION("stdStringConversion")
		{
			const std::u32string& o = s;
			REQUIRE( o==U"he\u0218\u0777\uffffllo" );

			const std::u32string& o2 = s.asUtf32();
			REQUIRE( o2==U"he\u0218\u0777\uffffllo" );

			// must be the same object
			REQUIRE( &o==&o2 );
		}


		SECTION("c_str")
		{		
			const char32_t* p = s.c_str();
			REQUIRE( std::u32string(p)==U"he\u0218\u0777\uffffllo" );
			
			// must be the exact same pointer
			REQUIRE( p==s.asUtf32Ptr() );
		}

		SECTION("data")
		{		
			const char32_t* p = s.data();
			REQUIRE( std::u32string(p)==U"he\u0218\u0777\uffffllo" );
			
			// must be the exact same pointer
			REQUIRE( p==s.asUtf32Ptr() );
		}
	}

	SECTION("wide")
	{
		SECTION("asPtr")
		{
			const wchar_t* p = s.asWidePtr();
			REQUIRE( std::wstring(p)==L"he\u0218\u0777\uffffllo" );
		}

		SECTION("as")
		{
			const std::wstring& o = s.asWide();
			REQUIRE( o==L"he\u0218\u0777\uffffllo" );

			const std::wstring& o2 = s.asWide();
			REQUIRE( o2==L"he\u0218\u0777\uffffllo" );

			// must be the same object
			REQUIRE( &o==&o2 );
		}

		SECTION("constPtrConversion")
		{		
			const wchar_t* p( s );
			REQUIRE( std::wstring(p)==L"he\u0218\u0777\uffffllo" );
			
			// must be the exact same pointer
			REQUIRE( p==s.asWidePtr() );
		}

		SECTION("stdStringConversion")
		{
			const std::wstring& o = s;
			REQUIRE( o==L"he\u0218\u0777\uffffllo" );

			const std::wstring& o2 = s.asWide();
			REQUIRE( o2==L"he\u0218\u0777\uffffllo" );

			// must be the same object
			REQUIRE( &o==&o2 );
		}
	}


	SECTION("native")
	{
		SECTION("asPtr")
		{
			const StringImpl<DATATYPE>::NativeEncodedElement* p = s.asNativePtr();

#if BDN_WINDOWS
			REQUIRE( std::wstring(p)==L"he\u0218\u0777\uffffllo" );
#else
			REQUIRE( std::string(p)==u8"he\u0218\u0777\uffffllo" );
#endif
		}

		SECTION("as")
		{
			const StringImpl<DATATYPE>::NativeEncodedString& o = s.asNative();

#if BDN_WINDOWS
			REQUIRE( o==L"he\u0218\u0777\uffffllo" );
#else
			REQUIRE( o==u8"he\u0218\u0777\uffffllo" );
#endif
			
			const StringImpl<DATATYPE>::NativeEncodedString& o2 = s.asNative();

#if BDN_WINDOWS
			REQUIRE( o2==L"he\u0218\u0777\uffffllo" );
#else
			REQUIRE( o2==u8"he\u0218\u0777\uffffllo" );
#endif

			// must be the same object
			REQUIRE( &o==&o2 );
		}
		
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


template<class StringType, class RANGETYPE>
inline void verifyReplace(StringType& s, RANGETYPE start, RANGETYPE end, const StringType& replaceWith, const StringType& expected)
{
	SECTION("withIterators")
	{
		s.replace( start, end, replaceWith.begin(), replaceWith.end() );
		REQUIRE( s==expected );
	}

	SECTION("withIteratorsFromOtherClass")
	{
		std::u32string rep = replaceWith.asUtf32();

		s.replace( start, end, rep.begin(), rep.end() );
		REQUIRE( s==expected );
	}

	SECTION("withString")
	{
		s.replace( start, end, replaceWith );
		REQUIRE( s==expected );
	}

	SECTION("withSubString")
	{
		std::u32string replaceWith2Utf32 = U"abc"+replaceWith.asUtf32()+U"efg";
		StringType replaceWith2( replaceWith2Utf32 );

		s.replace( start, end, replaceWith2, 3, replaceWith.getLength() );
		REQUIRE( s==expected );
	}

	SECTION("withSubStringWithoutLength")
	{
		std::u32string replaceWith2Utf32 = U"abc"+replaceWith.asUtf32();
		StringType replaceWith2( replaceWith2Utf32 );

		s.replace( start, end, replaceWith2, 3 );
		REQUIRE( s==expected );
	}

	SECTION("withSubStringWithLengthTooBig")
	{
		std::u32string replaceWith2Utf32 = U"abc"+replaceWith.asUtf32();
		StringType replaceWith2( replaceWith2Utf32 );

		s.replace( start, end, replaceWith2, 3, 100 );
		REQUIRE( s==expected );
	}

	SECTION("withUTF8")
	{
		s.replace( start, end, replaceWith.asUtf8() );
		REQUIRE( s==expected );
	}

	SECTION("withUTF8Ptr")
	{
		s.replace( start, end, replaceWith.asUtf8Ptr() );
		REQUIRE( s==expected );
	}

	SECTION("withUTF8PtrWithLength")
	{
		std::string rep = replaceWith.asUtf8()+"xyz";

		s.replace( start, end, rep.c_str(), rep.length()-3 );
		REQUIRE( s==expected );
	}

	SECTION("withUTF16")
	{
		s.replace( start, end, replaceWith.asUtf16() );
		REQUIRE( s==expected );
	}

	SECTION("withUTF16Ptr")
	{
		s.replace( start, end, replaceWith.asUtf16Ptr() );
		REQUIRE( s==expected );
	}

	SECTION("withUTF16PtrWithLength")
	{
		std::u16string rep = replaceWith.asUtf16()+u"xyz";

		s.replace( start, end, rep.c_str(), rep.length()-3 );
		REQUIRE( s==expected );
	}

	SECTION("withUTF32")
	{
		s.replace( start, end, replaceWith.asUtf32() );
		REQUIRE( s==expected );
	}

	SECTION("withUTF32Ptr")
	{
		s.replace( start, end, replaceWith.asUtf32Ptr() );
		REQUIRE( s==expected );
	}

	SECTION("withUTF32PtrWithLength")
	{
		std::u32string rep = replaceWith.asUtf32()+U"xyz";

		s.replace( start, end, rep.c_str(), rep.length()-3 );
		REQUIRE( s==expected );
	}

	SECTION("withWide")
	{
		s.replace( start, end, replaceWith.asWide() );
		REQUIRE( s==expected );
	}

	SECTION("withWidePtr")
	{
		s.replace( start, end, replaceWith.asWidePtr() );
		REQUIRE( s==expected );
	}

	SECTION("withWidePtrWithLength")
	{
		std::wstring rep = replaceWith.asWide()+L"xyz";

		s.replace( start, end, rep.c_str(), rep.length()-3 );
		REQUIRE( s==expected );
	}

	SECTION("withCharInitializerList")
	{
		if(replaceWith.isEmpty())
		{
			s.replace( start, end, {} );
			REQUIRE( s==expected );
		}
		else if(replaceWith==U"BL\U00013333A")
		{
			// must leave out the unicode character
			s.replace( start, end, {'B', 'L', 'A'} );

			std::u32string exp = expected;
			size_t found = exp.find(U'\U00013333');
			if(found!=String::npos)
				exp.erase(found, 1);

			REQUIRE( s==exp );
		}
		else
			throw std::runtime_error("Test must be updated for new possble argument replaceWith");
	}

	SECTION("withChar32InitializerList")
	{
		if(replaceWith.isEmpty())
		{
			s.replace( start, end, {} );
			REQUIRE( s==expected );
		}
		else if(replaceWith==U"BL\U00013333A")
		{
			s.replace( start, end, {U'B', U'L', U'\U00013333', U'A'} );
			
			REQUIRE( s==expected );
		}
		else
			throw std::runtime_error("Test must be updated for new possble argument replaceWith");
	}
}

template<class DATATYPE>
inline void testReplaceWithString(StringImpl<DATATYPE>& s)
{
	struct TestData
	{
		int startIndex;
		int length;
		const char32_t* replaceWith;
		const char32_t* expectedResult;
		const char* desc;
	};

	TestData testData[] = { {0, 0, U"", U"he\U00012345loworld", "emptyWithEmptyAtStart"},
							{10, 0, U"", U"he\U00012345loworld", "emptyWithEmptyAtEnd"},
							{10, -1, U"", U"he\U00012345loworld", "emptyWithEmptyAtEndWithoutLength"},
							{10, 20, U"", U"he\U00012345loworld", "emptyWithEmptyAtEndWithLengthTooBig"},
							{5, 0, U"", U"he\U00012345loworld", "emptyWithEmptyInMiddle"},	

							{0, 0, U"BL\U00013333A", U"BL\U00013333Ahe\U00012345loworld", "emptyWithNonEmptyAtStart"},
							{10, 0, U"BL\U00013333A", U"he\U00012345loworldBL\U00013333A", "emptyWithNonEmptyAtEnd"},
							{10, -1, U"BL\U00013333A", U"he\U00012345loworldBL\U00013333A", "emptyWithNonEmptyAtEndWithoutLength"},
							{10, 20, U"BL\U00013333A", U"he\U00012345loworldBL\U00013333A", "emptyWithNonEmptyAtEndWithLengthTooBig"},
							{5, 0, U"BL\U00013333A", U"he\U00012345loBL\U00013333Aworld", "emptyWithNonEmptyInMiddle"},	

							{0, 2, U"", U"\U00012345loworld", "nonEmptyWithEmptyAtStart"},
							{8, 2, U"", U"he\U00012345lowor", "nonEmptyWithEmptyAtEnd"},
							{8, -1, U"", U"he\U00012345lowor", "nonEmptyWithEmptyAtEndWithoutLength"},
							{8, 20, U"", U"he\U00012345lowor", "nonEmptyWithEmptyAtEndWithLengthTooBig"},
							{5, 2, U"", U"he\U00012345lorld", "nonEmptyWithEmptyInMiddle"},	

							{0, 2, U"BL\U00013333A", U"BL\U00013333A\U00012345loworld", "nonEmptyWithNonEmptyAtStart"},
							{0, 4, U"BL\U00013333A", U"BL\U00013333Aoworld", "nonEmptyWithNonEmptyAtStartB"},
							{8, 2, U"BL\U00013333A", U"he\U00012345loworBL\U00013333A", "nonEmptyWithNonEmptyAtEnd"},
							{8, -1, U"BL\U00013333A", U"he\U00012345loworBL\U00013333A", "nonEmptyWithNonEmptyAtEndWithoutLength"},
							{8, 20, U"BL\U00013333A", U"he\U00012345loworBL\U00013333A", "nonEmptyWithNonEmptyAtEndWithLengthTooBig"},
							{5, 2, U"BL\U00013333A", U"he\U00012345loBL\U00013333Arld", "nonEmptyWithNonEmptyInMiddle"},	
							};

	int testDataCount = std::extent<decltype(testData)>().value;

	for(int t=0; t<testDataCount; t++)
	{
		TestData* pTestData = &testData[t];

		SECTION(pTestData->desc)
		{
			SECTION("rangeIndexLength")
			{
				verifyReplace<StringImpl<DATATYPE>, size_t>(s,
															pTestData->startIndex,
															((pTestData->length==-1) ? String::npos : pTestData->length),
															pTestData->replaceWith,
															pTestData->expectedResult);
			}

			SECTION("rangeIterators")
			{
				StringImpl<DATATYPE>::Iterator start = s.begin()+pTestData->startIndex;
				StringImpl<DATATYPE>::Iterator end;
			
				if(pTestData->length==-1)
					end = s.end();
				else if(pTestData->startIndex+pTestData->length>(int)s.getLength())
				{
					// some tests pass a length that is too big. We cannot represent that with iterators.
					end = s.end();
				}
				else
					end = start + pTestData->length;

				verifyReplace<StringImpl<DATATYPE>, StringImpl<DATATYPE>::Iterator>(	s,
																						start,
																						end,
																						pTestData->replaceWith,
																						pTestData->expectedResult);
			}
		}
	}
}


template<class DATATYPE>
inline void verifyReplaceNumChars(StringImpl<DATATYPE>& s, int startIndex, int length, int charCount, char32_t chr)
{
	std::u32string expected = s.asUtf32();

	expected.replace(	startIndex,
						((length==-1) ? String::npos : length),
						charCount,
						chr );

	SECTION("rangeIndexLength")
	{
		s.replace(	startIndex,
					((length==-1) ? String::npos : length),
					charCount,
					chr );

		REQUIRE( s==expected );
	}
	
	SECTION("rangeIterators")
	{
		StringImpl<DATATYPE>::Iterator start = s.begin()+startIndex;
		StringImpl<DATATYPE>::Iterator end;
			
		if(length==-1)
			end = s.end();
		else if(startIndex+length>(int)s.getLength())
		{
			// some tests pass a length that is too big. We cannot represent that with iterators.
			end = s.end();
		}
		else
			end = start + length;

		s.replace(	start,
					end,
					charCount,
					chr );

		REQUIRE( s==expected );
	}
}


template<class DATATYPE>
inline void testReplaceNumChars_WithCharCount(StringImpl<DATATYPE>& s, int startIndex, int length, int charCount)
{
	SECTION("nullChar")
	{
		verifyReplaceNumChars<DATATYPE>( s, startIndex, length, charCount, U'\0');
	}

	SECTION("asciiChar")
	{
		verifyReplaceNumChars<DATATYPE>( s, startIndex, length, charCount, U'F');
	}

	SECTION("nonAsciiChar")
	{
		verifyReplaceNumChars<DATATYPE>( s, startIndex, length, charCount, U'\U00011111');
	}
}

template<class DATATYPE>
inline void testReplaceNumChars( StringImpl<DATATYPE>& s)
{
	struct TestData
	{
		int startIndex;
		int length;
		const char* desc;
	};

	TestData testData[] = { {0, 0, "empty at start"},
							{10, 0, "empty at end"},
							{10, -1, "empty at end without length"},
							{10, 20, "empty at end with length too big"},
							{5, 0, "empty in middle"},	
							
							{0, 2, "non-empty at start"},
							{8, 2,  "non-empty at end"},
							{8, -1, "non-empty at end without length"},
							{8, 20, "non-empty at end with length too big"},
							{5, 2, "non-empty in middle"},
						};

	int testDataCount = std::extent<decltype(testData)>().value;

	for(int t=0; t<testDataCount; t++)
	{
		TestData* pTestData = &testData[t];

		SECTION( pTestData->desc )
		{
			SECTION("zero chars")
				testReplaceNumChars_WithCharCount(s, pTestData->startIndex, pTestData->length, 0);

			SECTION("1 char")
				testReplaceNumChars_WithCharCount(s, pTestData->startIndex, pTestData->length, 1);

			SECTION("7 chars")
				testReplaceNumChars_WithCharCount(s, pTestData->startIndex, pTestData->length, 7);
		}
	}
}


template<class DATATYPE>
inline void testReplace()
{
	SECTION("normal")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");

		SECTION("string")		
			testReplaceWithString<DATATYPE>(s);

		SECTION("numChars")		
			testReplaceNumChars<DATATYPE>(s);
	}

	SECTION("slice")
	{
		StringImpl<DATATYPE> s(U"xyhe\U00012345loworldabc");

		SECTION("string")
			testReplaceWithString<DATATYPE>( s.subString(2, 10) );

		SECTION("numChars")
			testReplaceNumChars<DATATYPE>( s.subString(2, 10) );
	}
}



template<class StringType, class SuffixArg>
inline void verifyAppendPlus(StringType& s, SuffixArg suffix, const StringType& expected)
{
	SECTION("append")
		s.append( suffix );

	SECTION("operator+=")
		s += suffix;

	REQUIRE( s==expected );
}


template<class StringType>
inline void verifyAppend(StringType& s, const StringType& suffix, const StringType& expected)
{
	SECTION("iterators")
	{
		s.append( suffix.begin(), suffix.end() );
		REQUIRE( s==expected );
	}

	SECTION("iteratorsFromOtherClass")
	{
		std::u32string suf = suffix.asUtf32();

		s.append( suf.begin(), suf.end() );
		REQUIRE( s==expected );
	}

	SECTION("String")
	{
		verifyAppendPlus(s, suffix, expected);
	}

	SECTION("subString")
	{
		std::u32string sufUtf32 = U"abc"+suffix.asUtf32()+U"efg";
		StringType suf( sufUtf32 );
		
		s.append( suf, 3, suffix.getLength() );
		REQUIRE( s==expected );
	}

	SECTION("subStringWithoutLength")
	{
		std::u32string sufUtf32 = U"abc"+suffix.asUtf32();
		StringType suf( sufUtf32 );

		s.append( suf, 3 );
		REQUIRE( s==expected );
	}

	SECTION("subStringWithLengthTooBig")
	{
		std::u32string sufUtf32 = U"abc"+suffix.asUtf32();
		StringType suf( sufUtf32 );

		s.append( suf, 3, 100 );
		REQUIRE( s==expected );
	}

	SECTION("utf8")
	{
		verifyAppendPlus(s, suffix.asUtf8(), expected);
	}

	SECTION("utf8Ptr")
	{
		verifyAppendPlus(s, suffix.asUtf8Ptr(), expected);
	}

	SECTION("utf8PtrWithLength")
	{
		std::string suf = suffix.asUtf8()+"xyz";

		s.append( suf.c_str(), suf.length()-3 );
		REQUIRE( s==expected );
	}

	SECTION("utf16")
	{
		verifyAppendPlus(s, suffix.asUtf16(), expected);
	}

	SECTION("utf16Ptr")
	{
		verifyAppendPlus(s, suffix.asUtf16Ptr(), expected);
	}

	SECTION("utf16PtrWithLength")
	{
		std::u16string suf = suffix.asUtf16()+u"xyz";

		s.append( suf.c_str(), suf.length()-3 );
		REQUIRE( s==expected );
	}

	SECTION("utf32")
	{
		verifyAppendPlus(s, suffix.asUtf32(), expected);
	}

	SECTION("utf32Ptr")
	{
		verifyAppendPlus(s, suffix.asUtf32Ptr(), expected);
	}

	SECTION("utf32PtrWithLength")
	{
		std::u32string suf = suffix.asUtf32()+U"xyz";

		s.append( suf.c_str(), suf.length()-3 );
		REQUIRE( s==expected );
	}

	SECTION("wide")
	{
		verifyAppendPlus(s, suffix.asWide(), expected);
	}

	SECTION("widePtr")
	{
		verifyAppendPlus(s, suffix.asWidePtr(), expected);
	}

	SECTION("widePtrWithLength")
	{
		std::wstring suf = suffix.asWide()+L"xyz";

		s.append( suf.c_str(), suf.length()-3 );
		REQUIRE( s==expected );
	}	

	if(!suffix.isEmpty())
	{	
		if(suffix==U"BL\U00013333A")
		{
			SECTION("charInitializerList")
			{			
				// must leave out the unicode character
				SECTION("append")
					s.append( {'B', 'L', 'A'} );

				SECTION("operator+=")
					s += {'B', 'L', 'A'};

				std::u32string exp = expected;
				size_t found = exp.find(U'\U00013333');
				if(found!=String::npos)
					exp.erase(found, 1);

				REQUIRE( s==exp );
			}

			SECTION("char32InitializerList")
			{
				SECTION("append")
					s.append( {U'B', U'L', U'\U00013333', U'A'} );

				SECTION("operator+=")
					s += {U'B', U'L', U'\U00013333', U'A'};
			
				REQUIRE( s==expected );
			}
		}
		else
			throw std::runtime_error("Test must be updated for new possble argument suffix");
	}

}

template<class DATATYPE>
inline void testAppendWithString(StringImpl<DATATYPE>& s)
{
	SECTION("empty")
	{
		verifyAppend< StringImpl<DATATYPE> >(s, "", s);
	}

	SECTION("nonEmpty")
	{
		verifyAppend< StringImpl<DATATYPE> >(s, U"BL\U00013333A", s.asUtf32()+U"BL\U00013333A" );
	}
}




template<class DATATYPE>
inline void verifyAppendNumChars(StringImpl<DATATYPE>& s, int charCount, char32_t chr)
{
	std::u32string expected = s.asUtf32();

	expected.append(	charCount,
						chr );

	if(charCount==1)
	{
		SECTION("operator+=")
			s += chr;
	}

	SECTION("append")
		s.append(	charCount, chr );


	REQUIRE( s==expected );
}


template<class DATATYPE>
inline void testAppendNumChars_WithCharCount(StringImpl<DATATYPE>& s, int charCount)
{
	SECTION("nullChar")
	{
		verifyAppendNumChars<DATATYPE>( s, charCount, U'\0');
	}

	SECTION("asciiChar")
	{
		verifyAppendNumChars<DATATYPE>( s, charCount, U'F');
	}

	SECTION("nonAsciiChar")
	{
		verifyAppendNumChars<DATATYPE>( s, charCount, U'\U00011111');
	}
}

template<class DATATYPE>
inline void testAppendNumChars( StringImpl<DATATYPE>& s)
{	
	SECTION("zero chars")
		testAppendNumChars_WithCharCount(s, 0);

	SECTION("1 char")
		testAppendNumChars_WithCharCount(s, 1);

	SECTION("7 chars")
		testAppendNumChars_WithCharCount(s, 7);
}


template<class DATATYPE>
inline void testAppendSingleChar( StringImpl<DATATYPE>& s)
{	
	std::u32string expected = s.asUtf32();

	expected += U'\U00012345';

	SECTION("append")
		s.append(U'\U00012345');

	SECTION("push_back")
		s.push_back(U'\U00012345');

	REQUIRE( s==expected );
}

template<class DATATYPE>
inline void testAppend()
{
	SECTION("normal")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");
		

		SECTION("string")
			testAppendWithString<DATATYPE>(s);

		SECTION("numChars")
			testAppendNumChars<DATATYPE>(s);

		SECTION("singleChar")
			testAppendSingleChar<DATATYPE>(s);
	}

	SECTION("slice")
	{
		StringImpl<DATATYPE> s(U"xyhe\U00012345loworldabc");

		SECTION("string")
			testAppendWithString<DATATYPE>( s.subString(2, 10) );

		SECTION("numChars")
			testAppendNumChars<DATATYPE>(s);

		SECTION("singleChar")
			testAppendSingleChar<DATATYPE>(s);
	}
}





template<class StringType, class InsertPosType>
inline void verifyInsert(StringType& s, InsertPosType insertPos, const StringType& toInsert, const StringType& expected)
{
	SECTION("iterators")
	{
		s.insert(insertPos, toInsert.begin(), toInsert.end());
		REQUIRE(s == expected);
	}

	SECTION("iteratorsFromOtherClass")
	{
		std::u32string suf = toInsert.asUtf32();

		s.insert(insertPos, suf.begin(), suf.end());
		REQUIRE(s == expected);
	}

	SECTION("String")
	{
		s.insert(insertPos, toInsert);
		REQUIRE(s == expected);
	}

	SECTION("subString")
	{
		std::u32string sufUtf32 = U"abc" + toInsert.asUtf32() + U"efg";
		StringType suf(sufUtf32);

		s.insert(insertPos, suf, 3, toInsert.getLength());
		REQUIRE(s == expected);
	}

	SECTION("subStringWithoutLength")
	{
		std::u32string sufUtf32 = U"abc" + toInsert.asUtf32();
		StringType suf(sufUtf32);

		s.insert(insertPos, suf, 3);
		REQUIRE(s == expected);
	}

	SECTION("subStringWithLengthTooBig")
	{
		std::u32string sufUtf32 = U"abc" + toInsert.asUtf32();
		StringType suf(sufUtf32);

		s.insert(insertPos, suf, 3, 100);
		REQUIRE(s == expected);
	}

	SECTION("utf8")
	{
		s.insert(insertPos, toInsert.asUtf8());
		REQUIRE(s == expected);
	}

	SECTION("utf8Ptr")
	{
		s.insert(insertPos, toInsert.asUtf8Ptr());
		REQUIRE(s == expected);
	}

	SECTION("utf8PtrWithLength")
	{
		std::string suf = toInsert.asUtf8() + "xyz";

		s.insert(insertPos, suf.c_str(), suf.length() - 3);
		REQUIRE(s == expected);
	}

	SECTION("utf16")
	{
		s.insert(insertPos, toInsert.asUtf16());
		REQUIRE(s == expected);
	}

	SECTION("utf16Ptr")
	{
		s.insert(insertPos, toInsert.asUtf16Ptr());
		REQUIRE(s == expected);
	}

	SECTION("utf16PtrWithLength")
	{
		std::u16string suf = toInsert.asUtf16() + u"xyz";

		s.insert(insertPos, suf.c_str(), suf.length() - 3);
		REQUIRE(s == expected);
	}

	SECTION("utf32")
	{
		s.insert(insertPos, toInsert.asUtf32());
		REQUIRE(s == expected);
	}

	SECTION("utf32Ptr")
	{
		s.insert(insertPos, toInsert.asUtf32Ptr());
		REQUIRE(s == expected);
	}

	SECTION("utf32PtrWithLength")
	{
		std::u32string suf = toInsert.asUtf32() + U"xyz";

		s.insert(insertPos, suf.c_str(), suf.length() - 3);
		REQUIRE(s == expected);
	}

	SECTION("wide")
	{
		s.insert(insertPos, toInsert.asWide());
		REQUIRE(s == expected);
	}

	SECTION("widePtr")
	{
		s.insert(insertPos, toInsert.asWidePtr());
		REQUIRE(s == expected);
	}

	SECTION("widePtrWithLength")
	{
		std::wstring suf = toInsert.asWide() + L"xyz";

		s.insert(insertPos, suf.c_str(), suf.length() - 3);
		REQUIRE(s == expected);
	}

	if (!toInsert.isEmpty())
	{
		if (toInsert == U"BL\U00013333A")
		{
			SECTION("charInitializerList")
			{
				// must leave out the unicode character
				s.insert(insertPos, { 'B', 'L', 'A' });

				std::u32string exp = expected;
				size_t found = exp.find(U'\U00013333');
				if (found != String::npos)
					exp.erase(found, 1);

				REQUIRE(s == exp);
			}

			SECTION("char32InitializerList")
			{
				s.insert(insertPos, { U'B', U'L', U'\U00013333', U'A' });

				REQUIRE(s == expected);
			}
		}
		else
			throw std::runtime_error("Test must be updated for new possible argument toInsert");
	}

}

template<class DATATYPE>
inline void testInsertWithString(StringImpl<DATATYPE>& s, const StringImpl<DATATYPE>& toInsert)
{
	int insertPosArray[] = { 0, 5, 10 };
	int insertPosCount = std::extent<decltype(insertPosArray)>().value;

	for(int i=0; i<insertPosCount; i++)
	{
		int insertPos = insertPosArray[i];

		std::u32string expected = s.asUtf32();

		expected.insert(insertPos, toInsert.asUtf32() );

		SECTION(std::to_string(insertPos))
		{
			SECTION("atIndex")
				verifyInsert<StringImpl<DATATYPE>, int>(s, insertPos, toInsert, expected);

			SECTION("atIterator")
				verifyInsert<StringImpl<DATATYPE>, StringImpl<DATATYPE>::Iterator>(s, s.begin()+insertPos, toInsert, expected);
		}
	}
}


template<class DATATYPE>
inline void testInsertWithString(StringImpl<DATATYPE>& s)
{
	SECTION("empty")
	{
		testInsertWithString< DATATYPE >(s, "");
	}

	SECTION("nonEmpty")
	{
		testInsertWithString< DATATYPE >(s, U"BL\U00013333A" );
	}
}



template<class DATATYPE>
inline void verifyInsertNumChars(StringImpl<DATATYPE>& s, int charCount, char32_t chr)
{
	int insertPosArray[] = { 0, 5, 10 };
	int insertPosCount = std::extent<decltype(insertPosArray)>().value;

	for(int i=0; i<insertPosCount; i++)
	{
		int insertPos = insertPosArray[i];

		SECTION(std::to_string(insertPos))
		{

			std::u32string expected = s.asUtf32();

			expected.insert(insertPos, charCount, chr);

			SECTION("atIndex")
				s.insert(insertPos, charCount, chr);

			SECTION("atIterator")
				s.insert(s.begin()+insertPos, charCount, chr);

			REQUIRE(s == expected);
		}
	}
}


template<class DATATYPE>
inline void testInsertNumChars_WithCharCount(StringImpl<DATATYPE>& s, int charCount)
{
	SECTION("nullChar")
	{
		verifyInsertNumChars<DATATYPE>(s, charCount, U'\0');
	}

	SECTION("asciiChar")
	{
		verifyInsertNumChars<DATATYPE>(s, charCount, U'F');
	}

	SECTION("nonAsciiChar")
	{
		verifyInsertNumChars<DATATYPE>(s, charCount, U'\U00011111');
	}
}

template<class DATATYPE>
inline void testInsertNumChars(StringImpl<DATATYPE>& s)
{
	SECTION("zero chars")
		testInsertNumChars_WithCharCount(s, 0);
	
	SECTION("1 char")
		testInsertNumChars_WithCharCount(s, 1);

	SECTION("7 chars")
		testInsertNumChars_WithCharCount(s, 7);	
}


template<class DATATYPE>
inline void testInsertSingleChar(StringImpl<DATATYPE>& s)
{
	std::u32string expected = s.asUtf32();

	int insertPosArray[] = { 0, 5, 10 };
	int insertPosCount = std::extent<decltype(insertPosArray)>().value;

	for(int i=0; i<insertPosCount; i++)
	{
		int insertPos = insertPosArray[i];

		SECTION(std::to_string(insertPos))
		{
			expected.insert(expected.begin()+insertPos, U'\U00012345');

			SECTION("atIndex")
				s.insert(insertPos, U'\U00012345');

			SECTION("atIterator")
				s.insert(s.begin()+insertPos, U'\U00012345');

			REQUIRE(s == expected);
		}
	}
}






template<class DATATYPE>
inline void testInsert()
{
	SECTION("normal")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");
		
		SECTION("string")
			testInsertWithString<DATATYPE>(s);

		SECTION("numChars")
			testInsertNumChars<DATATYPE>(s);

		SECTION("singleChar")
			testInsertSingleChar<DATATYPE>(s);
	}

	SECTION("slice")
	{
		StringImpl<DATATYPE> s(U"xyhe\U00012345loworldabc");

		SECTION("string")
			testInsertWithString<DATATYPE>(s.subString(2, 10));

		SECTION("numChars")
			testInsertNumChars<DATATYPE>(s.subString(2, 10));

		SECTION("singleChar")
			testInsertSingleChar<DATATYPE>(s.subString(2, 10));
	}
}

template<class DATATYPE>
inline void testEraseWithString(StringImpl<DATATYPE>& s, int atPos, size_t eraseLength)
{
	std::u32string expected = s.asUtf32();

	expected.erase(atPos, eraseLength);

	SECTION("index")
		s.erase(atPos, eraseLength);

	if(eraseLength==1 && atPos<(int)s.getLength())
	{
		SECTION("itOneChar")
		{
			StringImpl<DATATYPE>::Iterator resultIt = s.erase( s.begin()+atPos );

			REQUIRE( resultIt==s.begin()+atPos );
		}
	}

	SECTION("itRange")
	{
		StringImpl<DATATYPE>::Iterator beginIt = s.begin()+atPos;
		
		StringImpl<DATATYPE>::Iterator endIt;
		if(eraseLength==StringImpl<DATATYPE>::npos || atPos+eraseLength>=s.length())
			endIt = s.end();
		else
			endIt = beginIt+eraseLength;

		StringImpl<DATATYPE>::Iterator resultIt = s.erase(beginIt, endIt);	
		REQUIRE( resultIt==s.begin()+atPos );
	}


	REQUIRE( s==expected );
}


template<class DATATYPE>
inline void testEraseWithString(StringImpl<DATATYPE>& s, int atPos)
{
	std::u32string expected = s.asUtf32();

	size_t	lengthArray[] ={ 0, 1, 5, 10, 20, StringImpl<DATATYPE>::npos };
	int		lengthCount = std::extent<decltype(lengthArray)>().value;
	for(int i=0; i<lengthCount; i++)
	{
		size_t length = lengthArray[i];

		SECTION("length"+std::to_string(length))
			testEraseWithString<DATATYPE>(s, atPos, length);
	}
}


template<class DATATYPE>
inline void testEraseWithString(StringImpl<DATATYPE>& s)
{
	std::u32string expected = s.asUtf32();

	int atArray[] = { 0, 5, 10 };
	int atCount = std::extent<decltype(atArray)>().value;
	for(int i=0; i<atCount; i++)
	{
		int atPos = atArray[i];

		SECTION("at"+std::to_string(atPos))
			testEraseWithString<DATATYPE>(s, atPos);
	}
}


template<class DATATYPE>
inline void testErase()
{
	SECTION("normal")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");
		
		testEraseWithString<DATATYPE>(s);
	}

	SECTION("slice")
	{
		StringImpl<DATATYPE> s(U"xyhe\U00012345loworldabc");

		testEraseWithString<DATATYPE>(s.subString(2, 10));
	}
}


template<class DATATYPE, class ARG>
inline void verifyAssignFull(StringImpl<DATATYPE>& s, ARG arg)
{
	SECTION("assignFunction")
		s.assign(arg);

	SECTION("operator=")
		s = arg;	
}

template<class DATATYPE>
inline void testAssignFull(StringImpl<DATATYPE>& s, const StringImpl<DATATYPE>& source)
{
	std::u32string origSource = source.asUtf32();

	SECTION("String")
		verifyAssignFull(s, source);

	SECTION("utf8")
		verifyAssignFull(s, source.asUtf8());
	SECTION("utf8ptr")
		verifyAssignFull(s, source.asUtf8().c_str());

	SECTION("wide")
		verifyAssignFull(s, source.asWide());
	SECTION("wideptr")
		verifyAssignFull(s, source.asWide().c_str());

	SECTION("utf16")
		verifyAssignFull(s, source.asUtf16());
	SECTION("utf16ptr")
		verifyAssignFull(s, source.asUtf16().c_str());

	SECTION("utf32")
		verifyAssignFull(s, source.asUtf32());
	SECTION("utf32ptr")
		verifyAssignFull(s, source.asUtf32().c_str());


	REQUIRE(s==source);
	REQUIRE(source==origSource);
}

template<class DATATYPE>
inline void testAssignWithString(StringImpl<DATATYPE>& s, const StringImpl<DATATYPE>& source)
{
	SECTION("full")
		testAssignFull(s, source);

	SECTION("move")
	{
		std::u32string			origSource = source.asUtf32();
		StringImpl<DATATYPE>	moveSource(source);

		SECTION("assignFunction")				
			s.assign( std::move(moveSource) );

		SECTION("operator=")
			s = std::move(moveSource);

		// moveSource should be empty afterwards
		REQUIRE( moveSource.isEmpty() );
		REQUIRE( moveSource.getLength()==0 );
		REQUIRE( moveSource=="" );
		REQUIRE( moveSource==U"" );

		REQUIRE( s==source );
		REQUIRE( s.getLength()==source.getLength() );

		REQUIRE( source==origSource );
	}

	if(source.getLength()>=7)
	{
		SECTION("subString")
		{
			std::u32string origSource = source.asUtf32();

			s.assign(source, 3, 4);

			REQUIRE( source==origSource );

			REQUIRE( s==source.subString(3,4) );			
		}
	}

	SECTION("iterators")
	{
		std::u32string origSource = source.asUtf32();

		s.assign(source.begin(), source.end());

		REQUIRE( source==origSource );

		REQUIRE( s==source );			
	}

	if(source.getLength()>2)
	{
		SECTION("ptrWithLength")
		{
			std::u32string origSource = source.asUtf32();

			SECTION("utf8PtrWithLength")	
				s.assign(source.asUtf8().c_str(), 2);			

			SECTION("widePtrWithLength")		
				s.assign(source.asWide().c_str(), 2);			

			SECTION("utf16PtrWithLength")		
				s.assign(source.asUtf16().c_str(), 2);			

			SECTION("utf32PtrWithLength")		
				s.assign(source.asUtf32().c_str(), 2);			

			REQUIRE(source==origSource);

			REQUIRE(s==source.subString(0, 2) );
		}
	}
}

template<class DATATYPE>
inline void testAssignWithString(StringImpl<DATATYPE>& s)
{
	SECTION("sourceEmpty")
		testAssignWithString<DATATYPE>(s, U"");

	SECTION("sourceNonEmpty")
		testAssignWithString<DATATYPE>(s, U"");

	SECTION("sourceEmptySlice")
	{
		StringImpl<DATATYPE> x(U"worldhe\U00012345lo");

		testAssignWithString<DATATYPE>(s, x.subString(2, 0) );
	}

	SECTION("sourceNonEmptySlice")
	{
		StringImpl<DATATYPE> x(U"worldhe\U00012345lo");

		testAssignWithString(s, x.subString(2, 5) );
	}

	SECTION("numChars")
	{
		std::vector<int> numCharArray( {0, 1, 5, 20} );
		for(auto it = numCharArray.begin(); it!=numCharArray.end(); ++it)
		{
			int numChars = *it;

			SECTION(std::to_string(numChars))
			{
				s.assign(numChars, U'\U00012345');

				std::u32string expected;
				expected.assign(numChars, U'\U00012345');

				REQUIRE( s==expected );
			}
		}
	}

	SECTION("initializerList")
	{
		SECTION("assignFunction")
			s.assign( {'a', 'b', U'\U00012345'} );
		SECTION("operator=")
			s = {'a', 'b', U'\U00012345'};
			
		std::u32string expected;
		expected.assign( {'a', 'b', U'\U00012345'} );
		
		REQUIRE( s==expected );
	}	

}


template<class DATATYPE>
inline void testAssign()
{
	SECTION("destNotEmpty")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");
		
		testAssignWithString<DATATYPE>(s);
	}

	SECTION("destEmpty")
	{
		StringImpl<DATATYPE> s;
		
		testAssignWithString<DATATYPE>(s);
	}

	SECTION("destNotEmpty-Slice")
	{
		StringImpl<DATATYPE> s(U"xyhe\U00012345loworldabc");

		testAssignWithString<DATATYPE>(s.subString(2, 10));

		// the slice source should not have been modified
		REQUIRE( s==U"xyhe\U00012345loworldabc" );
	}

	SECTION("destEmpty-Slice")
	{
		StringImpl<DATATYPE> s(U"xyhe\U00012345loworldabc");

		testAssignWithString<DATATYPE>(s.subString(2, 0));

		// the slice source should not have been modified
		REQUIRE( s==U"xyhe\U00012345loworldabc" );
	}
}



template<class DATATYPE>
inline void verifyResizeResult(const StringImpl<DATATYPE>& s, int expectedLength, const char32_t* expected)
{
	REQUIRE( s.getLength()==expectedLength );

	auto it = s.begin();
	for(int i=0; i<expectedLength; i++)
	{
		REQUIRE( *it == expected[i] );

		++it;
	}

	REQUIRE( it==s.end() );
}


template<class DATATYPE>
inline void verifyResize(StringImpl<DATATYPE>& s)
{
	SECTION("same")
	{
		s.resize(5);

		verifyResizeResult(s, 5, U"hello");
	}

	SECTION("smaller")
	{
		s.resize(3);

		verifyResizeResult(s, 3, U"hel");
	}

	SECTION("biggerWithDefaultPadding")
	{
		s.resize(10);

		verifyResizeResult(s, 10, U"hello\0\0\0\0\0");		
	}

	SECTION("biggerWithAsciiPaddingDefaultPadding")
	{
		s.resize(10, U'Z');

		verifyResizeResult(s, 10, U"helloZZZZZ");
	}

	SECTION("biggerWithNonPaddingDefaultPadding")
	{
		s.resize(10, U'\U00012345');

		verifyResizeResult(s, 10, U"hello\U00012345\U00012345\U00012345\U00012345\U00012345");
	}
}

template<class DATATYPE>
inline void testResize()
{
	SECTION("normal")
	{
		StringImpl<DATATYPE> s("hello");

		verifyResize(s);
	}

	SECTION("slice")
	{
		StringImpl<DATATYPE> source("hihelloworld");		
		StringImpl<DATATYPE> s = source.subString(2, 5);

		verifyResize(s);

		// source should not have been modified
		REQUIRE( source=="hihelloworld" );
	}
}


template<class DATATYPE>
inline void testSwapWithAB(StringImpl<DATATYPE>& a, StringImpl<DATATYPE>& b)
{
	std::u32string origA = a.asUtf32();
	std::u32string origB = b.asUtf32();

	SECTION("a.swap(b)")
	{
		a.swap(b);
	}

	SECTION("b.swap(a)")
	{
		b.swap(a);
	}

	REQUIRE( a==origB );
	REQUIRE( b==origA );
}


template<class DATATYPE>
inline void testSwapWithA(StringImpl<DATATYPE>& a)
{
	SECTION("B-normal")
	{
		StringImpl<DATATYPE> b(U"wo\U00012345rld");

		testSwapWithAB<DATATYPE>(a, b);		
	}

	SECTION("B-slice")
	{
		StringImpl<DATATYPE> b(U"xxwo\U00012345rldyy");

		StringImpl<DATATYPE> b2 = b.subString(2, 3);

		testSwapWithAB<DATATYPE>( a, b2 );		
	}

	SECTION("B-empty")
	{
		StringImpl<DATATYPE> b;

		testSwapWithAB<DATATYPE>( a, b );
	}	

	SECTION("B-emptySlice")
	{
		StringImpl<DATATYPE> b(U"xxwo\U00012345rldyy");

		StringImpl<DATATYPE> b2 = b.subString(2, 0);

		testSwapWithAB<DATATYPE>( a, b2 );		
	}
	
}

template<class DATATYPE>
inline void testSwap()
{
	SECTION("A-normal")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");

		testSwapWithA<DATATYPE>(s);		
	}

	SECTION("A-slice")
	{
		StringImpl<DATATYPE> s(U"xxhe\U00012345loworldyy");

		testSwapWithA<DATATYPE>( s.subString(2, 10) );		
	}

	SECTION("A-empty")
	{
		StringImpl<DATATYPE> s;

		testSwapWithA<DATATYPE>( s );
	}	

	SECTION("A-emptySlice")
	{
		StringImpl<DATATYPE> s(U"xxhe\U00012345loworldyy");

		testSwapWithA<DATATYPE>( s.subString(2, 0) );		
	}
}


template<class DATATYPE>
inline void testRemoveLast(StringImpl<DATATYPE>& s)
{
	std::u32string expected=s;

	if(!expected.empty())
		expected.erase( expected.end()-1 );

	SECTION("actualRemoveLast")
		s.removeLast();

	SECTION("pop_back")
		s.pop_back();

	REQUIRE(s==expected);
}

template<class DATATYPE>
inline void testRemoveLast()
{
	SECTION("normal")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");

		testRemoveLast<DATATYPE>(s);		
	}

	SECTION("slice")
	{
		StringImpl<DATATYPE> s(U"xxhe\U00012345loworldyy");

		testRemoveLast<DATATYPE>( s.subString(2, 10) );		
	}

	SECTION("empty")
	{
		StringImpl<DATATYPE> s;

		testRemoveLast<DATATYPE>( s );
	}	

	SECTION("emptySlice")
	{
		StringImpl<DATATYPE> s(U"xxhe\U00012345loworldyy");

		testRemoveLast<DATATYPE>( s.subString(2, 0) );		
	}

}


template<class DATATYPE>
inline void testReserveCapacity()
{
	StringImpl<DATATYPE> s;

	SECTION("empty")
	{
		SECTION("initial")
			REQUIRE(s.capacity()==0);

		SECTION("reserve")
		{
			s.reserve(20);
			REQUIRE( s.capacity()>=20 );
			REQUIRE( s.capacity()<=30 );	// sanity check - not actually forbidden to have bigger values
		}
	}

	SECTION("nonEmpty")
	{
		s = U"he\U00012345loworld";

		SECTION("initial")
		{
			size_t cap = s.capacity();
			REQUIRE( s.capacity()>=10 );
			REQUIRE( s.capacity()<=16 );	// sanity check - not actually forbidden to have bigger values
		}

		SECTION("reserve")
		{
			s.reserve(20);
			REQUIRE( s.capacity()>=20 );
			REQUIRE( s.capacity()<=30 );	// sanity check - not actually forbidden to have bigger values
		}

		SECTION("reserveReduce")
		{
			s.reserve(20);

			size_t cap = s.capacity();
			REQUIRE( cap>=20 );
			REQUIRE( cap<=30 );	// sanity check - not actually forbidden to have bigger values
			
			s.reserve(15);

			// the second call should have had no effect, since the second value is bigger than the length.
			REQUIRE( s.capacity()==cap );
		}

		SECTION("reserveReduceToLess")
		{
			s.reserve(20);
			size_t cap = s.capacity();

			s.reserve(5);

			// the request to reduce the space is actually non-binding. So all we can test that it either stayed the same or got reduced.			
			REQUIRE( s.capacity()<=cap );
			REQUIRE( s.capacity()>=10 );
		}


		SECTION("capacityWithCopies")
		{
			s.reserve(20);	

			size_t cap = s.capacity();
			REQUIRE( cap>=20 );
			REQUIRE( cap<=30 );	// sanity check - not actually forbidden to have bigger values

			{
				StringImpl<DATATYPE> s2 = s;

				// the capacity should equal the string length again, since the data is now shared between two objects
				REQUIRE( (s.capacity()==10 || s.capacity()==11) );
				REQUIRE( (s2.capacity()==10 || s2.capacity()==11) );
			}

			// now the capacity should be reported as 20 again
			REQUIRE( s.capacity()==cap );
		}

		SECTION("reserveWithCopies")
		{
			{
				size_t initialCap = s.capacity();

				StringImpl<DATATYPE> s2 = s;

				// data is shared. So capacity should be reported as exactly the length
				REQUIRE( s2.capacity()==10 );
				REQUIRE( s.capacity()==10 );

				s.reserve(20);			
				REQUIRE( s.capacity()>=20 );
				REQUIRE( s.capacity()<=30 );	// sanity check - not actually forbidden to have bigger values

				// the reserve call should have created its own copy of the data for s.
				// So the capacity of s2 should still be 10.
				REQUIRE( s2.capacity()==initialCap );
			}

			REQUIRE( s.capacity()>=20 );
			REQUIRE( s.capacity()<=30 );	// sanity check - not actually forbidden to have bigger values
		}

		REQUIRE( s == U"he\U00012345loworld" );
	}
}



template<class DATATYPE>
inline void testGetAllocator()
{
	StringImpl<DATATYPE> s;

	// there is not much that we can test here, other than that the functions do not crash.

	StringImpl<DATATYPE>::Allocator alloc = s.getAllocator();

	StringImpl<DATATYPE>::Allocator alloc2 = s.get_allocator();
}



template<class DATATYPE>
inline void testCopy(StringImpl<DATATYPE>& s)
{
	std::u32string u32 = s.asUtf32();

	char32_t buffer[30];
	char32_t expectedBuffer[30];

	memset(buffer, 0xffffffff, 30*sizeof(char32_t) );
	memset(expectedBuffer, 0xffffffff, 30*sizeof(char32_t) );

	size_t result = StringImpl<DATATYPE>::npos;
	size_t expectedResult = StringImpl<DATATYPE>::npos;

	SECTION("zeroFromStart")
	{
		result = s.copy(buffer, 0);
		expectedResult = u32.copy(expectedBuffer, 0);
	}

	SECTION("zeroFromEnd")
	{
		result = s.copy(buffer, 0, s.getLength());
		expectedResult = u32.copy(expectedBuffer, 0, s.getLength());
	}

	SECTION("zeroFromMiddle")
	{
		result = s.copy(buffer, 0, s.getLength()/2);
		expectedResult = u32.copy(expectedBuffer, 0, s.getLength()/2);
	}


	SECTION("halfFromStart")
	{
		result = s.copy(buffer, s.getLength()/2, 0);
		expectedResult = u32.copy(expectedBuffer, s.getLength()/2, 0);
	}

	SECTION("halfFromEnd")
	{
		result = s.copy(buffer, s.getLength()/2, s.getLength());
		expectedResult = u32.copy(expectedBuffer, s.getLength()/2, s.getLength());
	}

	SECTION("halfFromMiddle")
	{
		result = s.copy(buffer, s.getLength()/2, s.getLength()/2);
		expectedResult = u32.copy(expectedBuffer, s.getLength()/2, s.getLength()/2);
	}


	SECTION("fullFromStart")
	{
		result = s.copy(buffer, s.getLength(), 0);
		expectedResult = u32.copy(expectedBuffer, s.getLength(), 0);
	}

	SECTION("fullFromEnd")
	{
		result = s.copy(buffer, s.getLength(), s.getLength());
		expectedResult = u32.copy(expectedBuffer, s.getLength(), s.getLength());
	}

	SECTION("fullFromMiddle")
	{
		result = s.copy(buffer, s.getLength(), s.getLength()/2);
		expectedResult = u32.copy(expectedBuffer, s.getLength(), s.getLength()/2);
	}



	SECTION("moreThanFullFromStart")
	{
		result = s.copy(buffer, s.getLength()+10, 0);
		expectedResult = u32.copy(expectedBuffer, s.getLength()+10, 0);
	}

	SECTION("moreThanFullFromEnd")
	{
		result = s.copy(buffer, s.getLength()+10, s.getLength());
		expectedResult = u32.copy(expectedBuffer, s.getLength()+10, s.getLength());
	}

	SECTION("moreThanFullFromMiddle")
	{
		result = s.copy(buffer, s.getLength()+10, s.getLength()/2);
		expectedResult = u32.copy(expectedBuffer, s.getLength()+10, s.getLength()/2);
	}


	SECTION("nposFromStart")
	{
		result = s.copy(buffer, s.npos, 0);
		expectedResult = u32.copy(expectedBuffer, s.npos, 0);
	}

	SECTION("nposFromEnd")
	{
		result = s.copy(buffer, s.npos, s.getLength());
		expectedResult = u32.copy(expectedBuffer, s.npos, s.getLength());
	}

	SECTION("nposFromMiddle")
	{
		result = s.copy(buffer, s.npos, s.getLength()/2);
		expectedResult = u32.copy(expectedBuffer, s.npos, s.getLength()/2);
	}


	REQUIRE( result==expectedResult );
	
	for(int i=0; i<30; i++)
	{
		REQUIRE( buffer[i] == expectedBuffer[i] );
	}
}

template<class DATATYPE>
inline void testCopy()
{
	SECTION("empty")
	{
		StringImpl<DATATYPE> s;	
		testCopy<DATATYPE>(s);
	}

	SECTION("nonEmpty")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");	

		testCopy<DATATYPE>(s);
	}

	SECTION("badStartIndex")
	{
		StringImpl<DATATYPE> s(U"he\U00012345loworld");	

		char32_t buffer[1];

		REQUIRE_THROWS_AS( s.copy(buffer, 0, s.getLength()+1), OutOfRangeError );

		REQUIRE_THROWS_AS( s.copy(buffer, 1, s.getLength()+1), OutOfRangeError );
	}
}


template<class DATATYPE>
inline void testIteratorWithIndex()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");	

	StringImpl<DATATYPE>::IteratorWithIndex it(s.begin(), 17);

	REQUIRE( it.getIndex() == 17);
	it++;
	REQUIRE( it.getIndex() == 18);

	it+=3;
	REQUIRE( it.getIndex() == 21);

	REQUIRE( it.getInner() == s.begin()+4);

	REQUIRE( *it == *(s.begin()+4) );

	it-=2;
	REQUIRE( it.getIndex() == 19);

	it = it+1;
	REQUIRE( it.getIndex() == 20);

	it = it+3;
	REQUIRE( it.getIndex() == 23);

	it = it-2;
	REQUIRE( it.getIndex() == 21);

	it = it-1;
	REQUIRE( it.getIndex() == 20);

	++it;
	REQUIRE( it.getIndex() == 21);

	--it;
	REQUIRE( it.getIndex() == 20);

	REQUIRE( (it++).getIndex() == 20);

	REQUIRE( it.getIndex() == 21);

	REQUIRE( (it--).getIndex() == 21);

	REQUIRE( it.getIndex() == 20);

	it-=3;
	REQUIRE( it.getIndex() == 17);

	REQUIRE( it.getInner() == s.begin() );

	REQUIRE( *it == *s.begin() );
}



template<class STRINGTYPE, class ToFindStringType>
inline void testFindStringFromIndex()
{
	STRINGTYPE s(U"he\U00012345loworld");	
	STRINGTYPE empty(U"");	

	STRINGTYPE toFindObj(U"\U00012345lo");	
	STRINGTYPE toFindNonMatchObj(U"\U00012345lO");	
	STRINGTYPE toFindEmptyObj(U"");	

	ToFindStringType toFind = (ToFindStringType)toFindObj;
	ToFindStringType toFindNonMatch = (ToFindStringType)toFindNonMatchObj;
	ToFindStringType toFindEmpty = (ToFindStringType)toFindEmptyObj;


	SECTION("fromStart-matching")
	{
		size_t result = s.find(toFind, 0);
		REQUIRE( result==2 );
	}

	SECTION("fromStart-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 0);
		REQUIRE( result==s.npos );
	}


	SECTION("fromStartUsingDefaultArg-matching")
	{
		size_t result = s.find(toFind);
		REQUIRE( result==2 );
	}

	SECTION("fromStartUsingDefaultArg-notMatching")
	{
		size_t result = s.find(toFindNonMatch);
		REQUIRE( result==s.npos );
	}


	SECTION("fromMatchPos-matching")
	{
		size_t result = s.find(toFind, 2 );
		REQUIRE( result==2 );
	}

	SECTION("fromMatchPos-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 2 );
		REQUIRE( result==s.npos );
	}


	SECTION("fromAfterMatchPos-matching")
	{
		size_t result = s.find(toFind, 3 );
		REQUIRE( result == s.npos );
	}

	SECTION("fromAfterMatchPos-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 3 );
		REQUIRE( result == s.npos );
	}



	SECTION("fromEnd-matching")
	{
		size_t result = s.find(toFind, s.length() );
		REQUIRE( result == s.npos );
	}

	SECTION("fromEnd-notMatching")
	{
		size_t result = s.find(toFindNonMatch, s.length() );
		REQUIRE( result == s.npos );
	}


	SECTION("fromAfterEnd-matching")
	{
		size_t result = s.find(toFind, s.length()+1 );
		REQUIRE( result==s.npos );
	}

	SECTION("fromAfterEnd-notMatching")
	{
		size_t result = s.find(toFindNonMatch, s.length()+1 );
		REQUIRE( result==s.npos );
	}

	SECTION("fromAfterEnd-empty")
	{
		size_t result = s.find(toFindEmpty, s.length()+1 );
		REQUIRE( result==s.npos );
	}


	SECTION("empty-fromStart")
	{
		size_t result = s.find(toFindEmpty, 0 );
		REQUIRE( result==0 );
	}

	SECTION("empty-fromMiddle")
	{
		size_t result = s.find(toFindEmpty, 5 );
		REQUIRE( result==5 );
	}

	SECTION("empty-fromEnd")
	{
		size_t result = s.find(toFindEmpty, s.length() );
		REQUIRE( result==s.length() );
	}


	SECTION("notEmpty-inEmpty")
	{
		size_t result = empty.find(toFind, 0 );
		REQUIRE( result==empty.npos );
	}

	SECTION("empty-inEmpty")
	{
		size_t result = empty.find(toFindEmpty, 0 );
		REQUIRE( result==0 );
	}


	SECTION("withMultipleMatches")
	{
		STRINGTYPE s2 = s;
		s2 += s;

		SECTION("fromStart")
		{
			size_t result = s2.find(toFind, 0 );
			REQUIRE( result==2 );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = s2.find(toFind, 2 );
			REQUIRE( result==2 );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = s2.find(toFind, 3 );
			REQUIRE( result==12 );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = s2.find(toFind, 11 );
			REQUIRE( result==12 );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = s2.find(toFind, 12 );
			REQUIRE( result==12 );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			size_t result = s2.find(toFind, 13 );
			REQUIRE( result==s2.npos );
		}

	}
}


template<class PType>
int getCStringLength(PType s)
{
	int length=0;
	while(s[length]!=0)
		length++;
	return length;
}


template<class STRINGTYPE, class ToFindStringType>
inline void testFindStringWithLengthFromIndex()
{
	STRINGTYPE s(U"he\U00012345loworld");	
	STRINGTYPE empty(U"");	

	STRINGTYPE toFindObj(U"\U00012345lo");	
	STRINGTYPE toFindNonMatchObj(U"\U00012345lO");	
	STRINGTYPE toFindEmptyObj(U"");	

	ToFindStringType toFind = (ToFindStringType)toFindObj;
	ToFindStringType toFindNonMatch = (ToFindStringType)toFindNonMatchObj;
	ToFindStringType toFindEmpty = (ToFindStringType)toFindEmptyObj;

	size_t toFindLength = getCStringLength<ToFindStringType>(toFind);
	size_t toFindNonMatchLength = getCStringLength<ToFindStringType>(toFindNonMatch);
	size_t toFindEmptyLength = getCStringLength<ToFindStringType>(toFindEmpty);

	toFindObj += U"xyz";
	toFindNonMatchObj += U"xyz";
	toFindEmptyObj += U"xyz";

	toFind = (ToFindStringType)toFindObj;
	toFindNonMatch = (ToFindStringType)toFindNonMatchObj;
	toFindEmpty = (ToFindStringType)toFindEmptyObj;	


	SECTION("fromStart-matching")
	{
		size_t result = s.find(toFind, 0, toFindLength);
		REQUIRE( result==2 );
	}

	SECTION("fromStart-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 0, toFindNonMatchLength);
		REQUIRE( result==s.npos );
	}

	SECTION("fromMatchPos-matching")
	{
		size_t result = s.find(toFind, 2, toFindLength );
		REQUIRE( result==2 );
	}

	SECTION("fromMatchPos-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 2, toFindNonMatchLength );
		REQUIRE( result==s.npos );
	}


	SECTION("fromAfterMatchPos-matching")
	{
		size_t result = s.find(toFind, 3, toFindLength );
		REQUIRE( result == s.npos );
	}

	SECTION("fromAfterMatchPos-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 3, toFindNonMatchLength );
		REQUIRE( result == s.npos );
	}



	SECTION("fromEnd-matching")
	{
		size_t result = s.find(toFind, s.length(), toFindLength );
		REQUIRE( result == s.npos );
	}

	SECTION("fromEnd-notMatching")
	{
		size_t result = s.find(toFindNonMatch, s.length(), toFindNonMatchLength );
		REQUIRE( result == s.npos );
	}


	SECTION("fromAfterEnd-matching")
	{
		size_t result = s.find(toFind, s.length()+1, toFindLength );
		REQUIRE( result==s.npos );
	}

	SECTION("fromAfterEnd-notMatching")
	{
		size_t result = s.find(toFindNonMatch, s.length()+1, toFindNonMatchLength );
		REQUIRE( result==s.npos );
	}

	SECTION("fromAfterEnd-empty")
	{
		size_t result = s.find(toFindEmpty, s.length()+1, toFindEmptyLength );
		REQUIRE( result==s.npos );
	}


	SECTION("empty-fromStart")
	{
		size_t result = s.find(toFindEmpty, 0, toFindEmptyLength );
		REQUIRE( result==0 );
	}

	SECTION("empty-fromMiddle")
	{
		size_t result = s.find(toFindEmpty, 5, toFindEmptyLength );
		REQUIRE( result==5 );
	}

	SECTION("empty-fromEnd")
	{
		size_t result = s.find(toFindEmpty, s.length(), toFindEmptyLength );
		REQUIRE( result==s.length() );
	}


	SECTION("notEmpty-inEmpty")
	{
		size_t result = empty.find(toFind, 0, toFindLength );
		REQUIRE( result==empty.npos );
	}

	SECTION("empty-inEmpty")
	{
		size_t result = empty.find(toFindEmpty, 0, toFindEmptyLength );
		REQUIRE( result==0 );
	}


	SECTION("withMultipleMatches")
	{
		STRINGTYPE s2 = s;
		s2 += s;

		SECTION("fromStart")
		{
			size_t result = s2.find(toFind, 0, toFindLength );
			REQUIRE( result==2 );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = s2.find(toFind, 2, toFindLength );
			REQUIRE( result==2 );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = s2.find(toFind, 3, toFindLength );
			REQUIRE( result==12 );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = s2.find(toFind, 11, toFindLength );
			REQUIRE( result==12 );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = s2.find(toFind, 12, toFindLength );
			REQUIRE( result==12 );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			size_t result = s2.find(toFind, 13, toFindLength );
			REQUIRE( result==s2.npos );
		}

	}
}



template<class STRINGTYPE>
inline void testFindCharFromIterator()
{
	STRINGTYPE s(U"he\U00012345loworld");	
	STRINGTYPE empty(U"");	

	char32_t toFind = U'\U00012345';
	char32_t toFindNonMatch = 'x';

	SECTION("fromStart-matching")
	{
		STRINGTYPE::Iterator it = s.find(toFind, s.begin());
		REQUIRE( it==s.begin()+2 );
	}

	SECTION("fromStart-notMatching")
	{
		STRINGTYPE::Iterator it = s.find(toFindNonMatch, s.begin());
		REQUIRE( it==s.end() );
	}

	
	SECTION("fromMatchPos-matching")
	{
		STRINGTYPE::Iterator it = s.find(toFind, s.begin()+2 );
		REQUIRE( it==s.begin()+2 );
	}

	SECTION("fromMatchPos-notMatching")
	{
		STRINGTYPE::Iterator it = s.find(toFindNonMatch, s.begin()+2 );
		REQUIRE( it==s.end() );
	}


	SECTION("fromAfterMatchPos-matching")
	{
		STRINGTYPE::Iterator it = s.find(toFind, s.begin()+3 );
		REQUIRE( it == s.end() );
	}

	SECTION("fromAfterMatchPos-notMatching")
	{
		STRINGTYPE::Iterator it = s.find(toFindNonMatch, s.begin()+3 );
		REQUIRE( it == s.end() );
	}



	SECTION("fromEnd-matching")
	{
		STRINGTYPE::Iterator it = s.find(toFind, s.end() );
		REQUIRE( it == s.end() );
	}

	SECTION("fromEnd-notMatching")
	{
		STRINGTYPE::Iterator it = s.find(toFindNonMatch, s.end() );
		REQUIRE( it == s.end() );
	}

	
	SECTION("inEmpty")
	{
		STRINGTYPE::Iterator it = empty.find(toFind, empty.begin() );
		REQUIRE( it==empty.end() );
	}



	SECTION("withMultipleMatches")
	{
		STRINGTYPE s2 = s;
		s2 += s;

		SECTION("fromStart")
		{
			STRINGTYPE::Iterator it = s2.find(toFind, s2.begin() );
			REQUIRE( it==s2.begin()+2 );
		}

		SECTION("fromFirstMatch")
		{
			STRINGTYPE::Iterator it = s2.find(toFind, s2.begin()+2 );
			REQUIRE( it==s2.begin()+2 );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			STRINGTYPE::Iterator it = s2.find(toFind, s2.begin()+3 );
			REQUIRE( it==s2.begin()+12 );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			STRINGTYPE::Iterator it = s2.find(toFind, s2.begin()+11 );
			REQUIRE( it==s2.begin()+12 );
		}

		SECTION("fromSecondMatch")
		{
			STRINGTYPE::Iterator it = s2.find(toFind, s2.begin()+12 );
			REQUIRE( it==s2.begin()+12 );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			STRINGTYPE::Iterator it = s2.find(toFind, s2.begin()+13 );
			REQUIRE( it==s2.end() );
		}
	}

}


template<class STRINGTYPE>
inline void testFindCharFromIndex()
{
	STRINGTYPE s(U"he\U00012345loworld");	
	STRINGTYPE empty(U"");	

	char32_t toFind = U'\U00012345';
	char32_t toFindNonMatch = 'x';

	SECTION("fromStart-matching")
	{
		size_t result = s.find(toFind, 0);
		REQUIRE( result==2 );

		REQUIRE( s.find_first_of(toFind, 0) == result );
	}

	SECTION("fromStart-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 0);
		REQUIRE( result==s.npos );

		REQUIRE( s.find_first_of(toFindNonMatch, 0) == result );
	}


	SECTION("fromStartUsingDefaultArg-matching")
	{
		size_t result = s.find(toFind);
		REQUIRE( result==2 );

		REQUIRE( s.find_first_of(toFind) == result );
	}

	SECTION("fromStartUsingDefaultArg-notMatching")
	{
		size_t result = s.find(toFindNonMatch);
		REQUIRE( result==s.npos );

		REQUIRE( s.find_first_of(toFindNonMatch) == result );
	}


	SECTION("fromMatchPos-matching")
	{
		size_t result = s.find(toFind, 2 );
		REQUIRE( result==2 );

		REQUIRE( s.find_first_of(toFind, 2) == result );
	}

	SECTION("fromMatchPos-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 2 );
		REQUIRE( result==s.npos );

		REQUIRE( s.find_first_of(toFindNonMatch, 2) == result );
	}


	SECTION("fromAfterMatchPos-matching")
	{
		size_t result = s.find(toFind, 3 );
		REQUIRE( result == s.npos );

		REQUIRE( s.find_first_of(toFind, 3) == result );
	}

	SECTION("fromAfterMatchPos-notMatching")
	{
		size_t result = s.find(toFindNonMatch, 3 );
		REQUIRE( result == s.npos );

		REQUIRE( s.find_first_of(toFindNonMatch, 3) == result );
	}



	SECTION("fromEnd-matching")
	{
		size_t result = s.find(toFind, s.length() );
		REQUIRE( result == s.npos );

		REQUIRE( s.find_first_of(toFind, s.length()) == result );
	}

	SECTION("fromEnd-notMatching")
	{
		size_t result = s.find(toFindNonMatch, s.length() );
		REQUIRE( result == s.npos );

		REQUIRE( s.find_first_of(toFindNonMatch, s.length()) == result );
	}


	SECTION("fromAfterEnd-matching")
	{
		size_t result = s.find(toFind, s.length()+1 );
		REQUIRE( result==s.npos );

		REQUIRE( s.find_first_of(toFind, s.length()+1) == result );
	}

	SECTION("fromAfterEnd-notMatching")
	{
		size_t result = s.find(toFindNonMatch, s.length()+1 );
		REQUIRE( result==s.npos );

		REQUIRE( s.find_first_of(toFindNonMatch, s.length()+1) == result );
	}

		
	SECTION("inEmpty")
	{
		size_t result = empty.find(toFind, 0 );
		REQUIRE( result==empty.npos );

		REQUIRE( empty.find_first_of(toFind, 0) == result );
	}



	SECTION("withMultipleMatches")
	{
		STRINGTYPE s2 = s;
		s2 += s;

		SECTION("fromStart")
		{
			size_t result = s2.find(toFind, 0 );
			REQUIRE( result==2 );

			REQUIRE( s2.find_first_of(toFind, 0) == result );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = s2.find(toFind, 2 );
			REQUIRE( result==2 );

			REQUIRE( s2.find_first_of(toFind, 2) == result );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = s2.find(toFind, 3 );
			REQUIRE( result==12 );

			REQUIRE( s2.find_first_of(toFind, 3) == result );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = s2.find(toFind, 11 );
			REQUIRE( result==12 );

			REQUIRE( s2.find_first_of(toFind, 11) == result );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = s2.find(toFind, 12 );
			REQUIRE( result==12 );

			REQUIRE( s2.find_first_of(toFind, 12) == result );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			size_t result = s2.find(toFind, 13 );
			REQUIRE( result==s2.npos );

			REQUIRE( s2.find_first_of(toFind, 13) == result );
		}
	}

}


template<class DATATYPE>
inline void testFindStringFromIt()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");	
	StringImpl<DATATYPE> toFind(U"\U00012345lo");	
	StringImpl<DATATYPE> toFindNonMatch(U"\U00012345lO");	

	for(int withMatchEndIt=0; withMatchEndIt<2; withMatchEndIt++)
	{
		SECTION( withMatchEndIt==1 ? "withMatchEndIt" : "noMatchEndIt" )
		{
			StringImpl<DATATYPE>::Iterator* pMatchEndIt = nullptr;

			StringImpl<DATATYPE>::Iterator matchEndIt;
				
			if(withMatchEndIt==1)
				pMatchEndIt = &matchEndIt;

			SECTION("fromStart-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind, s.begin(), pMatchEndIt);
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}

			SECTION("fromStart-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFindNonMatch, s.begin(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind, s.begin()+2, pMatchEndIt );
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}

			SECTION("fromMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFindNonMatch, s.begin()+2, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}


			SECTION("fromAfterMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind, s.begin()+3, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromAfterMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFindNonMatch, s.begin()+3, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end()  );
			}



			SECTION("fromEnd-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind, s.end(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromEnd-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFindNonMatch, s.end(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}



			SECTION("empty-fromStart")
			{
				StringImpl<DATATYPE> empty;

				StringImpl<DATATYPE>::Iterator it = s.find(empty, s.begin(), pMatchEndIt );
				REQUIRE( it==s.begin() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}


			SECTION("empty-fromMiddle")
			{
				StringImpl<DATATYPE> empty;

				StringImpl<DATATYPE>::Iterator it = s.find(empty, s.begin()+5, pMatchEndIt );
				REQUIRE( it==s.begin()+5 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}

			SECTION("empty-fromEnd")
			{
				StringImpl<DATATYPE> empty;

				StringImpl<DATATYPE>::Iterator it = s.find(empty, s.end(), pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}


			SECTION("notEmpty-inEmpty")
			{
				StringImpl<DATATYPE> empty;

				StringImpl<DATATYPE>::Iterator it = empty.find(toFind, empty.begin(), pMatchEndIt );
				REQUIRE( it==empty.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == empty.end() );
			}

			SECTION("empty-inEmpty")
			{
				StringImpl<DATATYPE> empty;
				StringImpl<DATATYPE> empty2;

				StringImpl<DATATYPE>::Iterator it = empty.find(empty2, empty.begin(), pMatchEndIt );
				REQUIRE( it==empty.begin() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == empty.begin() );
			}


			SECTION("withMultipleMatches")
			{
				StringImpl<DATATYPE> s2 = s;
				s2+=s;

				SECTION("fromStart")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind, s2.begin(), pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind, s2.begin()+2, pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromJustAfterFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind, s2.begin()+3, pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromJustBeforeSecondMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind, s2.begin()+11, pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromSecondMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind, s2.begin()+12, pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromJustAfterSecondMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind, s2.begin()+13, pMatchEndIt );
					REQUIRE( it==s2.end() );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.end() );
				}
			}
		}
	}
}

template<class DATATYPE>
inline void testFindIterators()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");	
	StringImpl<DATATYPE> toFind(U"\U00012345lo");	
	StringImpl<DATATYPE> toFindNonMatch(U"\U00012345lO");	

	StringImpl<DATATYPE>::Iterator matchEndIt;

	for(int withMatchEndIt=0; withMatchEndIt<2; withMatchEndIt++)
	{
		SECTION( withMatchEndIt==1 ? "withMatchEndIt" : "noMatchEndIt" )
		{
			StringImpl<DATATYPE>::Iterator* pMatchEndIt = nullptr;
				
			if(withMatchEndIt==1)
				pMatchEndIt = &matchEndIt;

			SECTION("fromStart-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind.begin(), toFind.end(), s.begin(), pMatchEndIt);
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}
				
			SECTION("fromStart-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFindNonMatch.begin(), toFindNonMatch.end(), s.begin(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind.begin(), toFind.end(), s.begin()+2, pMatchEndIt );
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}

			SECTION("fromMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFindNonMatch.begin(), toFindNonMatch.end(), s.begin()+2, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end()  );
			}


			SECTION("fromAfterMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind.begin(), toFind.end(), s.begin()+3, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromAfterMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFindNonMatch.begin(), toFindNonMatch.end(), s.begin()+3, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}



			SECTION("fromEnd-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind.begin(), toFind.end(), s.end(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromEnd-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFindNonMatch.begin(), toFindNonMatch.end(), s.end(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}



			SECTION("empty-fromStart")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind.begin(), toFind.begin(), s.begin(), pMatchEndIt );
				REQUIRE( it==s.begin() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}


			SECTION("empty-fromMiddle")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind.begin(), toFind.begin(), s.begin()+5, pMatchEndIt );
				REQUIRE( it==s.begin()+5 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}

			SECTION("empty-fromEnd")
			{
				StringImpl<DATATYPE>::Iterator it = s.find(toFind.begin(), toFind.begin(), s.end(), pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}


			SECTION("notEmpty-inEmpty")
			{
				StringImpl<DATATYPE> empty;

				StringImpl<DATATYPE>::Iterator it = empty.find(toFind.begin(), toFind.end(), empty.begin(), pMatchEndIt );
				REQUIRE( it==empty.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == empty.end() );
			}

			SECTION("empty-inEmpty")
			{
				StringImpl<DATATYPE> empty;

				StringImpl<DATATYPE>::Iterator it = empty.find(toFind.begin(), toFind.begin(), empty.begin(), pMatchEndIt );
				REQUIRE( it==empty.begin() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}


			SECTION("withMultipleMatches")
			{
				StringImpl<DATATYPE> s2 = s;
				s2+=s;

				SECTION("fromStart")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind.begin(), toFind.end(), s2.begin(), pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.begin()+5 );
				}

				SECTION("fromFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind.begin(), toFind.end(), s2.begin()+2, pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.begin()+5 );
				}

				SECTION("fromJustAfterFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind.begin(), toFind.end(), s2.begin()+3, pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.begin()+15 );
				}

				SECTION("fromJustBeforeSecondMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind.begin(), toFind.end(), s2.begin()+11, pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.begin()+15 );
				}

				SECTION("fromSecondMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind.begin(), toFind.end(), s2.begin()+12, pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.begin()+15 );
				}

				SECTION("fromJustAfterSecondMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.find(toFind.begin(), toFind.end(), s2.begin()+13, pMatchEndIt );
					REQUIRE( it==s2.end() );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.end() );
				}
			}
		}
	}
}


template<class DATATYPE>
inline void testFind()
{
	SECTION("iterators")
		testFindIterators<DATATYPE>();
	

	SECTION("stringFromIt")
		testFindStringFromIt<DATATYPE>();

	SECTION("stringFromIndex")
	{
		SECTION("String")
			testFindStringFromIndex<StringImpl<DATATYPE>, StringImpl<DATATYPE> >();


		SECTION("std::string")
			testFindStringFromIndex<StringImpl<DATATYPE>, std::string >();

		SECTION("std::u16string")
			testFindStringFromIndex<StringImpl<DATATYPE>, std::u16string >();

		SECTION("std::u32string")
			testFindStringFromIndex<StringImpl<DATATYPE>, std::u32string >();

		SECTION("std::wstring")
			testFindStringFromIndex<StringImpl<DATATYPE>, std::wstring >();


		SECTION("const char*")
			testFindStringFromIndex<StringImpl<DATATYPE>, const char* >();

		SECTION("const char16_t*")
			testFindStringFromIndex<StringImpl<DATATYPE>, const char16_t* >();

		SECTION("const char32_t*")
			testFindStringFromIndex<StringImpl<DATATYPE>, const char32_t* >();

		SECTION("const wchar_t*")
			testFindStringFromIndex<StringImpl<DATATYPE>, const wchar_t* >();


		SECTION("const char* with length")
			testFindStringWithLengthFromIndex<StringImpl<DATATYPE>, const char* >();

		SECTION("const char16_t* with length")
			testFindStringWithLengthFromIndex<StringImpl<DATATYPE>, const char16_t* >();

		SECTION("const char32_t* with length")
			testFindStringWithLengthFromIndex<StringImpl<DATATYPE>, const char32_t* >();

		SECTION("const wchar_t* with length")
			testFindStringWithLengthFromIndex<StringImpl<DATATYPE>, const wchar_t* >();
	}

	SECTION("charFromIterator")
		testFindCharFromIterator< StringImpl<DATATYPE> >();

	SECTION("charFromIndex")
		testFindCharFromIndex< StringImpl<DATATYPE> >();


	SECTION("inStdU32String")
	{
		// run the same tests we run on our own string on a std::u32string (to ensure that we behave the same way.

		SECTION("stringFromIndex")			
			testFindStringFromIndex<std::u32string, std::u32string >();

		SECTION("charFromIndexInU32String")
			testFindCharFromIndex< std::u32string >();
	}

}





template<class DATATYPE>
inline void testRFindIterators()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");	
	StringImpl<DATATYPE> toFind(U"\U00012345lo");	
	StringImpl<DATATYPE> toFindNonMatch(U"\U00012345lO");	
	StringImpl<DATATYPE> empty;

	StringImpl<DATATYPE>::Iterator matchEndIt;

	for(int withMatchEndIt=0; withMatchEndIt<2; withMatchEndIt++)
	{
		SECTION( withMatchEndIt==1 ? "withMatchEndIt" : "noMatchEndIt" )
		{
			StringImpl<DATATYPE>::Iterator* pMatchEndIt = nullptr;
				
			if(withMatchEndIt==1)
				pMatchEndIt = &matchEndIt;

			SECTION("fromStart-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind.begin(), toFind.end(), s.begin(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}
				
			SECTION("fromStart-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch.begin(), toFindNonMatch.end(), s.begin(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}


			SECTION("fromBeforeMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind.begin(), toFind.end(), s.begin()+1, pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}
				
			SECTION("fromBeforeMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch.begin(), toFindNonMatch.end(), s.begin()+1, pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind.begin(), toFind.end(), s.begin()+2, pMatchEndIt );
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}

			SECTION("fromMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch.begin(), toFindNonMatch.end(), s.begin()+2, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end()  );
			}


			SECTION("fromAfterMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind.begin(), toFind.end(), s.begin()+3, pMatchEndIt );
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}

			SECTION("fromAfterMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch.begin(), toFindNonMatch.end(), s.begin()+3, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}



			SECTION("fromEnd-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind.begin(), toFind.end(), s.end(), pMatchEndIt);
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}

			SECTION("fromEnd-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch.begin(), toFindNonMatch.end(), s.end(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}



			SECTION("empty-fromStart")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(empty.begin(), empty.end(), s.begin(), pMatchEndIt );
				REQUIRE( it==s.begin() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}


			SECTION("empty-fromMiddle")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(empty.begin(), empty.end(), s.begin()+5, pMatchEndIt );
				REQUIRE( it==s.begin()+5 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}

			SECTION("empty-fromEnd")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(empty.begin(), empty.end(), s.end(), pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}


			SECTION("notEmpty-inEmpty")
			{
				StringImpl<DATATYPE>::Iterator it = empty.rfind(toFind.begin(), toFind.end(), empty.end(), pMatchEndIt );
				REQUIRE( it==empty.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == empty.end() );
			}

			SECTION("empty-inEmpty")
			{
				StringImpl<DATATYPE> empty;

				StringImpl<DATATYPE>::Iterator it = empty.rfind(toFind.begin(), toFind.begin(), empty.end(), pMatchEndIt );
				REQUIRE( it==empty.begin() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}


			SECTION("withMultipleMatches")
			{
				StringImpl<DATATYPE> s2 = s;
				s2+=s;

				SECTION("fromEnd")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind.begin(), toFind.end(), s2.end(), pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+3 );
				}

				SECTION("fromLastMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind.begin(), toFind.end(), s2.begin()+12, pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+3 );
				}

				SECTION("fromJustBeforeLastMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind.begin(), toFind.end(), s2.begin()+11, pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+3 );
				}

				SECTION("fromJustAfterFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind.begin(), toFind.end(), s2.begin()+3, pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+3 );
				}

				SECTION("fromFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind.begin(), toFind.end(), s2.begin()+2, pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+3 );
				}

				SECTION("fromJustBeforeFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind.begin(), toFind.end(), s2.begin()+1, pMatchEndIt );
					REQUIRE( it==s2.end() );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.end() );
				}

				SECTION("fromBegin")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind.begin(), toFind.end(), s2.begin(), pMatchEndIt );
					REQUIRE( it==s2.end() );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.end() );
				}
			}
		}
	}
}



template<class DATATYPE>
inline void testRFindStringFromIt()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");	
	StringImpl<DATATYPE> toFind(U"\U00012345lo");	
	StringImpl<DATATYPE> toFindNonMatch(U"\U00012345lO");	
	StringImpl<DATATYPE> empty;

	for(int withMatchEndIt=0; withMatchEndIt<2; withMatchEndIt++)
	{
		SECTION( withMatchEndIt==1 ? "withMatchEndIt" : "noMatchEndIt" )
		{
			StringImpl<DATATYPE>::Iterator* pMatchEndIt = nullptr;

			StringImpl<DATATYPE>::Iterator matchEndIt;
				
			if(withMatchEndIt==1)
				pMatchEndIt = &matchEndIt;

			SECTION("fromEnd-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind, s.end(), pMatchEndIt);
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}

			SECTION("fromEnd-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch, s.end(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind, s.begin()+2, pMatchEndIt );
				REQUIRE( it==s.begin()+2 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it+toFind.getLength() );
			}

			SECTION("fromMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch, s.begin()+2, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}


			SECTION("fromBeforeMatchPos-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind, s.begin()+1, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromBeforeMatchPos-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch, s.begin()+1, pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end()  );
			}



			SECTION("fromBegin-matching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFind, s.begin(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}

			SECTION("fromBegin-notMatching")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(toFindNonMatch, s.begin(), pMatchEndIt);
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.end() );
			}



			SECTION("empty-fromEnd")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(empty, s.end(), pMatchEndIt );
				REQUIRE( it==s.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}


			SECTION("empty-fromMiddle")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(empty, s.begin()+5, pMatchEndIt );
				REQUIRE( it==s.begin()+5 );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == it );
			}

			SECTION("empty-fromBegin")
			{
				StringImpl<DATATYPE>::Iterator it = s.rfind(empty, s.begin(), pMatchEndIt );
				REQUIRE( it==s.begin() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == s.begin() );
			}


			SECTION("notEmpty-inEmpty")
			{
				StringImpl<DATATYPE>::Iterator it = empty.rfind(toFind, empty.end(), pMatchEndIt );
				REQUIRE( it==empty.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == empty.end() );
			}

			SECTION("empty-inEmpty")
			{
				StringImpl<DATATYPE> empty2;

				StringImpl<DATATYPE>::Iterator it = empty.rfind(empty2, empty.end(), pMatchEndIt );
				REQUIRE( it==empty.end() );

				if(pMatchEndIt!=nullptr)
					REQUIRE( *pMatchEndIt == empty.end() );
			}


			SECTION("withMultipleMatches")
			{
				StringImpl<DATATYPE> s2 = s;
				s2+=s;

				SECTION("fromEnd")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind, s2.end(), pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromSecondMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind, s2.begin()+12, pMatchEndIt );
					REQUIRE( it==s2.begin()+12 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromJustBeforeSecondMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind, s2.begin()+11, pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromJustAfterFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind, s2.begin()+3, pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind, s2.begin()+2, pMatchEndIt );
					REQUIRE( it==s2.begin()+2 );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == it+toFind.getLength() );
				}

				SECTION("fromJustBeforeFirstMatch")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind, s2.begin()+1, pMatchEndIt );
					REQUIRE( it==s2.end() );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.end() );
				}

				SECTION("fromBegin")
				{
					StringImpl<DATATYPE>::Iterator it = s2.rfind(toFind, s2.begin(), pMatchEndIt );
					REQUIRE( it==s2.end() );

					if(pMatchEndIt!=nullptr)
						REQUIRE( *pMatchEndIt == s2.end() );
				}
			}
		}
	}
}



template<class STRINGTYPE, class ToFindStringType>
inline void testRFindStringFromIndex()
{
	STRINGTYPE s(U"he\U00012345loworld");	
	STRINGTYPE empty(U"");	

	STRINGTYPE toFindObj(U"\U00012345lo");	
	STRINGTYPE toFindNonMatchObj(U"\U00012345lO");	
	STRINGTYPE toFindEmptyObj(U"");	

	ToFindStringType toFind = (ToFindStringType)toFindObj;
	ToFindStringType toFindNonMatch = (ToFindStringType)toFindNonMatchObj;
	ToFindStringType toFindEmpty = (ToFindStringType)toFindEmptyObj;


	SECTION("fromNpos-matching")
	{
		size_t result = s.rfind(toFind, String::npos);
		REQUIRE( result==2 );
	}

	SECTION("fromNpos-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, String::npos);
		REQUIRE( result==s.npos );
	}

	SECTION("fromNpos-empty")
	{
		size_t result = s.rfind(toFindEmpty, String::npos);
		REQUIRE( result==s.length() );
	}


	SECTION("fromEnd-matching")
	{
		size_t result = s.rfind(toFind, s.length() );
		REQUIRE( result==2 );
	}

	SECTION("fromEnd-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, s.length() );
		REQUIRE( result==s.npos );
	}

	SECTION("fromEnd-empty")
	{
		size_t result = s.rfind(toFindEmpty, s.length() );
		REQUIRE( result==s.length() );
	}


	SECTION("fromAfterEnd-matching")
	{
		size_t result = s.rfind(toFind, s.length()+1 );
		REQUIRE( result==2 );
	}

	SECTION("fromAfterEnd-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, s.length()+1 );
		REQUIRE( result==s.npos );
	}

	SECTION("fromAfterEnd-empty")
	{
		size_t result = s.rfind(toFindEmpty, s.length()+1 );
		REQUIRE( result==s.length() );
	}


	SECTION("fromEndUsingDefaultArg-matching")
	{
		size_t result = s.rfind(toFind);
		REQUIRE( result==2 );
	}

	SECTION("fromEndUsingDefaultArg-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch);
		REQUIRE( result==s.npos );
	}


	SECTION("fromMatchPos-matching")
	{
		size_t result = s.rfind(toFind, 2 );
		REQUIRE( result==2 );
	}

	SECTION("fromMatchPos-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, 2 );
		REQUIRE( result==s.npos );
	}


	SECTION("fromBeforeMatchPos-matching")
	{
		size_t result = s.rfind(toFind, 1 );
		REQUIRE( result == s.npos );
	}

	SECTION("fromBeforeMatchPos-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, 1 );
		REQUIRE( result == s.npos );
	}



	SECTION("fromStart-matching")
	{
		size_t result = s.rfind(toFind, 0 );
		REQUIRE( result == s.npos );
	}

	SECTION("fromStart-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, 0 );
		REQUIRE( result == s.npos );
	}



	SECTION("empty-fromStart")
	{
		size_t result = s.rfind(toFindEmpty, 0 );
		REQUIRE( result==0 );
	}

	SECTION("empty-fromMiddle")
	{
		size_t result = s.rfind(toFindEmpty, 5 );
		REQUIRE( result==5 );
	}

	SECTION("empty-fromEnd")
	{
		size_t result = s.rfind(toFindEmpty, s.length() );
		REQUIRE( result==s.length() );
	}


	SECTION("notEmpty-inEmpty")
	{
		size_t result = empty.rfind(toFind );
		REQUIRE( result==empty.npos );
	}

	SECTION("empty-inEmpty")
	{
		size_t result = empty.rfind(toFindEmpty );
		REQUIRE( result==0 );
	}


	SECTION("withMultipleMatches")
	{
		STRINGTYPE s2 = s;
		s2 += s;

		SECTION("fromEnd")
		{
			size_t result = s2.rfind(toFind );
			REQUIRE( result==12 );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = s2.rfind(toFind, 12 );
			REQUIRE( result==12 );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = s2.rfind(toFind, 11 );
			REQUIRE( result==2 );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = s2.rfind(toFind, 3 );
			REQUIRE( result==2 );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = s2.rfind(toFind, 2 );
			REQUIRE( result==2 );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			size_t result = s2.rfind(toFind, 1 );
			REQUIRE( result==s2.npos );
		}

	}
}


template<class STRINGTYPE, class ToFindStringType>
inline void testRFindStringWithLengthFromIndex()
{
	STRINGTYPE s(U"he\U00012345loworld");	
	STRINGTYPE empty(U"");	

	STRINGTYPE toFindObj(U"\U00012345lo");	
	STRINGTYPE toFindNonMatchObj(U"\U00012345lO");	
	STRINGTYPE toFindEmptyObj(U"");	

	ToFindStringType toFind = (ToFindStringType)toFindObj;
	ToFindStringType toFindNonMatch = (ToFindStringType)toFindNonMatchObj;
	ToFindStringType toFindEmpty = (ToFindStringType)toFindEmptyObj;

	size_t toFindLength = getCStringLength(toFind);
	size_t toFindNonMatchLength = getCStringLength(toFindNonMatch);
	size_t toFindEmptyLength = getCStringLength(toFindEmpty);

	toFindObj += "hexy";
	toFindNonMatchObj += "hexy";
	toFindEmptyObj += "hexy";

	toFind = (ToFindStringType)toFindObj;
	toFindNonMatch = (ToFindStringType)toFindNonMatchObj;
	toFindEmpty = (ToFindStringType)toFindEmptyObj;

	SECTION("fromNpos-matching")
	{
		size_t result = s.rfind(toFind, String::npos, toFindLength);
		REQUIRE( result==2 );
	}

	SECTION("fromNpos-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, String::npos, toFindNonMatchLength);
		REQUIRE( result==s.npos );
	}

	SECTION("fromNpos-empty")
	{
		size_t result = s.rfind(toFindEmpty, String::npos, toFindEmptyLength);
		REQUIRE( result==s.length() );
	}


	SECTION("fromEnd-matching")
	{
		size_t result = s.rfind(toFind, s.length(), toFindLength );
		REQUIRE( result==2 );
	}

	SECTION("fromEnd-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, s.length(), toFindNonMatchLength );
		REQUIRE( result==s.npos );
	}

	SECTION("fromEnd-empty")
	{
		size_t result = s.rfind(toFindEmpty, s.length(), toFindEmptyLength );
		REQUIRE( result==s.length() );
	}


	SECTION("fromAfterEnd-matching")
	{
		size_t result = s.rfind(toFind, s.length()+1, toFindLength );
		REQUIRE( result==2 );
	}

	SECTION("fromAfterEnd-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, s.length()+1, toFindNonMatchLength );
		REQUIRE( result==s.npos );
	}

	SECTION("fromAfterEnd-empty")
	{
		size_t result = s.rfind(toFindEmpty, s.length()+1, toFindEmptyLength );
		REQUIRE( result==s.length() );
	}



	SECTION("fromMatchPos-matching")
	{
		size_t result = s.rfind(toFind, 2, toFindLength );
		REQUIRE( result==2 );
	}

	SECTION("fromMatchPos-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, 2, toFindNonMatchLength );
		REQUIRE( result==s.npos );
	}


	SECTION("fromBeforeMatchPos-matching")
	{
		size_t result = s.rfind(toFind, 1, toFindLength );
		REQUIRE( result == s.npos );
	}

	SECTION("fromBeforeMatchPos-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, 1, toFindNonMatchLength );
		REQUIRE( result == s.npos );
	}



	SECTION("fromStart-matching")
	{
		size_t result = s.rfind(toFind, 0, toFindLength );
		REQUIRE( result == s.npos );
	}

	SECTION("fromStart-notMatching")
	{
		size_t result = s.rfind(toFindNonMatch, 0, toFindNonMatchLength );
		REQUIRE( result == s.npos );
	}



	SECTION("empty-fromStart")
	{
		size_t result = s.rfind(toFindEmpty, 0, toFindEmptyLength );
		REQUIRE( result==0 );
	}

	SECTION("empty-fromMiddle")
	{
		size_t result = s.rfind(toFindEmpty, 5, toFindEmptyLength );
		REQUIRE( result==5 );
	}

	SECTION("empty-fromEnd")
	{
		size_t result = s.rfind(toFindEmpty, s.length(), toFindEmptyLength );
		REQUIRE( result==s.length() );
	}


	SECTION("notEmpty-inEmpty")
	{
		size_t result = empty.rfind(toFind, String::npos, toFindLength );
		REQUIRE( result==empty.npos );
	}

	SECTION("empty-inEmpty")
	{
		size_t result = empty.rfind(toFindEmpty, String::npos, toFindEmptyLength );
		REQUIRE( result==0 );
	}


	SECTION("withMultipleMatches")
	{
		STRINGTYPE s2 = s;
		s2 += s;

		SECTION("fromEnd")
		{
			size_t result = s2.rfind(toFind, String::npos, toFindLength );
			REQUIRE( result==12 );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = s2.rfind(toFind, 12, toFindLength );
			REQUIRE( result==12 );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = s2.rfind(toFind, 11, toFindLength );
			REQUIRE( result==2 );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = s2.rfind(toFind, 3, toFindLength );
			REQUIRE( result==2 );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = s2.rfind(toFind, 2, toFindLength );
			REQUIRE( result==2 );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			size_t result = s2.rfind(toFind, 1, toFindLength );
			REQUIRE( result==s2.npos );
		}

	}
}



template<class STRINGTYPE>
inline void testRFindCharFromIterator()
{
	STRINGTYPE s(U"he\U00012345loworld");	
	STRINGTYPE empty(U"");	

	char32_t toFind = U'\U00012345';
	char32_t toFindNonMatch = 'x';

	SECTION("fromEnd-matching")
	{
		STRINGTYPE::Iterator it = s.rfind(toFind, s.end());
		REQUIRE( it==s.begin()+2 );
	}

	SECTION("fromEnd-notMatching")
	{
		STRINGTYPE::Iterator it = s.rfind(toFindNonMatch, s.end());
		REQUIRE( it==s.end() );
	}

	
	SECTION("fromMatchPos-matching")
	{
		STRINGTYPE::Iterator it = s.rfind(toFind, s.begin()+2 );
		REQUIRE( it==s.begin()+2 );
	}

	SECTION("fromMatchPos-notMatching")
	{
		STRINGTYPE::Iterator it = s.rfind(toFindNonMatch, s.begin()+2 );
		REQUIRE( it==s.end() );
	}


	SECTION("fromBeforeMatchPos-matching")
	{
		STRINGTYPE::Iterator it = s.rfind(toFind, s.begin()+1 );
		REQUIRE( it == s.end() );
	}

	SECTION("fromBeforeMatchPos-notMatching")
	{
		STRINGTYPE::Iterator it = s.rfind(toFindNonMatch, s.begin()+1 );
		REQUIRE( it == s.end() );
	}



	SECTION("fromBegin-matching")
	{
		STRINGTYPE::Iterator it = s.rfind(toFind, s.begin() );
		REQUIRE( it == s.end() );
	}

	SECTION("fromBegin-notMatching")
	{
		STRINGTYPE::Iterator it = s.rfind(toFindNonMatch, s.begin() );
		REQUIRE( it == s.end() );
	}

	
	SECTION("inEmpty")
	{
		STRINGTYPE::Iterator it = empty.rfind(toFind, empty.end() );
		REQUIRE( it==empty.end() );
	}



	SECTION("withMultipleMatches")
	{
		STRINGTYPE s2 = s;
		s2 += s;

		SECTION("fromEnd")
		{
			STRINGTYPE::Iterator it = s2.rfind(toFind, s2.end() );
			REQUIRE( it==s2.begin()+12 );
		}

		SECTION("fromSecondMatch")
		{
			STRINGTYPE::Iterator it = s2.rfind(toFind, s2.begin()+12 );
			REQUIRE( it==s2.begin()+12 );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			STRINGTYPE::Iterator it = s2.rfind(toFind, s2.begin()+11 );
			REQUIRE( it==s2.begin()+2 );
		}

		SECTION("fromJusAfterFirstMatch")
		{
			STRINGTYPE::Iterator it = s2.rfind(toFind, s2.begin()+3 );
			REQUIRE( it==s2.begin()+2 );
		}
		
		SECTION("fromFirstMatch")
		{
			STRINGTYPE::Iterator it = s2.rfind(toFind, s2.begin()+2 );
			REQUIRE( it==s2.begin()+2 );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			STRINGTYPE::Iterator it = s2.rfind(toFind, s2.begin()+1 );
			REQUIRE( it==s2.end() );
		}
	}

}


template<class StringType, class ...Args>
size_t callRFindChar(StringType& s, Args... args)
{
	size_t result = s.rfind(args...);
	size_t result2 = s.find_last_of(args...);
	
	REQUIRE( result2==result );

	return result;
}

template<class STRINGTYPE>
inline void testRFindCharFromIndex()
{
	STRINGTYPE s(U"he\U00012345loworld");	
	STRINGTYPE empty(U"");	

	char32_t toFind = U'\U00012345';
	char32_t toFindNonMatch = 'x';

	SECTION("fromEnd-matching")
	{
		size_t result = callRFindChar(s, toFind, s.length());
		REQUIRE( result==2 );
	}

	SECTION("fromEnd-notMatching")
	{
		size_t result = callRFindChar(s, toFindNonMatch, s.length());
		REQUIRE( result==s.npos );
	}


	SECTION("fromNpos-matching")
	{
		size_t result = callRFindChar(s, toFind, s.npos );
		REQUIRE( result==2 );
	}

	SECTION("fromNpos-notMatching")
	{
		size_t result = callRFindChar(s, toFindNonMatch, s.npos );
		REQUIRE( result==s.npos );
	}


	SECTION("fromEndUsingDefaultArg-matching")
	{
		size_t result = callRFindChar(s, toFind);
		REQUIRE( result==2 );
	}

	SECTION("fromEndUsingDefaultArg-notMatching")
	{
		size_t result = callRFindChar(s, toFindNonMatch);
		REQUIRE( result==s.npos );
	}



	SECTION("fromAfterEnd-matching")
	{
		size_t result = callRFindChar(s, toFind, s.length()+1);
		REQUIRE( result==2 );
	}

	SECTION("fromAfterEnd-notMatching")
	{
		size_t result = callRFindChar(s, toFindNonMatch, s.length()+1);
		REQUIRE( result==s.npos );
	}


	SECTION("fromMatchPos-matching")
	{
		size_t result = callRFindChar(s, toFind, 2 );
		REQUIRE( result==2 );
	}

	SECTION("fromMatchPos-notMatching")
	{
		size_t result = callRFindChar(s, toFindNonMatch, 2 );
		REQUIRE( result==s.npos );
	}


	SECTION("fromBeforeMatchPos-matching")
	{
		size_t result = callRFindChar(s, toFind, 1 );
		REQUIRE( result == s.npos );
	}

	SECTION("fromBeforeMatchPos-notMatching")
	{
		size_t result = callRFindChar(s, toFindNonMatch, 1 );
		REQUIRE( result == s.npos );
	}



	SECTION("fromBegin-matching")
	{
		size_t result = callRFindChar(s, toFind, 0 );
		REQUIRE( result == s.npos );
	}

	SECTION("fromBegin-notMatching")
	{
		size_t result = callRFindChar(s, toFindNonMatch, 0 );
		REQUIRE( result == s.npos );
	}


		
	SECTION("inEmpty")
	{
		size_t result = callRFindChar(empty, toFind, 0 );
		REQUIRE( result==empty.npos );
	}



	SECTION("withMultipleMatches")
	{
		STRINGTYPE s2 = s;
		s2 += s;

		SECTION("fromNpos")
		{
			size_t result = callRFindChar(s2, toFind, s2.npos );
			REQUIRE( result==12 );
		}

		SECTION("fromEnd")
		{
			size_t result = callRFindChar(s2, toFind, s2.length() );
			REQUIRE( result==12 );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = callRFindChar(s2, toFind, 12 );
			REQUIRE( result==12 );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callRFindChar(s2, toFind, 11 );
			REQUIRE( result==2 );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callRFindChar(s2, toFind, 3 );
			REQUIRE( result==2 );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = callRFindChar(s2, toFind, 2 );
			REQUIRE( result==2 );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			size_t result = callRFindChar(s2, toFind, 1 );
			REQUIRE( result==s2.npos );
		}
	}

}



template<class DATATYPE>
inline void testRFind()
{
	SECTION("iterators")
		testRFindIterators<DATATYPE>();
	
	SECTION("stringFromIt")
		testRFindStringFromIt<DATATYPE>();

	
	SECTION("stringFromIndex")
	{
		SECTION("String")
			testRFindStringFromIndex<StringImpl<DATATYPE>, StringImpl<DATATYPE> >();
		
		SECTION("std::string")
			testRFindStringFromIndex<StringImpl<DATATYPE>, std::string >();

		SECTION("std::u16string")
			testRFindStringFromIndex<StringImpl<DATATYPE>, std::u16string >();

		SECTION("std::u32string")
			testRFindStringFromIndex<StringImpl<DATATYPE>, std::u32string >();

		SECTION("std::wstring")
			testRFindStringFromIndex<StringImpl<DATATYPE>, std::wstring >();


		SECTION("const char*")
			testRFindStringFromIndex<StringImpl<DATATYPE>, const char* >();

		SECTION("const char16_t*")
			testRFindStringFromIndex<StringImpl<DATATYPE>, const char16_t* >();

		SECTION("const char32_t*")
			testRFindStringFromIndex<StringImpl<DATATYPE>, const char32_t* >();

		SECTION("const wchar_t*")
			testRFindStringFromIndex<StringImpl<DATATYPE>, const wchar_t* >();


		SECTION("const char* with length")
			testRFindStringWithLengthFromIndex<StringImpl<DATATYPE>, const char* >();

		SECTION("const char16_t* with length")
			testRFindStringWithLengthFromIndex<StringImpl<DATATYPE>, const char16_t* >();

		SECTION("const char32_t* with length")
			testRFindStringWithLengthFromIndex<StringImpl<DATATYPE>, const char32_t* >();

		SECTION("const wchar_t* with length")
			testRFindStringWithLengthFromIndex<StringImpl<DATATYPE>, const wchar_t* >();
	}

	SECTION("charFromIterator")
		testRFindCharFromIterator< StringImpl<DATATYPE> >();

	SECTION("charFromIndex")
		testRFindCharFromIndex< StringImpl<DATATYPE> >();


	SECTION("inStdU32String")
	{
		// run the same tests we run on our own string on a std::u32string (to ensure that we behave the same way.

		SECTION("stringFromIndex")			
			testRFindStringFromIndex<std::u32string, std::u32string >();

		SECTION("charFromIndexInU32String")
			testRFindCharFromIndex< std::u32string >();
	}
}


template<class DATATYPE, class Predicate>
inline void testFindConditionWithPred(Predicate pred, bool shouldMatch)
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	SECTION("fromStart")
	{	
		StringImpl<DATATYPE>::Iterator it = s.findCondition(pred, s.begin());

		if(shouldMatch)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );
	}


	SECTION("fromMatch")
	{	
		StringImpl<DATATYPE>::Iterator it = s.findCondition(pred, s.begin()+2);

		if(shouldMatch)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );
	}

	SECTION("fromAfterMatch")
	{	
		StringImpl<DATATYPE>::Iterator it = s.findCondition(pred, s.begin()+3);
		REQUIRE( it==s.end() );
	}

	SECTION("fromEnd")
	{	
		StringImpl<DATATYPE>::Iterator it = s.findCondition(pred, s.end() );
		REQUIRE( it==s.end() );
	}

	SECTION("inEmpty")
	{	
		StringImpl<DATATYPE> empty;

		StringImpl<DATATYPE>::Iterator it = empty.findCondition(pred, empty.begin() );
		REQUIRE( it==empty.end() );
	}
}

template<class DATATYPE, class Predicate>
inline void testFindConditionIndexWithPred(Predicate pred, bool shouldMatch)
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	SECTION("fromStart")
	{	
		size_t result = s.findCondition(pred, 0);

		if(shouldMatch)
			REQUIRE( result==2 );
		else
			REQUIRE( result==s.noMatch );
	}

	SECTION("fromStart-defaultArg")
	{	
		size_t result = s.findCondition(pred);

		if(shouldMatch)
			REQUIRE( result==2 );
		else
			REQUIRE( result==s.noMatch );
	}


	SECTION("fromMatch")
	{	
		size_t result = s.findCondition(pred, 2);

		if(shouldMatch)
			REQUIRE( result==2 );
		else
			REQUIRE( result==s.noMatch );
	}

	SECTION("fromAfterMatch")
	{	
		size_t result = s.findCondition(pred, 3);
		REQUIRE( result==s.noMatch );
	}

	SECTION("fromEnd")
	{	
		size_t result = s.findCondition(pred, s.getLength() );
		REQUIRE( result==s.noMatch );
	}

	SECTION("fromEnd-npos")
	{	
		size_t result = s.findCondition(pred, s.npos );
		REQUIRE( result==s.noMatch );
	}

	SECTION("fromAfter")
	{	
		size_t result = s.findCondition(pred, s.getLength()+1 );
		REQUIRE( result==s.noMatch );
	}

	SECTION("inEmpty")
	{	
		StringImpl<DATATYPE> empty;

		size_t result = empty.findCondition(pred, 0 );
		REQUIRE( result==s.noMatch );
	}
}

template<class DATATYPE>
inline void testFindCondition()
{	
	SECTION("iterator")
	{
		SECTION("match")
			testFindConditionWithPred<DATATYPE>( [](auto it){ return *it == U'\U00012345'; }, true );

		SECTION("noMatch")
			testFindConditionWithPred<DATATYPE>( [](auto it){ return false; }, false );
	}

	SECTION("index")
	{
		SECTION("match")
			testFindConditionIndexWithPred<DATATYPE>( [](auto it){ return *it == U'\U00012345'; }, true );

		SECTION("noMatch")
			testFindConditionIndexWithPred<DATATYPE>( [](auto it){ return false; }, false );		
	}
}




template<class DATATYPE, class Predicate>
inline void testReverseFindConditionWithPred(Predicate pred, bool shouldMatch)
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	SECTION("fromEnd")
	{	
		StringImpl<DATATYPE>::Iterator it = s.reverseFindCondition(pred, s.end());

		if(shouldMatch)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );
	}


	SECTION("fromMatch")
	{	
		StringImpl<DATATYPE>::Iterator it = s.reverseFindCondition(pred, s.begin()+2);

		if(shouldMatch)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );
	}

	SECTION("fromBeforeMatch")
	{	
		StringImpl<DATATYPE>::Iterator it = s.reverseFindCondition(pred, s.begin()+1);
		REQUIRE( it==s.end() );
	}

	SECTION("fromEnd")
	{	
		StringImpl<DATATYPE>::Iterator it = s.reverseFindCondition(pred, s.end() );
		
		if(shouldMatch)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );
	}

	SECTION("inEmpty")
	{	
		StringImpl<DATATYPE> empty;

		StringImpl<DATATYPE>::Iterator it = empty.reverseFindCondition(pred, empty.begin() );
		REQUIRE( it==empty.end() );
	}
}





template<class DATATYPE, class Predicate>
inline void testReverseFindConditionIndexWithPred(Predicate pred, bool shouldMatch)
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	SECTION("fromEnd")
	{	
		size_t result = s.reverseFindCondition(pred, s.getLength() );

		if(shouldMatch)
			REQUIRE( result==2 );
		else
			REQUIRE( result==s.npos );
	}

	SECTION("fromEnd-npos")
	{	
		size_t result = s.reverseFindCondition(pred, s.npos );

		if(shouldMatch)
			REQUIRE( result==2 );
		else
			REQUIRE( result==s.npos );
	}

	SECTION("fromAfterEnd")
	{	
		size_t result = s.reverseFindCondition(pred, s.getLength()+1 );

		if(shouldMatch)
			REQUIRE( result==2 );
		else
			REQUIRE( result==s.npos );
	}

	SECTION("fromEnd-defaultArg")
	{	
		size_t result = s.reverseFindCondition(pred);

		if(shouldMatch)
			REQUIRE( result==2 );
		else
			REQUIRE( result==s.npos );
	}

	SECTION("fromMatch")
	{	
		size_t result = s.reverseFindCondition(pred, 2);

		if(shouldMatch)
			REQUIRE( result==2 );
		else
			REQUIRE( result==s.npos );
	}

	SECTION("fromBeforeMatch")
	{	
		size_t result = s.reverseFindCondition(pred, 1);
		REQUIRE( result==s.npos );
	}


	SECTION("inEmpty")
	{	
		StringImpl<DATATYPE> empty;

		size_t result = empty.reverseFindCondition(pred, 0 );
		REQUIRE( result==empty.npos );
	}
}

template<class DATATYPE>
inline void testReverseFindCondition()
{	
	SECTION("iterator")
	{
		SECTION("match")
			testReverseFindConditionWithPred<DATATYPE>( [](auto it){ return *it == U'\U00012345'; }, true );
	
		SECTION("noMatch")
			testReverseFindConditionWithPred<DATATYPE>( [](auto it){ return false; }, false );
	}

	SECTION("index")
	{
		SECTION("match")
			testReverseFindConditionIndexWithPred<DATATYPE>( [](auto it){ return *it == U'\U00012345'; }, true );
	
		SECTION("noMatch")
			testReverseFindConditionIndexWithPred<DATATYPE>( [](auto it){ return false; }, false );
	}

}





template<class DATATYPE>
inline void testFindOneOfIterators(const std::u32string& toFind, bool matchPossible)
{	
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	SECTION("fromStart")
	{
		StringImpl<DATATYPE>::Iterator it = s.findOneOf(toFind.begin(), toFind.end(), s.begin() );
				
		if(matchPossible)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );				
	}

	SECTION("fromMatch")
	{
		StringImpl<DATATYPE>::Iterator it = s.findOneOf(toFind.begin(), toFind.end(), s.begin()+2 );

		if(matchPossible)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );
	}
			
	SECTION("fromAfterMatch")
	{
		StringImpl<DATATYPE>::Iterator it = s.findOneOf(toFind.begin(), toFind.end(), s.begin()+3 );
				
		REQUIRE( it==s.end() );
	}

	SECTION("fromEnd")
	{
		StringImpl<DATATYPE>::Iterator it = s.findOneOf(toFind.begin(), toFind.end(), s.end() );
				
		REQUIRE( it==s.end() );
	}


	SECTION("withMultipleMatches")
	{
		StringImpl<DATATYPE> s2 = s;
		s2+=s;

		SECTION("fromStart")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findOneOf(toFind.begin(), toFind.end(), s2.begin() );

			if(matchPossible)
				REQUIRE( it==s2.begin()+2 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findOneOf(toFind.begin(), toFind.end(), s2.begin()+2 );

			if(matchPossible)
				REQUIRE( it==s2.begin()+2 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findOneOf(toFind.begin(), toFind.end(), s2.begin()+3 );
					
			if(matchPossible)
				REQUIRE( it==s2.begin()+12 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findOneOf(toFind.begin(), toFind.end(), s2.begin()+11 );
					
			if(matchPossible)
				REQUIRE( it==s2.begin()+12 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findOneOf(toFind.begin(), toFind.end(), s2.begin()+12 );
					
			if(matchPossible)
				REQUIRE( it==s2.begin()+12 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findOneOf(toFind.begin(), toFind.end(), s2.begin()+13 );

			REQUIRE( it==s2.end() );
		}
	}
}


template<class StringType, class ...Args>
size_t callFindOneOf(StringType& s, Args... args)
{
	size_t result = s.findOneOf(args...);
	size_t result2 = s.find_first_of(args...);

	REQUIRE( result2==result );

	return result;
}

template<class StringType, class ToFindType>
inline void testFindOneOfString(const StringType& toFindObj, bool matchPossible)
{
	StringType s(U"he\U00012345loworld");

	ToFindType toFind = (ToFindType)toFindObj;

	SECTION("fromStart")
	{
		size_t result = callFindOneOf(s, toFind, 0 );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromStart-defaultArg")
	{
		size_t result = callFindOneOf(s, toFind );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromMatch")
	{
		size_t result = callFindOneOf(s, toFind, 2 );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
			
	SECTION("fromAfterMatch")
	{
		size_t result = callFindOneOf(s, toFind, 3 );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromEnd")
	{
		size_t result = callFindOneOf(s, toFind, s.getLength() );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromAfterEnd")
	{
		size_t result = callFindOneOf(s, toFind, s.getLength()+1 );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromNpos")
	{
		size_t result = callFindOneOf(s, toFind, s.npos );
				
		REQUIRE( result==StringType::noMatch );
	}


	SECTION("withMultipleMatches")
	{
		StringType s2 = s;
		s2+=s;

		SECTION("fromStart")
		{
			size_t result = callFindOneOf(s2, toFind, 0 );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromStart-defaultArg")
		{
			size_t result = callFindOneOf(s2, toFind );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 2 );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 3 );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 11 );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );			
		}

		SECTION("fromSecondMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 12 );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 13 );

			REQUIRE( result==s2.noMatch );
		}
	}
}



template<class StringType, class ToFindType>
inline void testFindOneOfStringWithLength(const StringType& toFindObjArg, bool matchPossible)
{
	StringType s(U"he\U00012345loworld");

	StringType toFindObj = toFindObjArg;

	ToFindType toFind = (ToFindType)toFindObj;

	size_t toFindLength = getCStringLength<ToFindType>(toFind);

	toFindObj += U"hexy";

	toFind = (ToFindType)toFindObj;


	SECTION("fromStart")
	{
		size_t result = callFindOneOf(s, toFind, 0, toFindLength );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
	
	SECTION("fromMatch")
	{
		size_t result = callFindOneOf(s, toFind, 2, toFindLength );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
			
	SECTION("fromAfterMatch")
	{
		size_t result = callFindOneOf(s, toFind, 3, toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromEnd")
	{
		size_t result = callFindOneOf(s, toFind, s.getLength(), toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromAfterEnd")
	{
		size_t result = callFindOneOf(s, toFind, s.getLength()+1, toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromNpos")
	{
		size_t result = callFindOneOf(s, toFind, s.npos, toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}


	SECTION("withMultipleMatches")
	{
		StringType s2 = s;
		s2+=s;

		SECTION("fromStart")
		{
			size_t result = callFindOneOf(s2, toFind, 0, toFindLength );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}


		SECTION("fromFirstMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 2, toFindLength );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 3, toFindLength );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 11, toFindLength );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );			
		}

		SECTION("fromSecondMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 12, toFindLength );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			size_t result = callFindOneOf(s2, toFind, 13, toFindLength );

			REQUIRE( result==s2.noMatch );
		}
	}
}



template<class DATATYPE>
inline void testFindOneOf()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	struct MatchData
	{
		const char*			desc;
		const char32_t*		matchString;
		bool				matchPossible;
	};

	std::list<MatchData> matchList = {	{"firstCharMatches",	U"\U00012345xy", true },
										{"second char matches", U"x\U00012345y", true },
										{"last char matches", U"xy\U00012345", true },
										{"single char matches", U"\U00012345", true },
										{"no char matches", U"xyz", false},
										{"empty", U"", false},
										{"single char no match", U"x", false} };

	for( auto matchIt = matchList.begin(); matchIt!=matchList.end(); matchIt++)
	{
		const MatchData& matchData = *matchIt;

		SECTION( matchData.desc )
		{
			SECTION("iterators")
				testFindOneOfIterators<DATATYPE>(matchData.matchString, matchData.matchPossible);

			SECTION("String")
				testFindOneOfString<StringImpl<DATATYPE>, StringImpl<DATATYPE> >( matchData.matchString, matchData.matchPossible);

			SECTION("std::string")
				testFindOneOfString<StringImpl<DATATYPE>, std::string >( matchData.matchString, matchData.matchPossible);

			SECTION("std::wstring")
				testFindOneOfString<StringImpl<DATATYPE>, std::wstring >( matchData.matchString, matchData.matchPossible);

			SECTION("std::u16string")
				testFindOneOfString<StringImpl<DATATYPE>, std::u16string >( matchData.matchString, matchData.matchPossible);

			SECTION("std::u32string")
				testFindOneOfString<StringImpl<DATATYPE>, std::u32string >( matchData.matchString, matchData.matchPossible);

			
			SECTION("const char*")
				testFindOneOfString<StringImpl<DATATYPE>, const char*>(matchData.matchString, matchData.matchPossible);

			SECTION("const char16_t*")
				testFindOneOfString<StringImpl<DATATYPE>, const char16_t*>(matchData.matchString, matchData.matchPossible);

			SECTION("const char32_t*")
				testFindOneOfString<StringImpl<DATATYPE>, const char32_t*>(matchData.matchString, matchData.matchPossible);

			SECTION("const wchar_t*")
				testFindOneOfString<StringImpl<DATATYPE>, const wchar_t*>(matchData.matchString, matchData.matchPossible);


			SECTION("const char* with length")
				testFindOneOfStringWithLength<StringImpl<DATATYPE>, const char*>(matchData.matchString, matchData.matchPossible);

			SECTION("const char16_t* with length")
				testFindOneOfStringWithLength<StringImpl<DATATYPE>, const char16_t*>(matchData.matchString, matchData.matchPossible);

			SECTION("const char32_t* with length")
				testFindOneOfStringWithLength<StringImpl<DATATYPE>, const char32_t*>(matchData.matchString, matchData.matchPossible);

			SECTION("const wchar_t* with length")
				testFindOneOfStringWithLength<StringImpl<DATATYPE>, const wchar_t*>(matchData.matchString, matchData.matchPossible);


		}
	}	
}





template<class DATATYPE>
inline void testFindNotOneOfIterators(const std::u32string& toFind, bool matchThirdChar, bool matchAllChars)
{	
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	SECTION("fromStart")
	{
		StringImpl<DATATYPE>::Iterator it = s.findNotOneOf(toFind.begin(), toFind.end(), s.begin() );
				
		if(matchAllChars)
			REQUIRE( it==s.begin() );
		
		else if(matchThirdChar)
			REQUIRE( it==s.begin()+2 );

		else
			REQUIRE( it==s.end() );				
	}

	SECTION("fromMatch")
	{
		StringImpl<DATATYPE>::Iterator it = s.findNotOneOf(toFind.begin(), toFind.end(), s.begin()+2 );

		if(matchThirdChar || matchAllChars)
			REQUIRE( it==s.begin()+2 );

		else
			REQUIRE( it==s.end() );	
	}
			
	SECTION("fromAfterMatch")
	{
		StringImpl<DATATYPE>::Iterator it = s.findNotOneOf(toFind.begin(), toFind.end(), s.begin()+3 );

		if(matchAllChars)
			REQUIRE( it==s.begin()+3 );
		
		else
			REQUIRE( it==s.end() );			
	}

	SECTION("fromEnd")
	{
		StringImpl<DATATYPE>::Iterator it = s.findNotOneOf(toFind.begin(), toFind.end(), s.end() );
				
		REQUIRE( it==s.end() );
	}


	SECTION("withMultipleMatches")
	{
		StringImpl<DATATYPE> s2 = s;
		s2+=s;

		SECTION("fromStart")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findNotOneOf(toFind.begin(), toFind.end(), s2.begin() );

			if(matchAllChars)
				REQUIRE( it==s2.begin() );
		
			else if(matchThirdChar)
				REQUIRE( it==s2.begin()+2 );

			else
				REQUIRE( it==s2.end() );	
		}

		SECTION("fromFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findNotOneOf(toFind.begin(), toFind.end(), s2.begin()+2 );

			if(matchThirdChar || matchAllChars)
				REQUIRE( it==s2.begin()+2 );

			else
				REQUIRE( it==s2.end() );	
		}

		SECTION("fromJustAfterFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findNotOneOf(toFind.begin(), toFind.end(), s2.begin()+3 );
					
			if(matchAllChars)
				REQUIRE( it==s2.begin()+3 );
		
			else if(matchThirdChar)
				REQUIRE( it==s2.begin()+12 );

			else
				REQUIRE( it==s2.end() );	
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findNotOneOf(toFind.begin(), toFind.end(), s2.begin()+11 );
					
			if(matchAllChars)
				REQUIRE( it==s2.begin()+11 );
		
			else if(matchThirdChar)
				REQUIRE( it==s2.begin()+12 );

			else
				REQUIRE( it==s2.end() );	
		}

		SECTION("fromSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findNotOneOf(toFind.begin(), toFind.end(), s2.begin()+12 );
					
			if(matchThirdChar || matchAllChars)
				REQUIRE( it==s2.begin()+12 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.findNotOneOf(toFind.begin(), toFind.end(), s2.begin()+13 );

			if(matchAllChars)
				REQUIRE( it==s2.begin()+13 );
		
			else
				REQUIRE( it==s2.end() );	
		}
	}
}



template<class StringType, class ...Args>
size_t callFindNotOneOf(StringType& s, Args... args)
{
	size_t result = s.findNotOneOf(args...);
	size_t result2 = s.find_first_not_of(args...);

	REQUIRE( result2==result );

	return result;
}

template<class StringType, class ToFindType>
inline void testFindNotOneOfString(const StringType& toFindObj, bool matchThirdChar, bool matchAllChars)
{
	StringType s(U"he\U00012345loworld");

	ToFindType toFind = (ToFindType)toFindObj;

	SECTION("fromStart")
	{
		size_t result = callFindNotOneOf(s, toFind, 0 );

		if(matchAllChars)
			REQUIRE( result==0 );

		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );					
	}

	SECTION("fromStart-defaultArg")
	{
		size_t result = callFindNotOneOf(s, toFind );

		if(matchAllChars)
			REQUIRE( result==0 );

		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );	
	}

	SECTION("fromMatch")
	{
		size_t result = callFindNotOneOf(s, toFind, 2 );

		if(matchThirdChar || matchAllChars)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );			
	}
			
	SECTION("fromAfterMatch")
	{
		size_t result = callFindNotOneOf(s, toFind, 3 );

		if(matchAllChars)
			REQUIRE( result==3 );

		else
			REQUIRE( result==StringType::noMatch );	
	}

	SECTION("fromEnd")
	{
		size_t result = callFindNotOneOf(s, toFind, s.getLength() );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromAfterEnd")
	{
		size_t result = callFindNotOneOf(s, toFind, s.getLength()+1 );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromNpos")
	{
		size_t result = callFindNotOneOf(s, toFind, s.npos );
				
		REQUIRE( result==StringType::noMatch );
	}


	SECTION("withMultipleMatches")
	{
		StringType s2 = s;
		s2+=s;

		SECTION("fromStart")
		{
			size_t result = callFindNotOneOf(s2, toFind, 0 );

			if(matchAllChars)
				REQUIRE( result==0 );

			else if(matchThirdChar)
				REQUIRE( result==2 );

			else
				REQUIRE( result==StringType::noMatch );	
		}

		SECTION("fromStart-defaultArg")
		{
			size_t result = callFindNotOneOf(s2, toFind );

			if(matchAllChars)
				REQUIRE( result==0 );

			else if(matchThirdChar)
				REQUIRE( result==2 );

			else
				REQUIRE( result==StringType::noMatch );	
		}

		SECTION("fromFirstMatch")
		{
			size_t result = s2.findNotOneOf(toFind, 2 );

			if(matchThirdChar || matchAllChars)
				REQUIRE( result==2 );

			else
				REQUIRE( result==StringType::noMatch );	
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 3 );

			if(matchAllChars)
				REQUIRE( result==3 );

			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==StringType::noMatch );	
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 11 );

			if(matchAllChars)
				REQUIRE( result==11 );

			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==StringType::noMatch );	
		}

		SECTION("fromSecondMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 12 );

			if(matchThirdChar || matchAllChars)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterSecondMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 13 );

			if(matchAllChars)
				REQUIRE( result==13 );
			
			else
				REQUIRE( result==StringType::noMatch );	
		}
	}
}


template<class StringType, class ToFindType>
inline void testFindNotOneOfStringWithLength(const StringType& toFindObjArg, bool matchThirdChar, bool matchAllChars)
{
	StringType s(U"he\U00012345loworld");

	StringType toFindObj = toFindObjArg;

	ToFindType toFind = (ToFindType)toFindObj;

	size_t toFindLength = getCStringLength<ToFindType>(toFind);

	toFindObj += U"abc";

	toFind = (ToFindType)toFindObj;


	SECTION("fromStart")
	{
		size_t result = callFindNotOneOf(s, toFind, 0, toFindLength );

		if(matchAllChars)
			REQUIRE( result==0 );

		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );				
	}
	
	SECTION("fromMatch")
	{
		size_t result = callFindNotOneOf(s, toFind, 2, toFindLength );

		if(matchAllChars || matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );					
	}
			
	SECTION("fromAfterMatch")
	{
		size_t result = callFindNotOneOf(s, toFind, 3, toFindLength );

		if(matchAllChars)
			REQUIRE( result==3 );

		else
			REQUIRE( result==StringType::noMatch );	
	}

	SECTION("fromEnd")
	{
		size_t result = callFindNotOneOf(s, toFind, s.getLength(), toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromAfterEnd")
	{
		size_t result = callFindNotOneOf(s, toFind, s.getLength()+1, toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromNpos")
	{
		size_t result = callFindNotOneOf(s, toFind, s.npos, toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}


	SECTION("withMultipleMatches")
	{
		StringType s2 = s;
		s2+=s;

		SECTION("fromStart")
		{
			size_t result = callFindNotOneOf(s2, toFind, 0, toFindLength );

			if(matchAllChars)
				REQUIRE( result==0 );

			else if(matchThirdChar)
				REQUIRE( result==2 );

			else
				REQUIRE( result==StringType::noMatch );	
		}


		SECTION("fromFirstMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 2, toFindLength );

			if(matchThirdChar || matchAllChars )
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 3, toFindLength );

			if(matchAllChars)
				REQUIRE( result==3 );

			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==StringType::noMatch );	
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 11, toFindLength );

			if(matchAllChars)
				REQUIRE( result==11 );

			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==StringType::noMatch );	
		}

		SECTION("fromSecondMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 12, toFindLength );

			if(matchThirdChar || matchAllChars)
				REQUIRE( result==12 );

			else
				REQUIRE( result==StringType::noMatch );	
		}

		SECTION("fromJustAfterSecondMatch")
		{
			size_t result = callFindNotOneOf(s2, toFind, 13, toFindLength );

			if(matchAllChars)
				REQUIRE( result==13 );

			else
				REQUIRE( result==StringType::noMatch );	
		}
	}
}




template<class StringType>
inline void testFindFirstNotOfChar()
{
	StringType s(U"he\U00012345loworld");
	StringType empty(U"");

	size_t result = s.find_first_not_of(U'h');
	REQUIRE( result==1 );

	result = s.find_first_not_of(U'e');
	REQUIRE( result==0 );

	result = s.find_first_not_of(U'e', 1);
	REQUIRE( result==2 );

	result = s.find_first_not_of(U'e', 3);
	REQUIRE( result==3 );

	result = s.find_first_not_of(U'e', 10);
	REQUIRE( result==s.noMatch );

	result = s.find_first_not_of(U'e', s.npos);
	REQUIRE( result==s.noMatch );

	result = empty.find_first_not_of(U'e');
	REQUIRE( result==s.noMatch );

	result = empty.find_first_not_of(U'e', 1);
	REQUIRE( result==s.noMatch );

	result = empty.find_first_not_of(U'e', 10);
	REQUIRE( result==s.noMatch );

	result = empty.find_first_not_of(U'e', s.npos);
	REQUIRE( result==s.noMatch );
}

template<class DATATYPE>
inline void testFindNotOneOf()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	struct MatchData
	{
		const char*			desc;
		const char32_t*		matchString;
		bool				matchThirdChar;
		bool				matchAllChars;
	};

	std::list<MatchData> matchList = {	{"match",	U"\U00033333helowrd", true, false },
										{"noMatch",	U"\U00012345helowrd", false, false },
										{"matchAny-empty",	U"", true, true },
										{"matchAny-otherChars",	U"abc", true, true }
									};

	for( auto matchIt = matchList.begin(); matchIt!=matchList.end(); matchIt++)
	{
		const MatchData& matchData = *matchIt;

		SECTION( matchData.desc )
		{
			SECTION("iterators")
				testFindNotOneOfIterators<DATATYPE>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars );

			SECTION("String")
				testFindNotOneOfString<StringImpl<DATATYPE>, StringImpl<DATATYPE> >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars );

			SECTION("std::string")
				testFindNotOneOfString<StringImpl<DATATYPE>, std::string >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("std::wstring")
				testFindNotOneOfString<StringImpl<DATATYPE>, std::wstring >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("std::u16string")
				testFindNotOneOfString<StringImpl<DATATYPE>, std::u16string >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("std::u32string")
				testFindNotOneOfString<StringImpl<DATATYPE>, std::u32string >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			
			SECTION("const char*")
				testFindNotOneOfString<StringImpl<DATATYPE>, const char*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const char16_t*")
				testFindNotOneOfString<StringImpl<DATATYPE>, const char16_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const char32_t*")
				testFindNotOneOfString<StringImpl<DATATYPE>, const char32_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const wchar_t*")
				testFindNotOneOfString<StringImpl<DATATYPE>, const wchar_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);


			SECTION("const char* with length")
				testFindNotOneOfStringWithLength<StringImpl<DATATYPE>, const char*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const char16_t* with length")
				testFindNotOneOfStringWithLength<StringImpl<DATATYPE>, const char16_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const char32_t* with length")
				testFindNotOneOfStringWithLength<StringImpl<DATATYPE>, const char32_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const wchar_t* with length")
				testFindNotOneOfStringWithLength<StringImpl<DATATYPE>, const wchar_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);


		}
	}	


	SECTION("char")
		testFindFirstNotOfChar<StringImpl<DATATYPE> >();
}



template<class DATATYPE>
inline void testReverseFindOneOfIterators(const std::u32string& toFind, bool matchPossible)
{	
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	SECTION("fromEnd")
	{
		StringImpl<DATATYPE>::Iterator it = s.reverseFindOneOf(toFind.begin(), toFind.end(), s.end() );
				
		if(matchPossible)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );				
	}

	SECTION("fromMatch")
	{
		StringImpl<DATATYPE>::Iterator it = s.reverseFindOneOf(toFind.begin(), toFind.end(), s.begin()+2 );

		if(matchPossible)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );
	}
			
	SECTION("fromBeforeMatch")
	{
		StringImpl<DATATYPE>::Iterator it = s.reverseFindOneOf(toFind.begin(), toFind.end(), s.begin()+1 );
				
		REQUIRE( it==s.end() );
	}

	SECTION("fromBegin")
	{
		StringImpl<DATATYPE>::Iterator it = s.reverseFindOneOf(toFind.begin(), toFind.end(), s.begin() );
				
		REQUIRE( it==s.end() );
	}


	SECTION("withMultipleMatches")
	{
		StringImpl<DATATYPE> s2 = s;
		s2+=s;

		SECTION("fromEnd")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindOneOf(toFind.begin(), toFind.end(), s2.end() );

			if(matchPossible)
				REQUIRE( it==s2.begin()+12 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindOneOf(toFind.begin(), toFind.end(), s2.begin()+12 );

			if(matchPossible)
				REQUIRE( it==s2.begin()+12 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindOneOf(toFind.begin(), toFind.end(), s2.begin()+11 );
					
			if(matchPossible)
				REQUIRE( it==s2.begin()+2 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindOneOf(toFind.begin(), toFind.end(), s2.begin()+3 );
					
			if(matchPossible)
				REQUIRE( it==s2.begin()+2 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindOneOf(toFind.begin(), toFind.end(), s2.begin()+2 );
					
			if(matchPossible)
				REQUIRE( it==s2.begin()+2 );
			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindOneOf(toFind.begin(), toFind.end(), s2.begin()+1 );

			REQUIRE( it==s2.end() );
		}
	}
}


template<class StringType, class ...Args>
size_t callReverseFindOneOf(StringType& s, Args... args)
{
	size_t result = s.reverseFindOneOf(args...);
	size_t result2 = s.find_last_of(args...);

	REQUIRE( result2==result );

	return result;
}

template<class StringType, class ToFindType>
inline void testReverseFindOneOfString(const StringType& toFindObj, bool matchPossible)
{
	StringType s(U"he\U00012345loworld");

	ToFindType toFind = (ToFindType)toFindObj;

	SECTION("fromEnd")
	{
		size_t result = callReverseFindOneOf(s, toFind, s.getLength() );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromEnd-moreThanLength")
	{
		size_t result = callReverseFindOneOf(s, toFind, s.length()+1 );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromEnd-npos")
	{
		size_t result = callReverseFindOneOf(s, toFind, s.npos );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
	

	SECTION("fromEnd-defaultArg")
	{
		size_t result = callReverseFindOneOf(s, toFind );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromMatch")
	{
		size_t result = callReverseFindOneOf(s, toFind, 2 );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
			
	SECTION("fromBeforeMatch")
	{
		size_t result = callReverseFindOneOf(s, toFind, 1 );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromBegin")
	{
		size_t result = callReverseFindOneOf(s, toFind, 0 );
				
		REQUIRE( result==StringType::noMatch );
	}
	

	SECTION("withMultipleMatches")
	{
		StringType s2 = s;
		s2+=s;

		SECTION("fromEnd")
		{
			size_t result = callReverseFindOneOf(s2, toFind, s2.length() );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromAfterEnd")
		{
			size_t result = callReverseFindOneOf(s2, toFind, s2.length()+1 );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromEnd-npos")
		{
			size_t result = callReverseFindOneOf(s2, toFind, s2.npos );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromEnd-defaultArg")
		{
			size_t result = callReverseFindOneOf(s2, toFind );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 12 );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 11 );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 3 );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );			
		}

		SECTION("fromFirstMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 2 );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 1 );

			REQUIRE( result==s2.noMatch );
		}
	}
}

template<class StringType, class ToFindType>
inline void testReverseFindOneOfStringWithLength(const StringType& toFindObjArg, bool matchPossible)
{
	StringType s(U"he\U00012345loworld");

	StringType toFindObj = toFindObjArg;

	ToFindType toFind = (ToFindType)toFindObj;

	size_t toFindLength = getCStringLength<ToFindType>(toFind);

	toFindObj += U"hexy";

	toFind = (ToFindType)toFindObj;


	SECTION("fromEnd")
	{
		size_t result = callReverseFindOneOf(s, toFind, s.length(), toFindLength );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromEnd-npos")
	{
		size_t result = callReverseFindOneOf(s, toFind, s.npos, toFindLength );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromAfterEnd")
	{
		size_t result = callReverseFindOneOf(s, toFind, s.length()+1, toFindLength );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
	
	SECTION("fromMatch")
	{
		size_t result = callReverseFindOneOf(s, toFind, 2, toFindLength );
				
		if(matchPossible)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
			
	SECTION("fromBeforeMatch")
	{
		size_t result = callReverseFindOneOf(s, toFind, 1, toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}

	SECTION("fromBegin")
	{
		size_t result = callReverseFindOneOf(s, toFind, 0, toFindLength );
				
		REQUIRE( result==StringType::noMatch );
	}
	

	SECTION("withMultipleMatches")
	{
		StringType s2 = s;
		s2+=s;

		SECTION("fromEnd")
		{
			size_t result = callReverseFindOneOf(s2, toFind, s2.length(), toFindLength );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromAfterEnd")
		{
			size_t result = callReverseFindOneOf(s2, toFind, s2.length()+1, toFindLength );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromEnd-npos")
		{
			size_t result = callReverseFindOneOf(s2, toFind, s2.npos, toFindLength );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 12, toFindLength );

			if(matchPossible)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 11, toFindLength );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 3, toFindLength );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );			
		}

		SECTION("fromFirstMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 2, toFindLength );

			if(matchPossible)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			size_t result = callReverseFindOneOf(s2, toFind, 1, toFindLength );

			REQUIRE( result==s2.noMatch );
		}
	}
}


template<class DATATYPE>
inline void testReverseFindOneOf()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	struct MatchData
	{
		const char*			desc;
		const char32_t*		matchString;
		bool				matchPossible;
	};

	std::list<MatchData> matchList = {	{"firstCharMatches",	U"\U00012345xy", true },
										{"second char matches", U"x\U00012345y", true },
										{"last char matches", U"xy\U00012345", true },
										{"single char matches", U"\U00012345", true },
										{"no char matches", U"xyz", false},
										{"empty", U"", false},
										{"single char no match", U"x", false} };

	for( auto matchIt = matchList.begin(); matchIt!=matchList.end(); matchIt++)
	{
		const MatchData& matchData = *matchIt;

		SECTION( matchData.desc )
		{
			SECTION("iterators")
				testReverseFindOneOfIterators<DATATYPE>(matchData.matchString, matchData.matchPossible);

			SECTION("String")
				testReverseFindOneOfString<StringImpl<DATATYPE>, StringImpl<DATATYPE> >( matchData.matchString, matchData.matchPossible);

			SECTION("std::string")
				testReverseFindOneOfString<StringImpl<DATATYPE>, std::string >( matchData.matchString, matchData.matchPossible);

			SECTION("std::wstring")
				testReverseFindOneOfString<StringImpl<DATATYPE>, std::wstring >( matchData.matchString, matchData.matchPossible);

			SECTION("std::u16string")
				testReverseFindOneOfString<StringImpl<DATATYPE>, std::u16string >( matchData.matchString, matchData.matchPossible);

			SECTION("std::u32string")
				testReverseFindOneOfString<StringImpl<DATATYPE>, std::u32string >( matchData.matchString, matchData.matchPossible);

			
			SECTION("const char*")
				testReverseFindOneOfString<StringImpl<DATATYPE>, const char*>(matchData.matchString, matchData.matchPossible);

			SECTION("const char16_t*")
				testReverseFindOneOfString<StringImpl<DATATYPE>, const char16_t*>(matchData.matchString, matchData.matchPossible);

			SECTION("const char32_t*")
				testReverseFindOneOfString<StringImpl<DATATYPE>, const char32_t*>(matchData.matchString, matchData.matchPossible);

			SECTION("const wchar_t*")
				testReverseFindOneOfString<StringImpl<DATATYPE>, const wchar_t*>(matchData.matchString, matchData.matchPossible);


			SECTION("const char* with length")
				testReverseFindOneOfStringWithLength<StringImpl<DATATYPE>, const char*>(matchData.matchString, matchData.matchPossible);

			SECTION("const char16_t* with length")
				testReverseFindOneOfStringWithLength<StringImpl<DATATYPE>, const char16_t*>(matchData.matchString, matchData.matchPossible);

			SECTION("const char32_t* with length")
				testReverseFindOneOfStringWithLength<StringImpl<DATATYPE>, const char32_t*>(matchData.matchString, matchData.matchPossible);

			SECTION("const wchar_t* with length")
				testReverseFindOneOfStringWithLength<StringImpl<DATATYPE>, const wchar_t*>(matchData.matchString, matchData.matchPossible);


		}
	}	
}






template<class DATATYPE>
inline void testReverseFindNotOneOfIterators(const std::u32string& toFind, bool matchThirdChar, bool matchAllChars)
{	
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	SECTION("fromEnd")
	{
		StringImpl<DATATYPE>::Iterator it = s.reverseFindNotOneOf(toFind.begin(), toFind.end(), s.end() );

		if(matchAllChars)
			REQUIRE( it==s.end()-1 );
				
		else if(matchThirdChar)
			REQUIRE( it==s.begin()+2 );

		else
			REQUIRE( it==s.end() );				
	}

	SECTION("fromMatch")
	{
		StringImpl<DATATYPE>::Iterator it = s.reverseFindNotOneOf(toFind.begin(), toFind.end(), s.begin()+2 );

		if(matchAllChars || matchThirdChar)
			REQUIRE( it==s.begin()+2 );
		else
			REQUIRE( it==s.end() );
	}
			
	SECTION("fromBeforeMatch")
	{
		StringImpl<DATATYPE>::Iterator it = s.reverseFindNotOneOf(toFind.begin(), toFind.end(), s.begin()+1 );

		if(matchAllChars)
			REQUIRE( it==s.begin()+1 );
		else
			REQUIRE( it==s.end() );
	}

	SECTION("fromBegin")
	{
		StringImpl<DATATYPE>::Iterator it = s.reverseFindNotOneOf(toFind.begin(), toFind.end(), s.begin() );

		if(matchAllChars)
			REQUIRE( it==s.begin() );
		else
			REQUIRE( it==s.end() );
	}


	SECTION("withMultipleMatches")
	{
		StringImpl<DATATYPE> s2 = s;
		s2+=s;

		SECTION("fromEnd")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindNotOneOf(toFind.begin(), toFind.end(), s2.end() );

			if(matchAllChars)
				REQUIRE( it==s2.end()-1 );

			else if(matchThirdChar)
				REQUIRE( it==s2.begin()+12 );

			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindNotOneOf(toFind.begin(), toFind.end(), s2.begin()+12 );

			if(matchAllChars || matchThirdChar)
				REQUIRE( it==s2.begin()+12 );

			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindNotOneOf(toFind.begin(), toFind.end(), s2.begin()+11 );

			if(matchAllChars)
				REQUIRE( it==s2.begin()+11 );
					
			else if(matchThirdChar)
				REQUIRE( it==s2.begin()+2 );

			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindNotOneOf(toFind.begin(), toFind.end(), s2.begin()+3 );

			if(matchAllChars)
				REQUIRE( it==s2.begin()+3 );
					
			else if(matchThirdChar)
				REQUIRE( it==s2.begin()+2 );

			else
				REQUIRE( it==s2.end() );
		}

		SECTION("fromFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindNotOneOf(toFind.begin(), toFind.end(), s2.begin()+2 );

			if(matchThirdChar || matchAllChars)
				REQUIRE( it==s2.begin()+2 );

			else
				REQUIRE( it==s2.end() );			
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			StringImpl<DATATYPE>::Iterator it = s2.reverseFindNotOneOf(toFind.begin(), toFind.end(), s2.begin()+1 );

			if(matchAllChars)
				REQUIRE( it==s2.begin()+1 );					

			else
				REQUIRE( it==s2.end() );
		}
	}
}


template<class StringType, class ...Args>
size_t callReverseFindNotOneOf(StringType& s, Args... args)
{
	size_t result = s.reverseFindNotOneOf(args...);
	size_t result2 = s.find_last_not_of(args...);

	REQUIRE( result2==result );

	return result;
}

template<class StringType, class ToFindType>
inline void testReverseFindNotOneOfString(const StringType& toFindObj, bool matchThirdChar, bool matchAllChars)
{
	StringType s(U"he\U00012345loworld");

	ToFindType toFind = (ToFindType)toFindObj;

	SECTION("fromEnd")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, s.getLength() );

		if(matchAllChars)
			REQUIRE( result==s.getLength()-1 );
				
		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromEnd-moreThanLength")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, s.length()+1 );

		if(matchAllChars)
			REQUIRE( result==s.getLength()-1 );
				
		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );		
	}

	SECTION("fromEnd-npos")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, s.npos );

		if(matchAllChars)
			REQUIRE( result==s.getLength()-1 );
				
		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );				
	}
	

	SECTION("fromEnd-defaultArg")
	{
		size_t result = callReverseFindNotOneOf(s, toFind );

		if(matchAllChars)
			REQUIRE( result==s.getLength()-1 );
				
		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );			
	}

	SECTION("fromMatch")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, 2 );
				
		if(matchThirdChar || matchAllChars)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
			
	SECTION("fromBeforeMatch")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, 1 );

		if(matchAllChars)
			REQUIRE( result==1 );
				
		else
			REQUIRE( result==StringType::noMatch );						
	}

	SECTION("fromBegin")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, 0 );

		if(matchAllChars)
			REQUIRE( result==0 );				

		else
			REQUIRE( result==StringType::noMatch );		
	}
	

	SECTION("withMultipleMatches")
	{
		StringType s2 = s;
		s2+=s;

		SECTION("fromEnd")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, s2.length() );

			if(matchAllChars)
				REQUIRE( result==s2.getLength()-1 );
				
			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==s2.noMatch );		
		}

		SECTION("fromAfterEnd")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, s2.length()+1 );

			if(matchAllChars)
				REQUIRE( result==s2.getLength()-1 );
				
			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==s2.noMatch );		
		}

		SECTION("fromEnd-npos")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, s2.npos );

			if(matchAllChars)
				REQUIRE( result==s2.getLength()-1 );
				
			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromEnd-defaultArg")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind );

			if(matchAllChars)
				REQUIRE( result==s2.getLength()-1 );
				
			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromSecondMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 12 );

			if(matchAllChars || matchThirdChar)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 11 );

			if(matchAllChars)
				REQUIRE( result==11 );
				
			else if(matchThirdChar)
				REQUIRE( result==2 );

			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 3 );

			if(matchAllChars)
				REQUIRE( result==3 );
				
			else if(matchThirdChar)
				REQUIRE( result==2 );

			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 2 );

			if(matchAllChars || matchThirdChar)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 1 );

			if(matchAllChars)
				REQUIRE( result==1 );				

			else
				REQUIRE( result==s2.noMatch );
		}
	}
}

template<class StringType, class ToFindType>
inline void testReverseFindNotOneOfStringWithLength(const StringType& toFindObjArg, bool matchThirdChar, bool matchAllChars)
{
	StringType s(U"he\U00012345loworld");

	StringType toFindObj = toFindObjArg;

	ToFindType toFind = (ToFindType)toFindObj;

	size_t toFindLength = getCStringLength<ToFindType>(toFind);

	toFindObj += U"hexy";

	toFind = (ToFindType)toFindObj;
	
	
	SECTION("fromEnd")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, s.getLength(), toFindLength );

		if(matchAllChars)
			REQUIRE( result==s.getLength()-1 );
				
		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );				
	}

	SECTION("fromEnd-moreThanLength")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, s.length()+1, toFindLength );

		if(matchAllChars)
			REQUIRE( result==s.getLength()-1 );
				
		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );		
	}

	SECTION("fromEnd-npos")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, s.npos, toFindLength );

		if(matchAllChars)
			REQUIRE( result==s.getLength()-1 );
				
		else if(matchThirdChar)
			REQUIRE( result==2 );

		else
			REQUIRE( result==StringType::noMatch );				
	}
	

	SECTION("fromMatch")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, 2, toFindLength );
				
		if(matchThirdChar || matchAllChars)
			REQUIRE( result==2 );
		else
			REQUIRE( result==StringType::noMatch );				
	}
			
	SECTION("fromBeforeMatch")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, 1, toFindLength );

		if(matchAllChars)
			REQUIRE( result==1 );
				
		else
			REQUIRE( result==StringType::noMatch );						
	}

	SECTION("fromBegin")
	{
		size_t result = callReverseFindNotOneOf(s, toFind, 0, toFindLength );

		if(matchAllChars)
			REQUIRE( result==0 );				

		else
			REQUIRE( result==StringType::noMatch );		
	}
	

	SECTION("withMultipleMatches")
	{
		StringType s2 = s;
		s2+=s;

		SECTION("fromEnd")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, s2.length(), toFindLength );

			if(matchAllChars)
				REQUIRE( result==s2.getLength()-1 );
				
			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==s2.noMatch );		
		}

		SECTION("fromAfterEnd")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, s2.length()+1, toFindLength );

			if(matchAllChars)
				REQUIRE( result==s2.getLength()-1 );
				
			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==s2.noMatch );		
		}

		SECTION("fromEnd-npos")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, s2.npos, toFindLength );

			if(matchAllChars)
				REQUIRE( result==s2.getLength()-1 );
				
			else if(matchThirdChar)
				REQUIRE( result==12 );

			else
				REQUIRE( result==s2.noMatch );
		}


		SECTION("fromSecondMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 12, toFindLength );

			if(matchAllChars || matchThirdChar)
				REQUIRE( result==12 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeSecondMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 11, toFindLength );

			if(matchAllChars)
				REQUIRE( result==11 );
				
			else if(matchThirdChar)
				REQUIRE( result==2 );

			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustAfterFirstMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 3, toFindLength );

			if(matchAllChars)
				REQUIRE( result==3 );
				
			else if(matchThirdChar)
				REQUIRE( result==2 );

			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromFirstMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 2, toFindLength );

			if(matchAllChars || matchThirdChar)
				REQUIRE( result==2 );
			else
				REQUIRE( result==s2.noMatch );
		}

		SECTION("fromJustBeforeFirstMatch")
		{
			size_t result = callReverseFindNotOneOf(s2, toFind, 1, toFindLength );

			if(matchAllChars)
				REQUIRE( result==1 );				

			else
				REQUIRE( result==s2.noMatch );
		}
	}
}



template<class StringType>
inline void testFindLastNotOfChar()
{
	StringType s(U"he\U00012345loworld");
	StringType empty(U"");

	size_t result = s.find_last_not_of(U'x');
	REQUIRE( result==s.getLength()-1 );

	result = s.find_last_not_of(U'd');
	REQUIRE( result==s.getLength()-2 );

	result = s.find_last_not_of(U'l');
	REQUIRE( result==s.getLength()-1 );

	result = s.find_last_not_of(U'l', s.getLength() );
	REQUIRE( result==s.getLength()-1 );

	result = s.find_last_not_of(U'l', s.npos );
	REQUIRE( result==s.getLength()-1 );

	result = s.find_last_not_of(U'l', s.getLength()-1);
	REQUIRE( result==s.getLength()-1 );

	result = s.find_last_not_of(U'l', s.getLength()-2);
	REQUIRE( result==s.getLength()-3 );

	result = s.find_last_not_of(U'l', s.getLength()-3);
	REQUIRE( result==s.getLength()-3 );

	result = s.find_last_not_of(U'l', 0);
	REQUIRE( result==s.noMatch );

	result = empty.find_last_not_of(U'e');
	REQUIRE( result==s.noMatch );

	result = empty.find_last_not_of(U'e', 1);
	REQUIRE( result==s.noMatch );

	result = empty.find_last_not_of(U'e', 10);
	REQUIRE( result==s.noMatch );

	result = empty.find_last_not_of(U'e', s.npos);
	REQUIRE( result==s.noMatch );
}

template<class DATATYPE>
inline void testReverseFindNotOneOf()
{
	StringImpl<DATATYPE> s(U"he\U00012345loworld");

	struct MatchData
	{
		const char*			desc;
		const char32_t*		matchString;
		bool				matchThirdChar;
		bool				matchAllChars;
	};

	std::list<MatchData> matchList = {	{"match",	U"\U00033333helowrd", true, false },
										{"noMatch",	U"\U00012345helowrd", false, false },
										{"matchAny-empty",	U"", true, true },
										{"matchAny-otherChars",	U"abc", true, true }
									};

	for( auto matchIt = matchList.begin(); matchIt!=matchList.end(); matchIt++)
	{
		const MatchData& matchData = *matchIt;

		SECTION( matchData.desc )
		{
			SECTION("iterators")
				testReverseFindNotOneOfIterators<DATATYPE>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("String")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, StringImpl<DATATYPE> >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("std::string")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, std::string >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("std::wstring")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, std::wstring >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("std::u16string")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, std::u16string >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("std::u32string")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, std::u32string >( matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			
			SECTION("const char*")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, const char*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const char16_t*")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, const char16_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const char32_t*")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, const char32_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const wchar_t*")
				testReverseFindNotOneOfString<StringImpl<DATATYPE>, const wchar_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);


			SECTION("const char* with length")
				testReverseFindNotOneOfStringWithLength<StringImpl<DATATYPE>, const char*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const char16_t* with length")
				testReverseFindNotOneOfStringWithLength<StringImpl<DATATYPE>, const char16_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const char32_t* with length")
				testReverseFindNotOneOfStringWithLength<StringImpl<DATATYPE>, const char32_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);

			SECTION("const wchar_t* with length")
				testReverseFindNotOneOfStringWithLength<StringImpl<DATATYPE>, const wchar_t*>(matchData.matchString, matchData.matchThirdChar, matchData.matchAllChars);


		}
	}	


	SECTION("char")
		testFindLastNotOfChar<StringImpl<DATATYPE> >();
}



template<class DATATYPE>
inline void testStringImpl()
{
	SECTION("types")
		testTypes<DATATYPE>();

	SECTION("constants")
		testConstants<DATATYPE>();

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

	SECTION("max_size")
	{
		StringImpl<DATATYPE> s;

		size_t m = s.max_size();

		// max_size should not be bigger than INT_MAX,
		// since we use int for lengths and indices
		REQUIRE( m<=INT_MAX );

		// but it should have a "reasonably high" value.
		REQUIRE( m>=0x40000000/sizeof(DATATYPE::EncodedElement) );
	}

	SECTION("replace")
	{
		testReplace<DATATYPE>();
	}

	SECTION("append")
	{
		testAppend<DATATYPE>();
	}

	SECTION("insert")
	{
		testInsert<DATATYPE>();
	}

	SECTION("erase")
	{
		testErase<DATATYPE>();
	}

	SECTION("assign")
	{
		testAssign<DATATYPE>();
	}

	SECTION("resize")
	{
		testResize<DATATYPE>();
	}

	SECTION("swap")
		testSwap<DATATYPE>();

	SECTION("removeLast")
		testRemoveLast<DATATYPE>();

	SECTION("reserve-capacity")
		testReserveCapacity<DATATYPE>();

	SECTION("getAllocator")
		testGetAllocator<DATATYPE>();	

	SECTION("copy")
		testCopy<DATATYPE>();

	SECTION("IteratorWithIndex")
		testIteratorWithIndex<DATATYPE>();

	SECTION("find")
		testFind<DATATYPE>();

	SECTION("rfind")
		testRFind<DATATYPE>();
	
	SECTION("findCondition")
		testFindCondition<DATATYPE>();

	SECTION("reverseFindCondition")
		testReverseFindCondition<DATATYPE>();
	
	SECTION("findOneOf")
		testFindOneOf<DATATYPE>();

	SECTION("findNotOneOf")
		testFindNotOneOf<DATATYPE>();

	SECTION("reverseFindOneOf")
		testReverseFindOneOf<DATATYPE>();

	SECTION("reverseFindNotOneOf")
		testReverseFindNotOneOf<DATATYPE>();
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

	for(int t=0; t<dataCount; t++)
	{
		SubTestData* pCurrData = &allData[t];

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
}



