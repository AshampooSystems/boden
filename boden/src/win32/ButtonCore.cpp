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
				pOuter->label(),
				BS_PUSHBUTTON | WS_CHILD,
				0 )
{
}

void ButtonCore::setLabel(const String& label)
{
	setWindowText(getHwnd(), label);
}

Size ButtonCore::calcPreferredSize( const Size& availableSpace ) const
{
    String label;

    // we completely ignore the available space. We do not have any way to reduce the needed
    // size for the button.

    P<const Button> pButton = cast<const Button>( getOuterViewIfStillAttached() );
    if(pButton!=nullptr)
	    label = pButton->label();

    Size prefSize;

    {
        WindowDeviceContext dc( getHwnd() );

        P<const win32::Font> pFont = getFont();
        if(pFont!=nullptr)
		    dc.setFont( *pFont );
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
        uiPadding = UiMargin( UiLength::sem(0.12),
                              UiLength::sem( 0.5) );
    }
    else
        uiPadding = pad;
    
	prefSize += uiMarginToDipMargin( uiPadding );	

	// size for the 3D border around the button. Note that prefSize is in DIPs,
    // so it is ok to add the values unscaled.
	prefSize.width += ::GetSystemMetrics(SM_CXBORDER) * 2;
	prefSize.height += ::GetSystemMetrics(SM_CYBORDER) * 2;

	// size for the focus rect and one pixel of free space next to it
	prefSize.width += 2 * 2;
	prefSize.height += 2 * 2;

    if(pButton!=nullptr)
    {
        prefSize.applyMaximum( pButton->preferredSizeMaximum() );
        prefSize.applyMinimum( pButton->preferredSizeMinimum() );
    }

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


