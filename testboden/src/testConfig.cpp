#include <bdn/init.h>

#include <bdn/test.h>

namespace bdn
{
    
TEST_CASE( "config" )
{
    SECTION("charSize")
        REQUIRE( BDN_WCHAR_SIZE == sizeof(wchar_t) );
    
    SECTION("BDN_APPEND_LINE")
    {
        struct Helper
        {
            static int& getConstructedCount()
            {
                static int count=0;
                return count;
            }
            
            Helper()
            {
                getConstructedCount()++;
            }
        };
        
        Helper BDN_APPEND_LINE(myName);
        Helper BDN_APPEND_LINE(myName);
        
        REQUIRE( Helper::getConstructedCount() == 2 );
        
    }

	SECTION("endian macros")
	{		
#if !defined(BDN_IS_BIG_ENDIAN) || !defined(BDN_IS_LITTLE_ENDIAN)
		// should both be defined
		REQUIRE( false );
#endif

		// this line simply enforces that BDN_IS_BIG_ENDIAN and BDN_IS_LITTLE_ENDIAN
		// can be evaluated at compile time. Otherwise we will get a compile error here
#if BDN_IS_BIG_ENDIAN && BDN_IS_LITTLE_ENDIAN
		REQUIRE( true );
#endif

		bool compileTimeLittleEndian = BDN_IS_LITTLE_ENDIAN;
		bool compileTimeBigEndian = BDN_IS_BIG_ENDIAN;

		uint32_t val = 0xff;
		bool runTimeLittleEndian = ( ((uint8_t*)&val)[0] == 0xff );
		
		REQUIRE( compileTimeLittleEndian == runTimeLittleEndian );
		REQUIRE( compileTimeBigEndian == !runTimeLittleEndian );
	}

}
    
    
    
    
}


