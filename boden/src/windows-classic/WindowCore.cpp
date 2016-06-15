#include <bdn/init.h>
#include <bdn/WindowCore.h>

namespace bdn
{


WindowCore::WindowCore(Window* pWindow)
	: ViewCore(	pWindow,
				WindowCoreClass::get()->getName(),
				pWindow->title().get(),
				WS_OVERLAPPEDWINDOW | WS_POPUPWINDOW,
				WS_EX_APPWINDOW,
				200,
				200,
				300,
				200 )
{
}

void WindowCore::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_CLOSE)
	{
		::PostQuitMessage(0);
	}

	ViewCore::handleMessage(context, windowHandle, message, wParam, lParam);
}


}



