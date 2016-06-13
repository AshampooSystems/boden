#ifndef BDN_Button_H_
#define BDN_Button_H_

#include <bdn/View.h>
#include <bdn/IButtonCore.h>
#include <bdn/ClickEvent.h>

namespace bdn
{

/** A simple button with a text label.*/
class Button :	public View
{
public:
	Button()
	{
		initProperty<String, IButtonCore, &IButtonCore::setLabel>(_label);
	}

	/** Returns the button's label property.
		It is safe to use from any thread.
		*/
	Property<String>& label()
	{
		return *_label;
	}

	ReadProperty<String>& label() const
	{
		return *_label;
	}
		

	Notifier<const ClickEvent&>& onClick()
	{
		return _onClick;
	}


	/** Static function that returns the type name for #Button objects.*/
	static String getButtonViewTypeName()
	{
		return "bdn.Button";
	}

	String getViewTypeName() const override
	{
		return getButtonViewTypeName();
	}


protected:	
	DefaultProperty<String>		_label;

	Notifier<const ClickEvent&> _onClick;
};


}

#endif
