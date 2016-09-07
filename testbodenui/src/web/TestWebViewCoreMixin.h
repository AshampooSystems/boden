#ifndef BDN_TEST_TestWebViewCoreMixin_H_
#define BDN_TEST_TestWebViewCoreMixin_H_


#include <bdn/View.h>
#include <bdn/web/ViewCore.h>
#include <bdn/web/UiProvider.h>

namespace bdn
{
namespace test
{

    
/** A mixin class that adds implementations of web view specific functionality on top of
    the base class specified in the template parameter BaseClass.*/
template<class BaseClass>
class TestWebViewCoreMixin : public BaseClass
{
public:
    TestWebViewCoreMixin()
    : _domObject( emscripten::val::null() )
    {        
    }

protected:

    void initCore() override
    {
        BaseClass::initCore();

        _pWebCore = cast<bdn::web::ViewCore>( BaseClass::_pView->getViewCore() );
        REQUIRE( _pWebCore!=nullptr );

        _domObject = _pWebCore->getDomObject();
        REQUIRE( !_domObject.isNull() );
    }

    IUiProvider& getUiProvider() override
    {
        return bdn::web::UiProvider::get();
    }

    void verifyCoreVisibility() override
    {
        bool expectedVisible = BaseClass::_pView->visible();

        String vis = _domObject["style"]["visibility"].template as<std::string>();
        REQUIRE( vis == String(expectedVisible ? "visible" : "hidden") );
    }

    Rect getViewRect()
    {
        // bounds should translate to style setting

        emscripten::val style = _domObject["padding"];
        int width = style["width"].as<int>();
        int height = style["height"].as<int>();
        int x = style["left"].as<int>();
        int y = style["top"].as<int>();

        return Rect(x, y, width, height);
    }

    void verifyInitialDummyCoreBounds() override
    {        
        Rect rect = getViewRect();

        REQUIRE( rect == Rect() );
    }

    void verifyCoreBounds() override
    {        
        Rect rect = getViewRect();
        Rect expectedRect = BaseClass::_pView->bounds();

        REQUIRE( rect == expectedRect );
    }


    void verifyCorePadding() override
    {
        emscripten::val pad = _domObject["padding"];
        
        Nullable<UiMargin> expectedPad = BaseClass::_pView->padding();
        if(expectedPad.isNull())
        {
            // padding should be left at default.
            REQUIRE( ( pad.isNull() || pad.isUndefined() ) );
        }
        else
        {
            REQUIRE( !pad.isNull() );
            REQUIRE( !pad.isUndefined() );

            Margin expectedPixelPadding = BaseClass::_pView->uiMarginToPixelMargin( expectedPad );

            REQUIRE( pad["top"].as<std::string>() == std::to_string(expectedPixelPadding.top)+"px" );
            REQUIRE( pad["right"].as<std::string>() == std::to_string(expectedPixelPadding.right)+"px" );
            REQUIRE( pad["bottom"].as<std::string>() == std::to_string(expectedPixelPadding.bottom)+"px" );
            REQUIRE( pad["left"].as<std::string>() == std::to_string(expectedPixelPadding.left)+"px" );
        }
    }


    P<bdn::web::ViewCore> _pWebCore;
    emscripten::val       _domObject;
};



}
}



#endif

