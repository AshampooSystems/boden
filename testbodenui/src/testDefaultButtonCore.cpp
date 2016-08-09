#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>

using namespace bdn;

// generic tests for the default button core implementation
TEST_CASE("ButtonCore-default")
{
    P<Window> pWindow = newObj<Window>();

    P<Button> pButton = newObj<Button>();

    pWindow->setContentView(pButton);

    bdn::test::testButtonCore( pButton );        
}




