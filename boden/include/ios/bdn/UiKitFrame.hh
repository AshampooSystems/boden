#ifndef BDN_UiKitFrame_HH_
#define BDN_UiKitFrame_HH_

#include <bdn/IFrame.h>

#import <bdn/UiKitWindow.hh>


namespace bdn
{

class UiKitFrame : public UiKitWindow, BDN_IMPLEMENTS IFrame
{
public:
    UiKitFrame(const String& title);
    
    Property<String>& title() override
    {
        return UiKitWindow::title();
    }
    
    
    ReadProperty<String>& title() const override
    {
        return UiKitWindow::title();        
    }


};

}


#endif
