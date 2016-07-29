#ifndef BDN_WEB_SwitchCore_H_
#define BDN_WEB_SwitchCore_H_

#include <emscripten/html5.h>
/*
#include <bdn/ViewCore.h>

#include <list>

namespace bdn
{
namespace web
{

class SwitchCore : public ViewCore
{
public:
    SwitchCore( Window* pParent, const std::string& label)
    : Window(pParent, "label" )
    {
        // checkbox controls in HTML do not display a text by default.
        // One can get this effect by embedding the checkbox in a label control.
        
        setLabel(label);
        
        emscripten::val docVal = emscripten::val::global("document");
        
        _pJsInputObj = new emscripten::val( docVal.call<emscripten::val>("createElement", std::string("input")) );
        
        _pJsInputObj->set("type", "checkbox");
        
        _pJsObj->call<void>("insertBefore",  *_pJsInputObj, (*_pJsObj)["firstChild"] );
        
        // visible by default
        show();
    }
    
    void setLabel(const std::string& label)
    {
        _pJsObj->set("textContent", label);
    }
    
protected:
    emscripten::val*    _pJsInputObj;
    
};
    

}
}

*/
#endif


