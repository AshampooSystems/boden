#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Toggle.h>
#include <bdn/Window.h>
#include <bdn/win32/CheckboxCore.h>
#include <bdn/test/TestToggleCore.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;


class TestWin32ToggleCore : public bdn::test::TestWin32ViewCoreMixin< bdn::test::TestToggleCore >
{
protected:

    void verifyCoreLabel() override
    {
        String expectedLabel = _pToggle->label();

        String label = bdn::win32::Win32Window::getWindowText(_hwnd);
        
        REQUIRE( label == expectedLabel );
    }

    void verifyCoreOn() override
    {
    	bool expectedOn = _pToggle->on();
    	bool on = cast<win32::CheckboxCore<Toggle>>(_pWin32Core)->_on();
    	REQUIRE( on == expectedOn );
    }

};

TEST_CASE("win32.ToggleCore")
{
    P<TestWin32ToggleCore> pTest = newObj<TestWin32ToggleCore>();

    pTest->runTests();
}





