#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/WindowCore.h>
#include <bdn/win32/test/testWin32ViewCore.h>

using namespace bdn;

TEST_CASE("WindowCore-win32")
{
    P<Window> pWindow = newObj<Window>( &bdn::win32::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore( pWindow );        

    SECTION("win32-view")
        bdn::win32::test::testWin32ViewCore(pWindow, true);

    SECTION("win32-window")
    {
        P<bdn::win32::WindowCore> pCore = cast<bdn::win32::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        HWND hwnd = pCore->getHwnd();
        REQUIRE( hwnd!=NULL );

        SECTION("title")
        {
            // setTitle should change the window test
            pWindow->title() = "hello world";

            String text = bdn::win32::Win32Window::getWindowText(hwnd);

            REQUIRE( text == "hello world" );
        }                
    }
}




