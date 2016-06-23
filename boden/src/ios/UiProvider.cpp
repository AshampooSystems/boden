#include <bdn/init.h>
#include <bdn/ios/UiProvider.h>


#include <bdn/ViewCoreTypeNotSupportedError.h>

namespace bdn
{
    
P<IUiProvider> getPlatformUiProvider()
{
    return bdn::ios::UiProvider::get();
}
    
}


namespace bdn
{
namespace ios
{

String UiProvider::getName() const
{
    return "ios";
}
    
P<IViewCore> UiProvider::createViewCore(const String& coreTypeName, View* pView)
{/*
    if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
        return newObj<ContainerViewCore>( cast<ContainerView>(pView) );
    
    else if(coreTypeName == Button::getButtonCoreTypeName() )
        return newObj<ButtonCore>( cast<Button>(pView) );
    
    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( cast<Window>(pView) );
    
    else	*/
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}


}
}

