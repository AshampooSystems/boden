#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/test/testWindowCore.h>

using namespace bdn;

// generic tests for the default window core implementation
TEST_CASE("WindowCore-default")
{
    P<Window> pWindow = newObj<Window>();
    
    bdn::test::testWindowCore( pWindow );        
}


