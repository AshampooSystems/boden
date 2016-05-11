#include <bdn/init.h>
#include <bdn/Frame.h>

#import <bdn/Frame_Impl.hh>


namespace bdn
{
    
    
    
Frame::Frame(const String& title)
{
    _impl = new Impl(title);
}

void Frame::setTitle(const String& title)
{
    _impl->setTitle(title);
}
    
void Frame::show(bool visible)
{
    _impl->show(visible);
}
    


}