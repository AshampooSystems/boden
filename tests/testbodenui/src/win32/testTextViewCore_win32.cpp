#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#include "TestWin32ViewCoreMixin.h"

using namespace bdn;

class TestWin32TextViewCore
    : public bdn::test::TestWin32ViewCoreMixin<bdn::test::TestTextViewCore>
{
  protected:
    void verifyCoreText() override
    {
        String expectedText = _pTextView->text();

        String text = bdn::win32::Win32Window::getWindowText(_hwnd);

        REQUIRE(text == expectedText);
    }
};

TEST_CASE("win32.TextViewCore")
{
    P<TestWin32TextViewCore> pTest = newObj<TestWin32TextViewCore>();

    pTest->runTests();
}
