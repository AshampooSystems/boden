#include <bdn/init.h>
#include <bdn/winuwp/UiProvider.h>

#include <bdn/winuwp/WindowCore.h>
#include <bdn/winuwp/ContainerViewCore.h>
#include <bdn/winuwp/ButtonCore.h>
#include <bdn/winuwp/TextViewCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

namespace bdn
{
    
P<IUiProvider> getPlatformUiProvider()
{
    return &bdn::winuwp::UiProvider::get();
}
    
}


namespace bdn
{
namespace winuwp
{


BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );

String UiProvider::getName() const
{
    return "winuwp";
}
    
P<IViewCore> UiProvider::createViewCore(const String& coreTypeName, View* pView)
{
    if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
        return newObj<ContainerViewCore>( cast<ContainerView>(pView) );
    
    else if(coreTypeName == Button::getButtonCoreTypeName() )
        return newObj<ButtonCore>( cast<Button>(pView) );
    
    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( this, cast<Window>(pView) );

    else if(coreTypeName == TextView::getTextViewCoreTypeName() )
        return newObj<TextViewCore>( cast<TextView>(pView) );
    
	else
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}


}
}

