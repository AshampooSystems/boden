#ifndef _BDN_BUTTON_H_
#define _BDN_BUTTON_H_

#include <emscripten/html5.h>

#include <bdn/Window.h>
#include <bdn/EventSource.h>
#include <bdn/ClickEvent.h>

#include <list>

namespace bdn
{


class Button : public Window
{
public:
    Button( Window* pParent, const std::string& label)
    : Window(pParent, "button")
    {
        _pClickEventSource = new EventSource<ClickEvent>;
        
        setLabel(label);
        
        // visible by default
        show();
    }
    
    void setLabel(const std::string& label)
    {
        _pJsObj->set("textContent", label);
    }
    
    EventSource<ClickEvent>* getClickEventSource()
    {
        connectClick();
        
        return _pClickEventSource;
    }
    
    
protected:
    
    bool clickHandler(int eventType, const EmscriptenMouseEvent* pMouseEvent)
    {
        if(eventType==EMSCRIPTEN_EVENT_CLICK)
        {
            ClickEvent evt(this);
            
            _pClickEventSource->deliver(evt);
        }
        
        return false;
    }
    
    
    static EM_BOOL clickCallback(int eventType, const EmscriptenMouseEvent* pMouseEvent, void* pUserData)
    {
        return ((Button*)pUserData)->clickHandler(eventType, pMouseEvent);
    }
    
    void connectClick()
    {
        if(!_clickConnected)
        {
            emscripten_set_click_callback( _jsId.c_str(), this, false, clickCallback);
            _clickConnected = true;
        }
    }
    
    EventSource<ClickEvent>*    _pClickEventSource;
    
    bool                        _clickConnected=false;
    
};
    
}


#endif


