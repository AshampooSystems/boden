#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/test/testContainerViewCore.h>

#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/ContainerViewCore.h>
#include "testWinuwpViewCore.h"

using namespace bdn;

TEST_CASE("ContainerViewCore-winuwp")
{
    P<Window> pWindow = newObj<Window>();

    // ContainerView is an abstract base class, so we cannot instantiate it.
    // But we can use any ContainerView subclass for these tests because
    // they all use the same core.

    P<ColumnView> pColumnView = newObj<ColumnView>();

    pWindow->setContentView(pColumnView);

    SECTION("generic")
        bdn::test::testContainerViewCore(pWindow, pColumnView );        

    SECTION("winuwp-ViewCore")
        bdn::winuwp::test::testWinuwpViewCore(pWindow, pColumnView, false, true);

    SECTION("winuwp-ContainerViewCore")
    {
        P<bdn::winuwp::ContainerViewCore> pCore = cast<bdn::winuwp::ContainerViewCore>( pColumnView->getViewCore() );
        REQUIRE( pCore!=nullptr );


        // there is nothing winuwp-specific to test here.
        // The generic container view tests and the winuwp view test have already tested
        // everything that the container view core does.        
    }
}




