#include <bdn/init.h>
#include <bdn/Frame.h>

namespace bdn
{

P<IFrame> createFrame(const String& title)
{
	return newObj<Frame>(title);
}

void Frame::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_CLOSE)
	{
		::PostQuitMessage(0);
	}

	Window::handleMessage(context, windowHandle, message, wParam, lParam);
}


}



