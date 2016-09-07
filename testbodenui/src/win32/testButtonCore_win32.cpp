#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;


class TestWin32ButtonCore : public bdn::test::TestWin32ViewCoreMixin< bdn::test::TestButtonCore >
{
protected:

    void verifyCoreLabel() override
    {
        String expectedLabel = _pButton->label();

        String label = bdn::win32::Win32Window::getWindowText(_hwnd);
        
        REQUIRE( label == expectedLabel );
    }
};

TEST_CASE("win32.ButtonCore")
{
    P<TestWin32ButtonCore> pTest = newObj<TestWin32ButtonCore>();

    pTest->runTests();
}





