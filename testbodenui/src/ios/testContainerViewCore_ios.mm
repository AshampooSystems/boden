#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/test/testContainerViewCore.h>

#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/test/testIosViewCore.hh>

using namespace bdn;

TEST_CASE("ContainerViewCore-ios")
{
    P<Window> pWindow = newObj<Window>();

    // ContainerView is an abstract base class, so we cannot instantiate it.
    // But we can use any ContainerView subclass for these tests because
    // they all use the same core.

    P<ColumnView> pColumnView = newObj<ColumnView>();

    pWindow->setContentView(pColumnView);

    SECTION("generic")
        bdn::test::testContainerViewCore(pWindow, pColumnView );        

    SECTION("ios-ViewCore")
        bdn::ios::test::testIosViewCore(pWindow, pColumnView, false, true);

    SECTION("ios-ContainerViewCore")
    {
        P<bdn::ios::ContainerViewCore> pCore = cast<bdn::ios::ContainerViewCore>( pColumnView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        UIView* pUIView = pCore->getUIView();
        REQUIRE(  pUIView!=nullptr );

        // there is nothing mac specific to test here.
        // The generic container view tests and the mac view test have already tested
        // everything that the container view core does.        
    }
}




