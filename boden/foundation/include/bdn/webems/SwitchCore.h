#ifndef BDN_WEBEMS_SwitchCore_H_
#define BDN_WEBEMS_SwitchCore_H_

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>

#include <bdn/webems/ViewCore.h>
#include <bdn/ISwitchCore.h>
#include <bdn/Switch.h>
#include <bdn/Toggle.h>

namespace bdn
{
namespace webems
{

template <class T>
class SwitchCore : public ViewCore, BDN_IMPLEMENTS ISwitchCore
{
public:
    SwitchCore( T* pOuter )
    : ViewCore( pOuter,
                "div"  ),
    _switchDomObject(emscripten::val::null()),
    _switchHandleDomObject(emscripten::val::null()),
    _labelDomObject(emscripten::val::null())
    {
        createSwitchCompositeInner(pOuter);

        setLabel(pOuter->label());
        setOn(pOuter->on());

        // Set CSS class on container
        _domObject["classList"].template call<void>("add", emscripten::val("bdn_switch_container"));

        // Set click callback on switch (label is not clickable)
        emscripten_set_click_callback( _switchDomObject["id"].template as<std::string>().c_str(), this, false, _clickedCallback);
    }
   
    void setLabel(const String& label) override
    {
        _labelDomObject.set("textContent", label.asUtf8());
    }

    void setOn(const bool& on) override
    {
        if (on != _on) {
            _on = on;

            if (on) {
                _switchDomObject.call<void>("setAttribute", emscripten::val("data-on"), emscripten::val("on"));
                _switchDomObject["classList"].template call<void>("add", emscripten::val("bdn_on"));
                _switchDomObject["classList"].template call<void>("remove", emscripten::val("bdn_off"));
            } else {
                _switchDomObject.call<void>("removeAttribute", emscripten::val("data-on"));
                _switchDomObject["classList"].template call<void>("add", emscripten::val("bdn_off"));
                _switchDomObject["classList"].template call<void>("remove", emscripten::val("bdn_on"));
            }
        }
    }
    
    emscripten::val& _getLabelDomObject()
    {
        return _labelDomObject;
    }

    emscripten::val& _getSwitchDomObject()
    {
        return _switchDomObject;
    }

    
protected:
    bool _clicked(int eventType, const EmscriptenMouseEvent* pMouseEvent)
    {
        if(eventType==EMSCRIPTEN_EVENT_CLICK)
        {
            P<View> pView = getOuterViewIfStillAttached();
            if(pView!=nullptr)
            {
                ClickEvent evt( pView );

                setOn(!_on);

                // Set outer on state on click (see comment in constructor)
                cast<T>(pView)->setOn( _on );

                // We guarantee that the on property will be set on the outer control before a
                // notification is posted to onClick.
                cast<T>(pView)->onClick().notify(evt);
            }
        }
        
        return false;
    }
    
    static EM_BOOL _clickedCallback(int eventType, const EmscriptenMouseEvent* pMouseEvent, void* pUserData)
    {
        return ((SwitchCore*)pUserData)->_clicked(eventType, pMouseEvent);
    }

    void createSwitchCompositeInner(Switch *pOuter)
    {
        _labelDomObject = createElement("div");
        _switchDomObject = createElement("div"); 
        _switchHandleDomObject = createElement("div");

        // Add label and switch to container
        _addChildElement(_labelDomObject);
        _addChildElement(_switchDomObject); 

        // Add handle to switch
        _switchDomObject.call<void>("appendChild", _switchHandleDomObject);

        // Set appropriate CSS classes
        _switchDomObject["classList"].template call<void>("add", emscripten::val("bdn_switch"));
        _labelDomObject["classList"].template call<void>("add", emscripten::val("bdn_switch_label"));
        _switchHandleDomObject["classList"].template call<void>("add", emscripten::val("bdn_switch_handle"));

        // Label shouldn't wrap (for now)
        emscripten::val styleObj = _labelDomObject["style"];
        styleObj.set("white-space", "nowrap");
    }

    // Helper to create elements
    emscripten::val createElement(String elementName)
    {
        emscripten::val docVal = emscripten::val::global("document");
        emscripten::val elementVal = docVal.call<emscripten::val>("createElement", elementName.asUtf8());
        elementVal.set("id", _nextElementId().asUtf8());
        return elementVal;
    }

    bool _on = false;

    emscripten::val _switchDomObject;
    emscripten::val _switchHandleDomObject;
    emscripten::val _labelDomObject;
};


}    
}


#endif


