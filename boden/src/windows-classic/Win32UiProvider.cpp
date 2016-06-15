#include <bdn/init.h>
#include <bdn/Win32UiProvider.h>

#include <bdn/ButtonCore.h>
#include <bdn/WindowCore.h>
#include <bdn/ViewTypeNotSupportedError.h>

namespace bdn
{

P<IUiProvider> getPlatformUiProvider()
{
	static SafeInit<Win32UiProvider> init;

	return init.get();
}

P<IViewCore> Win32UiProvider::createViewCore(const String& viewTypeName, View* pView)
{
	if(viewTypeName == Button::getButtonViewTypeName() )
		return newObj<ButtonCore>( cast<Button>(pView) );

	else if(viewTypeName == Window::getWindowViewTypeName() )
		return newObj<WindowCore>( cast<Window>(pView) );

	throw ViewTypeNotSupportedError(viewTypeName);
}


}