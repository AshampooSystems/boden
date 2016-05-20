#include <bdn/init.h>
#include <bdn/Frame.h>
#include <bdn/Button.h>

namespace bdn
{



void Frame::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_CLOSE)
	{
		::PostQuitMessage(0);
	}

	Window::handleMessage(context, windowHandle, message, wParam, lParam);
}


}



