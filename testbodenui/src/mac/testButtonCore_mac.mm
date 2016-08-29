#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>

#import <bdn/mac/UiProvider.hh>
#import <bdn/mac/ButtonCore.hh>
#import <bdn/mac/test/testMacViewCore.hh>

using namespace bdn;

TEST_CASE("ButtonCore-mac")
{
    P<Window> pWindow = newObj<Window>( &bdn::mac::UiProvider::get() );

    P<Button> pButton = newObj<Button>();

    pWindow->setContentView(pButton);

    SECTION("generic")
        bdn::test::testButtonCore(pWindow, pButton );        

    SECTION("mac-view")
        bdn::mac::test::testMacViewCore(pWindow, pButton, true);

    SECTION("mac-button")
    {
        P<bdn::mac::ButtonCore> pCore = cast<bdn::mac::ButtonCore>( pButton->getViewCore() );
        REQUIRE( pCore!=nullptr );
        
        NSButton* pNS = (NSButton*)pCore->getNSView();

        REQUIRE(  pNS!=nullptr );

        SECTION("label")
        {
            // setLabel should change the window test
            pButton->label() = "hello world";

            String text = bdn::mac::macStringToString( pNS.title );

            REQUIRE( text == "hello world" );
        }                
    }
}




