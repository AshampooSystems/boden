#include <bdn/init.h>
#include <bdn/Button.h>

namespace bdn
{


P<IButton> createButton(IView* pParent, const String& label)
{
	return newObj<Button>(cast<Window>(pParent), label);
}
	

void Button::handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_COMMAND)
	{
		// we were clicked.
		generateClick();
	}
}


}


