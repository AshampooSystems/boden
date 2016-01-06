#ifndef _BDN_BUTTON_H_
#define _BDN_BUTTON_H_

#include <bdn/IWindow.h>
#include <bdn/EventSource.h>
#include <bdn/ClickEvent.h>

#include <string>

namespace bdn
{

class Button : public Base, virtual public IWindow
{
public:
    Button(IWindow* pParent, const std::string& label);
    
    void setLabel(const std::string& label);
    

    class Impl;
    Impl* getImpl()
    {
        return _pImpl;
    }
    
    EventSource<ClickEvent>* getClickEventSource();
    
    virtual void show(bool visible=true) override;
    
    virtual void hide() override
    {
        show(false);
    }

    
protected:
    Impl* _pImpl;
};

}


#endif


