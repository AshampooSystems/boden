#ifndef _BDN_BUTTON_H_
#define _BDN_BUTTON_H_

#include <bdn/IWindow.h>
#include <bdn/Notifier.h>
#include <bdn/ClickEvent.h>
#include <bdn/Property.h>

#include <string>
#include <functional>

namespace bdn
{

class Button : public Base, virtual public IWindow
{
public:
    Button(IWindow* pParent, const String& label);
    
    Property<String>& label();
    
    void setLabel(const std::string& label);
    

    class Impl;
    Impl* getImpl()
    {
        return _pImpl;
    }
    
    
    Notifier<const ClickEvent&>& onClick();
        
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


