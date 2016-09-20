#include <bdn/init.h>

#import <UIKit/UIKit.h>
#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/WindowCore.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/ButtonCore.hh>
#import <bdn/ios/TextViewCore.hh>

#include <bdn/ViewCoreTypeNotSupportedError.h>


namespace bdn
{
    
P<IUiProvider> getPlatformUiProvider()
{
    return &bdn::ios::UiProvider::get();
}
    
}


namespace bdn
{
namespace ios
{


BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );


UiProvider::UiProvider()
{
    // this is a bit of a hack. We use the height of the letter M as the em size.
    // Is there a better way?
    
    NSAttributedString* attrString = [NSAttributedString alloc];
    
    attrString = [attrString initWithString:@"M"];
    
    CGSize fontSize = attrString.size;
    
    // iOS also uses DIPs, so no conversion necessary
    
    _semDips = fontSize.height;

}

String UiProvider::getName() const
{
    return "ios";
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

