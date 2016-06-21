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

Size ButtonCore::calcPreferredSize() const
{
	String label = cast<Button>(_pOuterViewWeak)->label();

	const std::wstring& labelWide = label.asWide();

	HDC deviceContext = ::GetWindowDC(getHwnd());

	SIZE textSize = {0};
	::GetTextExtentPoint32W( deviceContext, labelWide.c_str(), labelWide.length(), &textSize);

	::ReleaseDC(getHwnd(), deviceContext);


	Size buttonSize(textSize.cx, textSize.cy);

	buttonSize += uiMarginToPixelMargin( _pOuterViewWeak->padding() );	

	buttonSize.width += ::GetSystemMetrics(SM_CXEDGE)*2;
	buttonSize.height += ::GetSystemMetrics(SM_CYEDGE)*2;

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


