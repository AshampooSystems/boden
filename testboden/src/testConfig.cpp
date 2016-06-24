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
}
    
    
    
    
}


