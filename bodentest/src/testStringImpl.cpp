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

		REQUIRE( !s.isEmpty() );
		REQUIRE( s2.isEmpty() );
		REQUIRE( s3.isEmpty() );
	}

	SECTION("subString")
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
			}

			SECTION("atLast")
			{
				StringImpl<DATATYPE> s2 = s.subString(9, 0);
				REQUIRE( s2=="" );

				StringImpl<DATATYPE> s3 = s.subString( s.end()-1, s.end()-1);
				REQUIRE( s3=="" );
			}

			SECTION("atEnd")
			{
				StringImpl<DATATYPE> s2 = s.subString(10, 0);
				REQUIRE( s2=="" );

				StringImpl<DATATYPE> s3 = s.subString( s.end(), s.end() );
				REQUIRE( s3=="" );
			}

			SECTION("inMiddle")
			{
				StringImpl<DATATYPE> s2 = s.subString(5, 0);
				REQUIRE( s2=="" );

				StringImpl<DATATYPE> s3 = s.subString( s.begin()+5, s.begin()+5 );
				REQUIRE( s3=="" );
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

