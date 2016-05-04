#include <bdn/Frame.h>

#import <bdn/Frame_Impl.hh>


namespace bdn
{
    
    
Frame::Frame(const String& title)
{
    _pImpl = new Impl(title);
}
    
void Frame::show(bool visible)
{
    _pImpl->show(visible);
}
    
void Frame::setTitle(const String& title)
{
    _pImpl->setTitle(title);
}


}