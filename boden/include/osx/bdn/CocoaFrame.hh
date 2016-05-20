#ifndef BDN_CocoaFrame_HH_
#define BDN_CocoaFrame_HH_

#import <bdn/CocoaWindow.hh>

namespace bdn
{

class CocoaFrame : public CocoaWindow
{
public:
    CocoaFrame(const String& title);
};

}


#endif
