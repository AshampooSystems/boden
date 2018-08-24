#ifndef BDN_WEBEMS_UiProvider_H_
#define BDN_WEBEMS_UiProvider_H_

#include <bdn/IUiProvider.h>
#include <bdn/LayoutCoordinator.h>
#include <bdn/ITextUi.h>

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
        _pLayoutCoordinator = newObj<LayoutCoordinator>();

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

    P<ITextUi> getTextUi() override;
    

    static UiProvider& get();


    double getSemSizeDips() const
    {
        return _semDips;
    }


    /** Returns the layout coordinator that is used by view cores created by this UI provider.*/
    P<LayoutCoordinator> getLayoutCoordinator()
    {
        return _pLayoutCoordinator;
    }



private:
    double _semDips;
    P<LayoutCoordinator> _pLayoutCoordinator;

    Mutex                _textUiInitMutex;
    P<ITextUi>           _pTextUi;

};

}
}


#endif
