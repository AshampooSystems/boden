#ifndef BDN_CocoaFrame_HH_
#define BDN_CocoaFrame_HH_

#include <bdn/IFrame.h>

#import <bdn/CocoaWindow.hh>


namespace bdn
{

class CocoaFrame : public CocoaWindow, BDN_IMPLEMENTS IFrame
{
public:
    CocoaFrame(const String& title);
    
    Property<String>& title() override
    {
        return CocoaWindow::title();
    }
    
    
    ReadProperty<String>& title() const override
    {
        return CocoaWindow::title();        
    }


};

}


#endif
