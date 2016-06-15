#include <bdn/init.h>
#include <bdn/ButtonCore.h>

namespace bdn
{

ButtonCore::ButtonCore(Button* pOuter)
	: ViewCore(	pOuter, 
				"BUTTON",
				pOuter->label().get(),
				BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
				0 )
{
}

void ButtonCore::handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_COMMAND)
	{
		// we were clicked.
		generateClick();
	}

	return ViewCore::handleParentMessage(context, windowHandle, message, wParam, lParam);
}


}


