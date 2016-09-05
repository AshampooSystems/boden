#include <bdn/init.h>

#import <bdn/mac/UiProvider.hh>

#import <Cocoa/Cocoa.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

#import <bdn/mac/WindowCore.hh>
#import <bdn/mac/ButtonCore.hh>
#import <bdn/mac/TextViewCore.hh>
#import <bdn/mac/ContainerViewCore.hh>

namespace bdn
{
    
P<IUiProvider> getPlatformUiProvider()
{
    return &bdn::mac::UiProvider::get();
}
    
}


namespace bdn
{
namespace mac
{

BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );


UiProvider::UiProvider()
{
    // this is a bit of a hack. We use the height of the letter M as the em size.
    // Is there a better way?
    
    NSAttributedString* attrString = [NSAttributedString alloc];
    
    attrString = [attrString initWithString:@"M"];
    
    NSSize fontSize = attrString.size;
    
    _semPixels = fontSize.height;
    
}

String UiProvider::getName() const
{
    return "mac";
}
    
P<IViewCore> UiProvider::createViewCore(const String& coreTypeName, View* pView)
{
    if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
        return newObj<ContainerViewCore>( cast<ContainerView>(pView) );
    
    else if(coreTypeName == Button::getButtonCoreTypeName() )
        return newObj<ButtonCore>( cast<Button>(pView) );
    
    else if(coreTypeName == TextView::getTextViewCoreTypeName() )
        return newObj<TextViewCore>( cast<TextView>(pView) );
    
    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( cast<Window>(pView) );
    
    else
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}


}
}

