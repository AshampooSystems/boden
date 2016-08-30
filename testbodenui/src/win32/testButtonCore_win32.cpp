#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/ButtonCore.h>
#include "testWin32ViewCore.h"

using namespace bdn;

TEST_CASE("ButtonCore-win32")
{
    P<Window> pWindow = newObj<Window>( &bdn::win32::UiProvider::get() );

    P<Button> pButton = newObj<Button>();

    SECTION("init")
    {
        SECTION("ViewCore")
            bdn::win32::test::testWin32ViewCoreInitialization( pWindow, pButton);

        SECTION("ButtonCore")
        {
            SECTION("label")
                pButton->label() == "hello";

            String expectedLabel = pButton->label();
                
            pWindow->setContentView(pButton);

            P<bdn::win32::ButtonCore> pCore = cast<bdn::win32::ButtonCore>( pButton->getViewCore() );
            REQUIRE( pCore!=nullptr );

            String text = bdn::win32::Win32Window::getWindowText( pCore->getHwnd() );

            REQUIRE( text == expectedLabel );
        }
    }

    SECTION("postInit")
    {
        pWindow->setContentView(pButton);

        SECTION("generic")
            bdn::test::testButtonCore(pWindow, pButton );        
    
        SECTION("win32")
        {
            SECTION("ViewCore")
                bdn::win32::test::testWin32ViewCore(pWindow, pButton, true);

            SECTION("ButtonCore")
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
    }
}




