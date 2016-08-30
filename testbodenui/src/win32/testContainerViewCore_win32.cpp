#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/test/testContainerViewCore.h>

#include <bdn/win32/UiProvider.h>
#include <bdn/win32/ContainerViewCore.h>
#include "testWin32ViewCore.h"

using namespace bdn;

TEST_CASE("ContainerViewCore-win32")
{
    P<Window> pWindow = newObj<Window>();

    // ContainerView is an abstract base class, so we cannot instantiate it.
    // But we can use any ContainerView subclass for these tests because
    // they all use the same core.

    P<ColumnView> pColumnView = newObj<ColumnView>();

    SECTION("init")
    {
        SECTION("ViewCore")
            bdn::win32::test::testWin32ViewCoreInitialization( pWindow, pColumnView);

        SECTION("ContainerViewCore")
        {
            // nothing container-view specific to test
        }
    }

    SECTION("postInit")
    {
        pWindow->setContentView(pColumnView);

        SECTION("generic")
            bdn::test::testContainerViewCore(pWindow, pColumnView );        

        SECTION("win32")
        {
            SECTION("ViewCore")
                bdn::win32::test::testWin32ViewCore(pWindow, pColumnView, false);

            SECTION("ContainerViewCore")
            {
                P<bdn::win32::ContainerViewCore> pCore = cast<bdn::win32::ContainerViewCore>( pColumnView->getViewCore() );
                REQUIRE( pCore!=nullptr );

                HWND hwnd = pCore->getHwnd();
                REQUIRE( hwnd!=NULL );

                // there is nothing win32-specific to test here.
                // The generic container view tests and the win32 view test have already tested
                // everything that the container view core does.        
            }
        }
    }
}




