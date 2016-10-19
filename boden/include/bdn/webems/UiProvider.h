#ifndef BDN_WEBEMS_UiProvider_H_
#define BDN_WEBEMS_UiProvider_H_

#include <bdn/IUiProvider.h>

#include <emscripten/val.h>

#include <cmath>

#include <sstream>
#include <iomanip>

namespace bdn
{
namespace webems
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
    

    static UiProvider& get();


    double getSemSizeDips() const
    {
        return _semDips;
    }


private:
    double _semDips;

};

}
}


#endif
