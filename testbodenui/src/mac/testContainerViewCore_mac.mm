#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/test/testContainerViewCore.h>

#import <bdn/mac/UiProvider.hh>
#import <bdn/mac/ContainerViewCore.hh>
#import "testMacViewCore.hh"

using namespace bdn;

TEST_CASE("ContainerViewCore-mac")
{
    P<Window> pWindow = newObj<Window>();

    // ContainerView is an abstract base class, so we cannot instantiate it.
    // But we can use any ContainerView subclass for these tests because
    // they all use the same core.

    P<ColumnView> pColumnView = newObj<ColumnView>();

    pWindow->setContentView(pColumnView);

    SECTION("generic")
        bdn::test::testContainerViewCore(pWindow, pColumnView );        

    SECTION("mac-ViewCore")
        bdn::mac::test::testMacViewCore(pWindow, pColumnView, false);

    SECTION("mac-ContainerViewCore")
    {
        P<bdn::mac::ContainerViewCore> pCore = cast<bdn::mac::ContainerViewCore>( pColumnView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        NSView* pNS = pCore->getNSView();
        REQUIRE(  pNS!=nullptr );

        // there is nothing mac specific to test here.
        // The generic container view tests and the mac view test have already tested
        // everything that the container view core does.        
    }
}




