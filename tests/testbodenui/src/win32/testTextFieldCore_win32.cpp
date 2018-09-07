#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextField.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextFieldCore.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;

class TestWin32TextFieldCore
    : public bdn::test::TestWin32ViewCoreMixin<bdn::test::TestTextFieldCore>
{
  protected:
    void verifyCoreText() override
    {
        String expectedText = _pTextField->text();
        String text = bdn::win32::Win32Window::getWindowText(_hwnd);
        REQUIRE(text == expectedText);
    }
};

TEST_CASE("win32.TextFieldCore")
{
    P<TestWin32TextFieldCore> pTest = newObj<TestWin32TextFieldCore>();
    pTest->runTests();
}