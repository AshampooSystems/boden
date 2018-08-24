#ifndef BDN_WIN32_TextFieldCore_H_
#define BDN_WIN32_TextFieldCore_H_

#include <bdn/TextField.h>
#include <bdn/ITextFieldCore.h>
#include <bdn/win32/ViewCore.h>

namespace bdn
{
namespace win32
{

class TextFieldCore : public ViewCore, BDN_IMPLEMENTS ITextFieldCore
{
public:
	TextFieldCore(TextField* pOuter);

	void setText(const String& text) override;

	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override;

protected:		
	void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	void handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;
	void textChanged();
	void returnKeyPressed();

};


}
}

#endif // BDN_WIN32_TextFieldCore_H_