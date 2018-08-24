#define NOMINMAX // prevent definition of min()/max() macros in windows.h to avoid clashes with std::numeric_limits
#include <bdn/init.h>
#include <bdn/win32/TextFieldCore.h>
#include <bdn/win32/WindowDeviceContext.h>

#include <limits>

#define BDN_WIN32_EDIT_MAX_LENGTH std::numeric_limits<unsigned short>::max()

namespace bdn
{
namespace win32
{

TextFieldCore::TextFieldCore(TextField* pOuter)
          : ViewCore(pOuter, 
                     "EDIT",
                     pOuter->text(),
                     WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                     0)
{
    setText(pOuter->text());
}

void TextFieldCore::setText(const String& text)
{
	if (text != getWindowText(getHwnd())) {
		P<bdn::TextField> outer = cast<bdn::TextField>(getOuterViewIfStillAttached());
        if (outer) {
    		String outerText = outer->text();
    		SetWindowText(getHwnd(), outerText.asWidePtr());
        }
	}
}

Size TextFieldCore::calcPreferredSize(const Size& availableSpace) const
{
    Size prefSize;
	String text;

    P<TextField> outer = cast<TextField>(getOuterViewIfStillAttached());
    if (outer) {
        text = outer->text();
    }

    {
        WindowDeviceContext dc(getHwnd());

        P<const win32::Font> pFont = getFont();
        if(pFont!=nullptr)
            dc.setFont(*pFont);

        prefSize = Size(0, dc.getTextSize(text).height);
    }

    UiMargin uiPadding;
    Nullable<UiMargin> pad;
    if(outer != nullptr)
        pad = outer->padding();

    if(pad.isNull())
    {
        // we should use the "default" padding. On win32 there is no real system-defined
        // default, so we choose one that looks good for most text fields:
        uiPadding = UiMargin( UiLength::sem(0.2),
                              UiLength::sem(0.) );
    }
    else
        uiPadding = pad;
    
    prefSize += uiMarginToDipMargin(uiPadding);

    prefSize.width += ::GetSystemMetrics(SM_CXEDGE);
    prefSize.height += ::GetSystemMetrics(SM_CYEDGE);

    if (outer != nullptr)
    {
        prefSize.applyMaximum(outer->preferredSizeMaximum());
        prefSize.applyMinimum(outer->preferredSizeMinimum());
    }
    
    return prefSize;
}

void TextFieldCore::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			returnKeyPressed();
		}
		break;
	case WM_CHAR:
		if (wParam == VK_RETURN) {
			context.setResult(0, false);
		}
		break;
    }
 
    return ViewCore::handleMessage(context, windowHandle, message, wParam, lParam);
}

void TextFieldCore::handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_COMMAND:        
        if (HIWORD(wParam) == EN_CHANGE) {
            textChanged();
        }
        break;
    }

    return ViewCore::handleParentMessage(context, windowHandle, message, wParam, lParam);
}

void TextFieldCore::textChanged()
{
    P<bdn::TextField> outer = cast<bdn::TextField>(getOuterViewIfStillAttached());
    if (outer) {
        String text = getWindowText( getHwnd() );
        outer->setText( text );
    }
}

void TextFieldCore::returnKeyPressed()
{
    P<bdn::TextField> outer = cast<bdn::TextField>(getOuterViewIfStillAttached());
    if (outer) {
        outer->submit();
    }
}

}
}

