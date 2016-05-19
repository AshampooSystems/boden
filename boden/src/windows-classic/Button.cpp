#include <bdn/init.h>
#include <bdn/Button.h>

namespace bdn
{
	

void Button::handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_COMMAND)
	{
		// we were clicked.
		generateClick();
	}
}


}


