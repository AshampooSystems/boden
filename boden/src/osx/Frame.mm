#include <bdn/Frame.h>

#import <bdn/Frame_Impl.hh>


namespace bdn
{
    
    
    
    Frame::Frame()
    {
        _impl = new Impl;
    }
        
    void Frame::show(bool visible)
    {
        _impl->show(visible);
    }
    


}