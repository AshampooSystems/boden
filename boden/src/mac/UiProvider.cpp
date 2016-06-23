#include <bdn/init.h>
#include <bdn/mac/UiProvider.h>


#include <bdn/ViewCoreTypeNotSupportedError.h>

namespace bdn
{
    
P<IUiProvider> getPlatformUiProvider()
{
    return bdn::mac::UiProvider::get();
}
    
}


namespace bdn
{
namespace mac
{

String UiProvider::getName() const
{
    return "mac";
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

