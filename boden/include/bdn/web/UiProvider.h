#ifndef BDN_WEB_UiProvider_H_
#define BDN_WEB_UiProvider_H_

#include <bdn/IUiProvider.h>

#include <emscripten/val.h>

#include <cmath>

#include <sstream>
#include <iomanip>

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

        // the browser also works with DIPs. So no conversion necessary

        _semDips = divVal["offsetHeight"].template as<double>();

        docVal.call<emscripten::val>("getElementsByTagName", std::string("body"))[0].call<void>("removeChild", divVal);
    }   

    
    String getName() const override;
    
    P<IViewCore> createViewCore(const String& coreTypeName, View* pView) override;


    double uiLengthToDips(const UiLength& uiLength) const
    {
        if(uiLength.unit==UiLength::sem)
            return uiLength.value * _semDips;

        else if(uiLength.unit==UiLength::dip)
        {
            // we assume that the browser uses device independent pixels.
            // So, no need for any scaling.
            return uiLength.value;
        }

        else
            throw InvalidArgumentError("Invalid UiLength unit passed to UiProvider::uiLengthToPixels: "+std::to_string((int)uiLength.unit) );
    }
    
    String uiLengthToHtmlString(const UiLength& length)
    {
        double dips = uiLengthToDips(length);

        return dipsToHtmlString(dips, Round::nearest);
    }

    enum Round
    {
        down,
        nearest,
        up
    };

    static String dipsToHtmlString(double dips, Round round)
    {
    	std::stringstream s;

        double roundedDips = dips;
        if(round==Round::down)
            roundedDips = std::floor(roundedDips * 10000)/10000;
        else if(round==Round::up)
            roundedDips = std::ceil(roundedDips * 10000)/10000;

    	// we need english string formatting.
    	s.imbue( std::locale::classic() );
    	s << std::fixed << std::setprecision(4) << roundedDips << "px";

    	return s.str();
    }

    Margin          uiMarginToDipMargin(const UiMargin& margin) const
    {
        return Margin(
            uiLengthToDips(margin.top),
            uiLengthToDips(margin.right),
            uiLengthToDips(margin.bottom),
            uiLengthToDips(margin.left) );
    }



    static UiProvider& get();


private:
    double _semDips;

};

}
}


#endif
