#ifndef BDN_Button_H_
#define BDN_Button_H_


#include <bdn/Window.h>

#include <bdn/IButton.h>


namespace bdn
{

class Button : public Window, BDN_IMPLEMENTS IButton
{
public:
	Button(Window* pParent, const std::string& label)
	{
		create(pParent, "BUTTON", label, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 0, 20, 20, 200, 30 );
	}


	/** Returns the button's label property.
		It is safe to use from any thread.
		*/
	Property<String>& label() override
	{
		return *_pText;
	}

	ReadProperty<String>& label() const override
	{
		return *_pText;
	}
		

	void generateClick()
	{
		ClickEvent evt(this);

		_onClick.notify(evt);
	}

	Notifier<const ClickEvent&>& onClick()
	{
		return _onClick;
	}

protected:		
	void handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;

	Notifier<const ClickEvent&> _onClick;
};


}

#endif
