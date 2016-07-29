#ifndef BDN_WIN32_ButtonCore_H_
#define BDN_WIN32_ButtonCore_H_

#include <bdn/Button.h>
#include <bdn/win32/ViewCore.h>

namespace bdn
{
namespace win32
{

class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
{
public:
	ButtonCore(Button* pOuter);

	void setLabel(const String& label) override;

	void generateClick()
	{
		ClickEvent evt( _pOuterViewWeak );

		cast<Button>(_pOuterViewWeak)->onClick().notify(evt);
	}


	Size calcPreferredSize() const;
	int calcPreferredHeightForWidth(int width) const;
	int calcPreferredWidthForHeight(int height) const;	

protected:		
	void handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;

};


}
}

#endif
