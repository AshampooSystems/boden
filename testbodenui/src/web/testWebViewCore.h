#ifndef BDN_WEB_TEST_testWebViewCore_H_
#define BDN_WEB_TEST_testWebViewCore_H_


#include <bdn/test/testViewCore.h>

#include <bdn/View.h>
#include <bdn/web/ViewCore.h>

namespace bdn
{
namespace web
{
namespace test
{

/** Tests the web view core that is associated with the specified view.

    \param canCalculatePreferredSize indicates whether the view core supports calculating a preferred size.
        Some view cores (e.g. ContainerViewCore) do not support that and instead require the outer view
        object to provide the preferred size.
        */
inline void testWebViewCore(P<Window> pWindow, P<View> pView, bool canCalculatePreferredSize)
{
    SECTION("generic")
        bdn::test::testViewCore(pWindow, pView, canCalculatePreferredSize);

    SECTION("web-specific")
    {
        P<bdn::web::ViewCore> pCore = cast<bdn::web::ViewCore>( pView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        emscripten::val domObject = pCore->getDomObject();
        REQUIRE( !domObject.isNull() );
        REQUIRE( !domObject.isUndefined() );

        SECTION("visible")
        {
            String vis = domObject["style"]["visibility"].template as<std::string>();
            REQUIRE( vis == String(pView->visible().get() ? "visible" : "hidden") );

            
            pView->visible() = true;
            vis = domObject["style"]["visibility"].template as<std::string>();
            REQUIRE( vis == "visible" );

            pView->visible() = false;
            vis = domObject["style"]["visibility"].template as<std::string>();
            REQUIRE( vis == "hidden" );

            pView->visible() = true;
            vis = domObject["style"]["visibility"].template as<std::string>();
            REQUIRE( vis == "visible" );
        }

        
        SECTION("padding")
        {
            pView->padding() = UiMargin(UiLength::realPixel, 11, 22, 33, 44);

            emscripten::val pad = domObject["padding"];
            REQUIRE( !pad.isNull() );
            REQUIRE( !pad.isUndefined() );

            REQUIRE( pad["top"].as<std::string>()=="11px" );
            REQUIRE( pad["right"].as<std::string>()=="22px" );
            REQUIRE( pad["bottom"].as<std::string>()=="33px" );
            REQUIRE( pad["left"].as<std::string>()=="44px" );
        }

        SECTION("bounds")
        {
            // bounds should translate to style setting
            pView->bounds() = Rect(110, 220, 880, 990);

            emscripten::val style = domObject["padding"];
            int width = style["width"].as<int>();
            int height = style["height"].as<int>();
            int x = style["left"].as<int>();
            int y = style["top"].as<int>();
            
            REQUIRE( x == 110 );
            REQUIRE( y == 220 );
            REQUIRE( width == 880 );
            REQUIRE( height == 990 );
        }

    }
}


}
}
}


#endif

