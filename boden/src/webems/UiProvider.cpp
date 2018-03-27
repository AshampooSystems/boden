#include <bdn/init.h>
#include <bdn/webems/UiProvider.h>


#include <bdn/ViewCoreTypeNotSupportedError.h>

#include <bdn/webems/ContainerViewCore.h>
#include <bdn/webems/ButtonCore.h>
#include <bdn/webems/TextViewCore.h>
#include <bdn/webems/WindowCore.h>
#include <bdn/webems/ScrollViewCore.h>


namespace bdn
{
    
P<IUiProvider> getDefaultUiProvider()
{
    return &bdn::webems::UiProvider::get();
}
    
}


namespace bdn
{
namespace webems
{

BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );

String UiProvider::getName() const
{
    return "webems";
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
    
    else if(coreTypeName == ScrollView::getScrollViewCoreTypeName() )
        return newObj<ScrollViewCore>( cast<ScrollView>(pView) );
    
    else
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}

P<ITextUi> UiProvider::getTextUi()
{
    {
        Mutex::Lock lock( _textUiInitMutex );
        if(_pTextUi==nullptr)
            _pTextUi = newObj< ViewTextUi >();
    }

    return _pTextUi;
}


}
}

