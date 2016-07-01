#include <bdn/init.h>
#include <bdn/gtk/UiProvider.h>

#include <bdn/gtk/WindowCore.h>
#include <bdn/gtk/ContainerViewCore.h>
#include <bdn/gtk/ButtonCore.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>

namespace bdn
{
    
P<IUiProvider> getPlatformUiProvider()
{
    return &bdn::gtk::UiProvider::get();
}
    
}


namespace bdn
{
namespace gtk
{


BDN_SAFE_STATIC_IMPL( UiProvider, UiProvider::get );

String UiProvider::getName() const
{
    return "gtk";
}
    
P<IViewCore> UiProvider::createViewCore(const String& coreTypeName, View* pView)
{
    if(coreTypeName == ContainerView::getContainerViewCoreTypeName() )
        return newObj<ContainerViewCore>( cast<ContainerView>(pView) );
    
    else if(coreTypeName == Button::getButtonCoreTypeName() )
        return newObj<ButtonCore>( cast<Button>(pView) );
    
    else if(coreTypeName == Window::getWindowCoreTypeName() )
        return newObj<WindowCore>( cast<Window>(pView) );
    
    else
        throw ViewCoreTypeNotSupportedError(coreTypeName);
}


}
}

