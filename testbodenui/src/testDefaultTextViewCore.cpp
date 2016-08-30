#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testTextViewCore.h>

using namespace bdn;

// generic tests for the default text view core implementation
TEST_CASE("TextViewCore-default")
{
    P<Window> pWindow = newObj<Window>();

    P<TextView> pTextView = newObj<TextView>();

    pWindow->setContentView(pTextView);

    bdn::test::testTextViewCore(pWindow, pTextView );        
}




