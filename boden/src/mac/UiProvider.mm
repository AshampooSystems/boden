#include <bdn/init.h>

#import <bdn/mac/UiProvider.hh>

#import <Cocoa/Cocoa.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

#import <bdn/mac/WindowCore.hh>
#import <bdn/mac/ButtonCore.hh>
#import <bdn/mac/TextViewCore.hh>
#import <bdn/mac/ContainerViewCore.hh>
#import <bdn/mac/ScrollViewCore.hh>

namespace bdn
{
    
P<IUiProvider> getDefaultUiProvider()
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
    // mac uses DIPs natively. So no conversion necessary
    _semDips = NSFont.systemFontSize;
    
    _pLayoutCoordinator = newObj<LayoutCoordinator>();
}

String UiProvider::getName() const
{
    return "mac";
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


}
}

