#include <bdn/init.h>

#include <bdn/test.h>

namespace bdn
{
    
TEST_CASE( "config" )
{
    REQUIRE( BDN_WCHAR_SIZE == sizeof(wchar_t) );
}
    
    
}


