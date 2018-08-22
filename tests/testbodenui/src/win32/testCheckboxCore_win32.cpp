#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/win32/CheckboxCore.h>
#include <bdn/test/TestCheckboxCore.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;


class TestWin32CheckboxCore : public bdn::test::TestWin32ViewCoreMixin< bdn::test::TestCheckboxCore >
{
protected:

    void verifyCoreLabel() override
    {
        String expectedLabel = _pCheckbox->label();
        String label = bdn::win32::Win32Window::getWindowText(_hwnd);
        REQUIRE( label == expectedLabel );
    }

    void verifyCoreState() override
    {
    	TriState expectedState = _pCheckbox->state();
    	TriState state = cast<win32::CheckboxCore<Checkbox>>(_pWin32Core)->_state();
    	REQUIRE( state == expectedState );
    }

};

TEST_CASE("win32.CheckboxCore")
{
    P<TestWin32CheckboxCore> pTest = newObj<TestWin32CheckboxCore>();

    pTest->runTests();
}





