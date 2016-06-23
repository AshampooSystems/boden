#ifndef BDN_Frame_HH_
#define BDN_Frame_HH_

#include <bdn/IFrame.h>

#import <bdn/Window.hh>


namespace bdn
{

class Frame : public Window, BDN_IMPLEMENTS IFrame
{
public:
    Frame(const String& title);
    
    Property<String>& title() override
    {
        return Window::title();
    }
    
    
    ReadProperty<String>& title() const override
    {
        return Window::title();        
    }


};

}


#endif
