#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextView.h>
#include <bdn/Window.h>
#include <bdn/test/testTextViewCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/TextViewCore.h>
#include "testWinuwpViewCore.h"

using namespace bdn;

TEST_CASE("TextViewCore-winuwp")
{
    P<Window> pWindow = newObj<Window>( &bdn::winuwp::UiProvider::get() );

    P<TextView> pTextView = newObj<TextView>();

    SECTION("init")
    {
        SECTION("ViewCore")
            bdn::winuwp::test::testWinuwpViewCoreInitialization( pWindow, pTextView);

        SECTION("TextViewCore")
        {
            SECTION("text")
                pTextView->text() = "hello";

            String expectedText = pTextView->text();
                
            pWindow->setContentView(pTextView);

            P<bdn::winuwp::TextViewCore> pCore = cast<bdn::winuwp::TextViewCore>( pTextView->getViewCore() );
            REQUIRE( pCore!=nullptr );

            ::Windows::UI::Xaml::Controls::TextBlock^ pTextBlock = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>( pCore->getFrameworkElement() );
            REQUIRE( pTextBlock!=nullptr );

            String text = pTextBlock->Text->Data();

            REQUIRE( text == expectedText );
        }
    }

    SECTION("postInit")
    {
        pWindow->setContentView(pTextView);

        SECTION("generic")
            bdn::test::testTextViewCore(pWindow, pTextView );        

        SECTION("winuwp")
        {
            SECTION("ViewCore")
                bdn::winuwp::test::testWinuwpViewCore(pWindow, pTextView, true, true);

            SECTION("TextViewCore")
            {
                P<bdn::winuwp::TextViewCore> pCore = cast<bdn::winuwp::TextViewCore>( pTextView->getViewCore() );
                REQUIRE( pCore!=nullptr );

                ::Windows::UI::Xaml::Controls::TextBlock^ pTextBlock = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>( pCore->getFrameworkElement() );
                REQUIRE( pTextBlock!=nullptr );

                SECTION("text")
                {
                    pTextView->text() = "hello world";

                    String text = pTextBlock->Text->Data();

                    REQUIRE( text == "hello world" );
                }                
            }
        }
    }
}




