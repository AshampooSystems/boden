#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/hex.h>

using namespace bdn;

TEST_CASE("isHexDigit")
{
	REQUIRE( isHexDigit('0') );
	REQUIRE( isHexDigit('1') );
	REQUIRE( isHexDigit('9') );
	REQUIRE( isHexDigit('a') );
	REQUIRE( isHexDigit('f') );
	REQUIRE( isHexDigit('A') );
	REQUIRE( isHexDigit('F') );
	
	REQUIRE( !isHexDigit('g') );
	REQUIRE( !isHexDigit('z') );
	REQUIRE( !isHexDigit('G') );
	REQUIRE( !isHexDigit('Z') );
	REQUIRE( !isHexDigit('?') );

	REQUIRE( !isHexDigit(0) );
	REQUIRE( !isHexDigit(12345) );
}

TEST_CASE("decodeHexDigit")
{
	for(int upper=0; upper<2; upper++)
	{
		for(int i=0; i<=15; i++)
		{
			char32_t chr;
			if(i<=9)
				chr = '0'+i;
			else if(upper)
				chr = 'A'+i-10;
			else
				chr = 'a'+i-10;

			REQUIRE( decodeHexDigit(chr) == i );			
		}
	}

	for(int i=0; i<1000; i++)
	{
		char32_t chr = (char32_t)i;

		if(!isHexDigit(chr))
			REQUIRE( decodeHexDigit(chr)==-1 );			
	}
}

TEST_CASE("encodeHexDigit")
{
	for(int i=0;i<15;i++)
	{
		char32_t chr = encodeHexDigit(i);

		if(i<=9)
			REQUIRE( chr=='0'+i );
		else
			REQUIRE( chr=='a'+i-10 );
	}

	REQUIRE( encodeHexDigit(-1)==0 );
	REQUIRE( encodeHexDigit(16)==0 );
	REQUIRE( encodeHexDigit(100)==0 );
}


