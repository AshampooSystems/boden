#include <bdn/init.h>
#include <bdn/win32/ButtonCore.h>

#include <bdn/win32/WindowDeviceContext.h>

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

Size ButtonCore::calcPreferredSize(int availableWidth, int availableHeight) const
{
    String label;

    P<const Button> pButton = cast<const Button>( getOuterViewIfStillAttached() );
    if(pButton!=nullptr)
	    label = pButton->label();

    Size prefSize;

    {
        WindowDeviceContext dc( getHwnd() );

        if(_pFont!=nullptr)
		    dc.setFont( *_pFont );
        prefSize = dc.getTextSize( label );
    }
    
	Nullable<UiMargin>  pad;
    if(pButton!=nullptr)
        pad = pButton->padding();
    UiMargin            uiPadding;
    if(pad.isNull())
    {
        // we should use the "default" padding. On win32 there is no real system-defined
        // default, so we choose one that looks good for most buttons:
        uiPadding = UiMargin(UiLength::sem, 0.12, 0.5);
    }
    else
        uiPadding = pad;

	prefSize += uiMarginToPixelMargin( uiPadding );	

	// size for the 3D border around the button
	prefSize.width += ((int)std::ceil( ::GetSystemMetrics(SM_CXEDGE) * _uiScaleFactor )) * 2;
	prefSize.height += ((int)std::ceil( ::GetSystemMetrics(SM_CYEDGE) * _uiScaleFactor )) * 2;

	// size for the focus rect and one pixel of free space next to it
	prefSize.width += ((int)std::ceil(2 * _uiScaleFactor)) * 2;
	prefSize.height += ((int)std::ceil(2 * _uiScaleFactor)) * 2;

	return prefSize;
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


