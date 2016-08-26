#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>

#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/WindowCore.hh>
#import <bdn/ios/test/testIosViewCore.hh>

using namespace bdn;

TEST_CASE("WindowCore-ios")
{
    P<Window> pWindow = newObj<Window>( &bdn::ios::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("ios-view")
        bdn::ios::test::testIosViewCore(pWindow, pWindow, false, false);

    SECTION("ios-window")
    {
        P<bdn::ios::WindowCore> pCore = cast<bdn::ios::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        UIWindow* pUIWindow = pCore->getUIWindow();
        REQUIRE( pUIWindow!=NULL );

        SECTION("title")
        {
            // setTitle should change the window test
            pWindow->title() = "hello world";
            
            String text = bdn::ios::iosStringToString( pUIWindow.rootViewController.title );
            
            REQUIRE( text == "hello world" );
        }
    }
}




