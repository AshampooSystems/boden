#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/Window.h>
#include <bdn/win32/SwitchCore.h>
#include <bdn/test/TestSwitchCore.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;

class TestWin32SwitchCore
    : public bdn::test::TestWin32ViewCoreMixin<bdn::test::TestSwitchCore>
{
  protected:
    void verifyCoreLabel() override
    {
        String expectedLabel = _pSwitch->label();

        String label = bdn::win32::Win32Window::getWindowText(_hwnd);

        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pSwitch->on();
        bool on = cast<win32::SwitchCore<Switch>>(_pWin32Core)->_on();
        REQUIRE(on == expectedOn);
    }
};

TEST_CASE("win32.SwitchCore")
{
    P<TestWin32SwitchCore> pTest = newObj<TestWin32SwitchCore>();

    pTest->runTests();
}
