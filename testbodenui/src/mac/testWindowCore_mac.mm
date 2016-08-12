#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>

#import <bdn/mac/UiProvider.hh>
#import <bdn/mac/WindowCore.hh>
#import <bdn/mac/test/testMacViewCore.hh>

using namespace bdn;

TEST_CASE("WindowCore-mac")
{
    P<Window> pWindow = newObj<Window>( &bdn::mac::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("mac-view")
        bdn::mac::test::testMacViewCore(pWindow, pWindow, false);

    SECTION("mac-window")
    {
        P<bdn::mac::WindowCore> pCore = cast<bdn::mac::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        NSWindow* pNS = pCore->getNSWindow();
        REQUIRE( pNS!=NULL );

        SECTION("title")
        {
            // setTitle should change the window test
            pWindow->title() = "hello world";
            
            String text = bdn::mac::macStringToString( pNS.title );
            
            REQUIRE( text == "hello world" );
        }
    }
}




