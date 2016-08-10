#include <bdn/init.h>
#include <bdn/win32/ButtonCore.h>

namespace bdn
{
namespace win32
{

ButtonCore::ButtonCore(Button* pOuter)
	: ViewCore(	pOuter, 
				"BUTTON",
				pOuter->label().get(),
				BS_PUSHBUTTON | WS_CHILD,
				0 )
{
}

void ButtonCore::setLabel(const String& label)
{
	setWindowText(getHwnd(), label);
}

Size ButtonCore::calcPreferredSize() const
{
	String label = cast<Button>(_pOuterViewWeak)->label();

	const std::wstring& labelWide = label.asWide();

	HDC deviceContext = ::GetWindowDC(getHwnd());

	HGDIOBJ oldFontHandle = NULL;
	if(_pFont!=nullptr)
		oldFontHandle = ::SelectObject( deviceContext, _pFont->getHandle() );

	SIZE textSize = {0};
	::GetTextExtentPoint32W( deviceContext, labelWide.c_str(), labelWide.length(), &textSize);

	if(_pFont!=nullptr)
		::SelectObject( deviceContext, oldFontHandle );

	::ReleaseDC(getHwnd(), deviceContext);


	Size buttonSize(textSize.cx, textSize.cy);

	buttonSize += uiMarginToPixelMargin( _pOuterViewWeak->padding() );	

	// size for the 3D border around the button
	buttonSize.width += ((int)std::ceil( ::GetSystemMetrics(SM_CXEDGE) * _uiScaleFactor )) * 2;
	buttonSize.height += ((int)std::ceil( ::GetSystemMetrics(SM_CYEDGE) * _uiScaleFactor )) * 2;

	// size for the focus rect and one pixel of free space next to it
	buttonSize.width += ((int)std::ceil(2 * _uiScaleFactor)) * 2;
	buttonSize.height += ((int)std::ceil(2 * _uiScaleFactor)) * 2;

	return buttonSize;
}

int ButtonCore::calcPreferredHeightForWidth(int width) const
{
	// we do not adapt our height to the width. So the same as the unconditional one.
	return calcPreferredSize().height;	
}

int ButtonCore::calcPreferredWidthForHeight(int height) const
{
	// we do not adapt our height to the width. So the same as the unconditional one.
	return calcPreferredSize().width;	
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
}


