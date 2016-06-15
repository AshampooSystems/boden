#ifndef BDN_ButtonCore_H_
#define BDN_ButtonCore_H_

#include <bdn/Button.h>
#include <bdn/ViewCore.h>

namespace bdn
{

class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
{
public:
	ButtonCore(Button* pOuter);

	void setLabel(const String& label) override
	{
		setWindowText(getHwnd(), label);
	}

	void generateClick()
	{
		ClickEvent evt( _pOuterViewWeak );

		cast<Button>(_pOuterViewWeak)->onClick().notify(evt);
	}


protected:		
	void handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;

};


}

#endif
