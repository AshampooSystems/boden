#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>

#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/ButtonCore.hh>
#import <bdn/ios/test/testIosViewCore.hh>

using namespace bdn;

TEST_CASE("ButtonCore-ios")
{
    P<Window> pWindow = newObj<Window>( &bdn::ios::UiProvider::get() );

    P<Button> pButton = newObj<Button>();

    pWindow->setContentView(pButton);

    SECTION("generic")
        bdn::test::testButtonCore(pWindow, pButton );        

    SECTION("ios-view")
        bdn::ios::test::testIosViewCore(pWindow, pButton, true, true);

    SECTION("ios-button")
    {
        P<bdn::ios::ButtonCore> pCore = cast<bdn::ios::ButtonCore>( pButton->getViewCore() );
        REQUIRE( pCore!=nullptr );
        
        UIButton* pUIButton = pCore->getUIButton();

        REQUIRE(  pUIButton!=nullptr );

        SECTION("label")
        {
            // setLabel should change the window test
            pButton->label() = "hello world";

            String text = bdn::ios::iosStringToString( pUIButton.currentTitle );

            REQUIRE( text == "hello world" );
        }                
    }
}




