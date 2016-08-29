#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>
#include <bdn/web/UiProvider.h>
#include <bdn/web/WindowCore.h>
#include "testWebViewCore.h"

using namespace bdn;

TEST_CASE("WindowCore-web")
{
    P<Window> pWindow = newObj<Window>( &bdn::web::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("web-view")
        bdn::web::test::testWebViewCore(pWindow, pWindow, false);

    SECTION("web-window")
    {
        P<bdn::web::WindowCore> pCore = cast<bdn::web::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        emscripten::val domObject = pCore->getDomObject();
        REQUIRE( !domObject.isNull() );
        REQUIRE( !domObject.isUndefined() );

        SECTION("title")
        {
            // setTitle has no effect for web windows.
        }                
    }

    SECTION("Element destroyed when object destroyed")
    {
        // there may be pending sizing info updates for the window, which keep it alive.
        // Ensure that those are done first.

        // wrap pWindow in a struct so that we can destroy all references
        // in the continuation.
        struct CaptureData : public Base
        {
            P<Window> pWindow;
        };
        P<CaptureData> pData = newObj<CaptureData>();
        pData->pWindow = pWindow;
        pWindow = nullptr;
        
        CONTINUE_SECTION_ASYNC(pData)
        {
            P<bdn::web::WindowCore> pCore = cast<bdn::web::WindowCore>( pData->pWindow->getViewCore() );
            REQUIRE( pCore!=nullptr );

            emscripten::val domObject = pCore->getDomObject();
            REQUIRE( !domObject.isNull() );
            REQUIRE( !domObject.isUndefined() );

            String elementId = pCore->getHtmlElementId();

            // sanity check
            emscripten::val doc = emscripten::val::global("document");
            {
                emscripten::val el = doc.call<emscripten::val>("getElementById", elementId.asUtf8() );
                REQUIRE( !el.isNull() );
                REQUIRE( !el.isUndefined() );
            }


            SECTION("core not kept alive")
            {
                pCore = nullptr;
            }

            SECTION("core kept alive")
            {
                // do nothing
            }        

            pData->pWindow = nullptr;

            // the element should have been destroyed            
            {
                emscripten::val el = doc.call<emscripten::val>("getElementById", elementId.asUtf8() );
                REQUIRE( el.isNull() );
            }
        };
    }
}




