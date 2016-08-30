#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/ButtonCore.h>
#include "testWinuwpViewCore.h"

using namespace bdn;

TEST_CASE("ButtonCore-winuwp")
{
    P<Window> pWindow = newObj<Window>( &bdn::winuwp::UiProvider::get() );

    P<Button> pButton = newObj<Button>();

    SECTION("init")
    {
        SECTION("ViewCore")
            bdn::winuwp::test::testWinuwpViewCoreInitialization( pWindow, pButton);

        SECTION("ButtonCore")
        {
            SECTION("label")
                pButton->label() == "hello";

            String expectedLabel = pButton->label();
                
            pWindow->setContentView(pButton);

            P<bdn::winuwp::ButtonCore> pCore = cast<bdn::winuwp::ButtonCore>( pButton->getViewCore() );
            REQUIRE( pCore!=nullptr );

            ::Windows::UI::Xaml::Controls::Button^ pWinButton = dynamic_cast<::Windows::UI::Xaml::Controls::Button^>( pCore->getFrameworkElement() );
            REQUIRE( pWinButton!=nullptr );

            String label = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>( pWinButton->Content )->Text->Data();                
            REQUIRE( label == expectedLabel );
        }
    }

    SECTION("postInit")
    {
        pWindow->setContentView(pButton);

        SECTION("generic")
            bdn::test::testButtonCore(pWindow, pButton );        

        SECTION("winuwp")
        {
            SECTION("ViewCore")
                bdn::winuwp::test::testWinuwpViewCore(pWindow, pButton, true, true);

            SECTION("ButtonCore")
            {
                P<bdn::winuwp::ButtonCore> pCore = cast<bdn::winuwp::ButtonCore>( pButton->getViewCore() );
                REQUIRE( pCore!=nullptr );

                ::Windows::UI::Xaml::Controls::Button^ pWinButton = dynamic_cast<::Windows::UI::Xaml::Controls::Button^>( pCore->getFrameworkElement() );

                REQUIRE( pWinButton!=nullptr );

                SECTION("label")
                {
                    // setLabel should change the window test
                    pButton->label() = "hello world";

                    String text = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>( pWinButton->Content )->Text->Data();                

                    REQUIRE( text == "hello world" );
                }                
            }
        }
    }
}




