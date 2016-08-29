#ifndef BDN_WEB_ButtonCore_H_
#define BDN_WEB_ButtonCore_H_

#include <emscripten/html5.h>

#include <bdn/web/ViewCore.h>
#include <bdn/IButtonCore.h>
#include <bdn/Button.h>


namespace bdn
{
namespace web
{


class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
{
public:
    ButtonCore( Button* pOuterButton )
    : ViewCore( pOuterButton, "button" )
    {
        setLabel( pOuterButton->label() );        

        // we do not want automatic wrapping for a simply button
        _domObject["style"].set("white-space", "nowrap");

        emscripten_set_click_callback( _elementId.asUtf8Ptr(), this, false, _clickedCallback);
    }
    
    void setLabel(const String& label) override
    {
        _domObject.set("textContent", label.asUtf8());
    }
    
    
protected:    
    bool _clicked(int eventType, const EmscriptenMouseEvent* pMouseEvent)
    {
        if(eventType==EMSCRIPTEN_EVENT_CLICK)
        {
            ClickEvent evt( getOuterView() );
            
            cast<Button>(getOuterView())->onClick().notify(evt);
        }
        
        return false;
    }
    
    
    static EM_BOOL _clickedCallback(int eventType, const EmscriptenMouseEvent* pMouseEvent, void* pUserData)
    {
        return ((ButtonCore*)pUserData)->_clicked(eventType, pMouseEvent);
    }
};


}    
}


#endif


