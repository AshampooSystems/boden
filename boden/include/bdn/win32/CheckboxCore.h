#ifndef BDN_WIN32_CheckboxCore_H_
#define BDN_WIN32_CheckboxCore_H_

#include <bdn/Checkbox.h>
#include <bdn/Toggle.h>
#include <bdn/ISwitchCore.h>
#include <bdn/IToggleCoreBase.h>
#include <bdn/win32/ViewCore.h>
#include <bdn/win32/WindowDeviceContext.h>

namespace bdn
{
namespace win32
{

template <class T>
class CheckboxCore : public ViewCore, BDN_IMPLEMENTS ICheckboxCore, BDN_IMPLEMENTS ISwitchCore
{
public:
	CheckboxCore(T* pOuter)
		: ViewCore(pOuter, 
				   "BUTTON",
			  	   pOuter->label(),
				   BS_3STATE | WS_CHILD,
				   0)
	{
		setState(pOuter->state());
	}

	void setLabel(const String& label) override
	{
		setWindowText(getHwnd(), label);
	}

	void setState(const TriState& state) override
	{
	    WPARAM win32CheckedState;
	    switch (state) {
	    case TriState::on:    win32CheckedState = BST_CHECKED;       break;
	    case TriState::off:   win32CheckedState = BST_UNCHECKED;     break;
	    case TriState::mixed: win32CheckedState = BST_INDETERMINATE; break;
	    }

	    SendMessage(getHwnd(), BM_SETCHECK, win32CheckedState, 0);		
	}

	void setOn(const bool& on) override
	{
		setState(on ? TriState::on : TriState::off);
	}

	void generateClick()
	{
	    P<View> pOuter = getOuterViewIfStillAttached();

	    if(pOuter!=nullptr)
	    {
	        ClickEvent evt( pOuter );

	        // Click is triggered even if the user uses the keyboard to check the checkbox,
	        // so we set the outer's state here to inform the framework about the state change.
	        P<Checkbox> pCheckbox = tryCast<Checkbox>(pOuter);
	        P<Toggle> pToggle = tryCast<Toggle>(pOuter);
	        if (pCheckbox) {
	        	cast<Checkbox>(pOuter)->setState( _state() );
	        } else if(pToggle) {
	        	cast<Toggle>(pOuter)->setOn( _state() == TriState::on );
	        }

	        // We guarantee that the on property will be set on the outer control before a
	        // notification is posted to onClick.
	        cast<Checkbox>(pOuter)->onClick().notify(evt);        
	    }
	}

	TriState _state() const
	{
	    UINT win32CheckedState = SendMessage(getHwnd(),BM_GETCHECK,0, 0);
	    TriState state;
	    switch (win32CheckedState) {
	    case BST_CHECKED:       state = TriState::on;    break;
	    case BST_UNCHECKED:     state = TriState::off;   break;
	    case BST_INDETERMINATE: state = TriState::mixed; break;
	    }    
	    return state;
	}

	bool _on() const
	{
	    UINT state = SendMessage(getHwnd(),BM_GETCHECK,0, 0);
	    return state == BST_CHECKED;
	}

	Size calcPreferredSize( const Size& availableSpace ) const
	{
	    String label;

	    P<const T> pOuter = cast<const T>( getOuterViewIfStillAttached() );
	    if (pOuter != nullptr)
		    label = pOuter->label();

	    Size prefSize;

	    {
	        WindowDeviceContext dc( getHwnd() );

	        P<const win32::Font> pFont = getFont();
	        if(pFont!=nullptr)
			    dc.setFont( *pFont );
	        prefSize = dc.getTextSize( label );
	    }

	    int checkboxWidth = GetSystemMetrics(SM_CXMENUCHECK);
	    prefSize.width += checkboxWidth + 4; // 4 = magic for gap between checkbox and setLabel
	    
		Nullable<UiMargin>  pad;
	    if(pOuter!=nullptr)
	        pad = pOuter->padding();
	    UiMargin            uiPadding;
	    if(pad.isNull())
	    {
	        uiPadding = UiMargin( UiLength::sem(0),
	                              UiLength::sem(0) );
	    }
	    else
	        uiPadding = pad;
	    
		prefSize += uiMarginToDipMargin( uiPadding );	

	    if(pOuter!=nullptr)
	    {
	        prefSize.applyMaximum( pOuter->preferredSizeMaximum() );
	        prefSize.applyMinimum( pOuter->preferredSizeMinimum() );
	    }

		return prefSize;
	}

protected:		
	void handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(message==WM_COMMAND)
		{
	        // Set state on HWND
	        setState(_state() == TriState::off || _state() == TriState::mixed ? TriState::on : TriState::off);
			// we were clicked.
			generateClick();
		}

		return ViewCore::handleParentMessage(context, windowHandle, message, wParam, lParam);
	}
};


}
}

#endif
