#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/test/testContainerViewCore.h>

#include <bdn/web/UiProvider.h>
#include <bdn/web/ContainerViewCore.h>
#include "testWebViewCore.h"

using namespace bdn;

TEST_CASE("ContainerViewCore-web")
{
    P<Window> pWindow = newObj<Window>();

    // ContainerView is an abstract base class, so we cannot instantiate it.
    // But we can use any ContainerView subclass for these tests because
    // they all use the same core.

    P<ColumnView> pColumnView = newObj<ColumnView>();

    pWindow->setContentView(pColumnView);

    SECTION("generic")
        bdn::test::testContainerViewCore(pWindow, pColumnView );        

    SECTION("web-ViewCore")
        bdn::web::test::testWebViewCore(pWindow, pColumnView, false);

    SECTION("web-ContainerViewCore")
    {
        P<bdn::web::ContainerViewCore> pCore = cast<bdn::web::ContainerViewCore>( pColumnView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        emscripten::val domObject = pCore->getDomObject();
        REQUIRE( !domObject.isNull() );
        REQUIRE( !domObject.isUndefined() );


        // there is nothing web-specific to test here.
        // The generic container view tests and the web view test have already tested
        // everything that the container view core does.        
    }
}




