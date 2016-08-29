#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>
#include <bdn/web/UiProvider.h>
#include <bdn/web/ButtonCore.h>
#include "testWebViewCore.h"

using namespace bdn;

TEST_CASE("ButtonCore-web")
{
    P<Window> pWindow = newObj<Window>( &bdn::web::UiProvider::get() );

    P<Button> pButton = newObj<Button>();

    pWindow->setContentView(pButton);

    SECTION("generic")
        bdn::test::testButtonCore(pWindow, pButton );        

    SECTION("web-view")
        bdn::web::test::testWebViewCore(pWindow, pButton, true);

    SECTION("web-button")
    {
        P<bdn::web::ButtonCore> pCore = cast<bdn::web::ButtonCore>( pButton->getViewCore() );
        REQUIRE( pCore!=nullptr );

        emscripten::val domObject = pCore->getDomObject();
        REQUIRE( !domObject.isNull() );
        REQUIRE( !domObject.isUndefined() );

        SECTION("label")
        {
            pButton->label() = "hello world";

            // should update the element text content
            String text = domObject["textContent"].as<std::string>();

            REQUIRE( text == "hello world" );
        }                
    }
}




