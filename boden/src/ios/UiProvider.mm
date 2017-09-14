#include <bdn/init.h>

#import <UIKit/UIKit.h>
#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/WindowCore.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/ButtonCore.hh>
#import <bdn/ios/TextViewCore.hh>
#import <bdn/ios/ScrollViewCore.hh>

#include <bdn/ViewCoreTypeNotSupportedError.h>


namespace bdn
{
    
P<IUiProvider> getDefaultUiProvider()
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
    // iOS uses DIPs for font sizes (although they call it "points").
    // So no conversion necessary
    
    _semDips = UIFont.systemFontSize;
    
    _pLayoutCoordinator = newObj<LayoutCoordinator>();
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
    
    else if(coreTypeName == ScrollView::getScrollViewCoreTypeName() )
        return newObj<ScrollViewCore>( cast<ScrollView>(pView) );
    
    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( cast<Window>(pView) );
    
    else
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}


P<ITextUi> UiProvider::getTextUi()
{
    {
        MutexLock lock( _textUiInitMutex );
        if(_pTextUi==nullptr)
            _pTextUi = newObj< ViewTextUi >();
    }
    
    return _pTextUi;
}



}
}

