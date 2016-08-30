#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextView.h>
#include <bdn/Window.h>
#include <bdn/test/testTextViewCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/TextViewCore.h>
#include "testWin32ViewCore.h"

using namespace bdn;

TEST_CASE("TextViewCore-win32")
{
    P<Window> pWindow = newObj<Window>( &bdn::win32::UiProvider::get() );

    P<TextView> pTextView = newObj<TextView>();

    SECTION("init")
    {
        SECTION("ViewCore")
            bdn::win32::test::testWin32ViewCoreInitialization( pWindow, pTextView);

        SECTION("TextViewCore")
        {            
            SECTION("text")
                pTextView->text() = "hello";

            String expectedText = pTextView->text();
                
            pWindow->setContentView(pTextView);

            P<bdn::win32::TextViewCore> pCore = cast<bdn::win32::TextViewCore>( pTextView->getViewCore() );
            REQUIRE( pCore!=nullptr );

            String text = bdn::win32::Win32Window::getWindowText( pCore->getHwnd() );

            REQUIRE( text == expectedText );
        }
    }

    SECTION("postInit")
    {
        pWindow->setContentView(pTextView);

        SECTION("generic")
            bdn::test::testTextViewCore(pWindow, pTextView );        

        SECTION("win32")
        {
            SECTION("ViewCore")
                bdn::win32::test::testWin32ViewCore(pWindow, pTextView, true);

            SECTION("TextViewCore")
            {
                P<bdn::win32::TextViewCore> pCore = cast<bdn::win32::TextViewCore>( pTextView->getViewCore() );
                REQUIRE( pCore!=nullptr );

                HWND hwnd = pCore->getHwnd();
                REQUIRE( hwnd!=NULL );

                SECTION("text")
                {
                    pTextView->text() = "hello world";

                    String text = bdn::win32::Win32Window::getWindowText(hwnd);

                    REQUIRE( text == "hello world" );
                }                
            }
        }
    }
}




