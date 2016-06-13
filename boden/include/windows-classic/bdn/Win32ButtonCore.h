#ifndef BDN_ButtonCore_H_
#define BDN_ButtonCore_H_

#include <bdn/ViewCore.h>

#include <bdn/IButtonCore.h>

namespace bdn
{

class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
{
public:
	ButtonCore(Button* pOuter)
	{
		create(pParent, "BUTTON", pOuter->label().get(), BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 0, 20, 20, 200, 30 );
	}

	void setLabel(const String& label) override
	{
		setWindowText(getHwnd(), label);
	}

	void generateClick()
	{
		ClickEvent evt(this);

		_onClick.notify(evt);
	}


protected:		
	void handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;

	Notifier<const ClickEvent&> _onClick;
};


}

#endif
