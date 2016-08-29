#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/WindowCore.h>
#include "testWin32ViewCore.h"

using namespace bdn;

TEST_CASE("WindowCore-win32")
{
    P<Window> pWindow = newObj<Window>( &bdn::win32::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("win32-view")
        bdn::win32::test::testWin32ViewCore(pWindow, pWindow, false);

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

    SECTION("HWND destroyed when object destroyed")
    {
        // there may be pending sizing info updates for the window, which keep it alive.
        // Ensure that those are done first.

        // wrap pWindow in a struct so that we can destroy all references
        // in the continuation.
        struct CaptureData : public Base
        {
            P<Window> pWindow;
        };
        P<CaptureData> pData = newObj<CaptureData>();
        pData->pWindow = pWindow;
        pWindow = nullptr;
        
        CONTINUE_SECTION_ASYNC(pData)
        {
            P<bdn::win32::WindowCore> pCore = cast<bdn::win32::WindowCore>( pData->pWindow->getViewCore() );
            REQUIRE( pCore!=nullptr );

            HWND hwnd = pCore->getHwnd();
            REQUIRE( hwnd!=NULL );

            SECTION("core not kept alive")
            {
                pCore = nullptr;
            }

            SECTION("core kept alive")
            {
                // do nothing
            }        

            pData->pWindow = nullptr;

            // window should have been destroyed
            REQUIRE( ! ::IsWindow(hwnd) );
        };
    }
}




