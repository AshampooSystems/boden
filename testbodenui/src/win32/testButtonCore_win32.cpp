#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/ButtonCore.h>
#include <bdn/win32/test/testWin32ViewCore.h>

using namespace bdn;

TEST_CASE("ButtonCore-win32")
{
    P<Window> pWindow = newObj<Window>( &bdn::win32::UiProvider::get() );

    P<Button> pButton = newObj<Button>();

    pWindow->setContentView(pButton);

    SECTION("generic")
        bdn::test::testButtonCore( pButton );        

    SECTION("win32-view")
        bdn::win32::test::testWin32ViewCore(pButton, true);

    SECTION("win32-button")
    {
        P<bdn::win32::ButtonCore> pCore = cast<bdn::win32::ButtonCore>( pButton->getViewCore() );
        REQUIRE( pCore!=nullptr );

        HWND hwnd = pCore->getHwnd();
        REQUIRE( hwnd!=NULL );

        SECTION("label")
        {
            // setLabel should change the window test
            pButton->label() = "hello world";

            String text = bdn::win32::Win32Window::getWindowText(hwnd);

            REQUIRE( text == "hello world" );
        }                
    }
}




