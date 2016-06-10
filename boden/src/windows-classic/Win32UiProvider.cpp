#include <bdn/init.h>
#include <bdn/Win32UiProvider.h>

#include <bdn/Win32ButtonCore.h>
#include <bdn/Win32WindowCore.h>

namespace bdn
{

P<IViewCore> Win32UiProvider::createViewCore(const String& viewTypeName, View* pView)
{
	if(viewTypeName == Button::getButtonViewTypeName() )
		return newObj<Win32ButtonCore>( cast<Button>(pView) );

	else if(viewTypeName == Window::getWindowViewTypeName() )
		return newObj<Win32WindowCore>( cast<Window>(pView) );


}


}