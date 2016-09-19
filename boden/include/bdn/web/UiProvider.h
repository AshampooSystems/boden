#ifndef BDN_WEB_UiProvider_H_
#define BDN_WEB_UiProvider_H_

#include <bdn/IUiProvider.h>

#include <emscripten/val.h>

#include <cmath>

namespace bdn
{
namespace web
{

class UiProvider : public Base, BDN_IMPLEMENTS IUiProvider
{
public:
    UiProvider()
    {
        emscripten::val docVal = emscripten::val::global("document");
        
        emscripten::val divVal( docVal.call<emscripten::val>("createElement", std::string("div") ) );

        docVal.call<emscripten::val>("getElementsByTagName", std::string("body"))[0].call<void>("appendChild", divVal);

        emscripten::val styleObj = divVal["style"];
        styleObj.set("height", "1em");

        _semPixels = divVal["offsetHeight"].template as<int>();

        docVal.call<emscripten::val>("getElementsByTagName", std::string("body"))[0].call<void>("removeChild", divVal);
    }   

    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;


    XXX
    int             uiLengthToPixels(const UiLength& uiLength) const
    {
        XXX

        if(uiLength.unit==UiLength::sem)
            return std::lround( uiLength.value * _semPixels );

        else if(uiLength.unit==UiLength::dip)
        {
            // we assume that the browser uses device independent pixels.
            // So, no need for any scaling.
            return std::lround( uiLength.value );
        }

        else if(uiLength.unit==UiLength::realPixel)
            return std::lround( uiLength.value );

        else
            throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
    }
    
    String uiLengthToHtmlString(const UiLength& length)
    {
        int pixels = uiLengthToPixels(length);

        return pixelsToHtmlString(pixels);
    }

    static String pixelsToHtmlString(int pixels)
    {
        return std::to_string(pixels)+"px";
    }

    XXX
    Margin          uiMarginToPixelMargin(const UiMargin& margin) const
    {
        return Margin(
            uiLengthToPixels(margin.top),
            uiLengthToPixels(margin.right),
            uiLengthToPixels(margin.bottom),
            uiLengthToPixels(margin.left) );
    }



    static UiProvider& get();


private:
    double _semPixels;

};

}
}


#endif
