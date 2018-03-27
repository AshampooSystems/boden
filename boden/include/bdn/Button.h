#ifndef BDN_Button_H_
#define BDN_Button_H_

#include <bdn/View.h>
#include <bdn/IButtonCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/SimpleNotifier.h>

namespace bdn
{

/** A simple button with a text label.*/
class Button :	public View
{
public:
	Button()
	{
        _pOnClick = newObj< SimpleNotifier<const ClickEvent&> >();

		initProperty<String, IButtonCore, &IButtonCore::setLabel, (int)PropertyInfluence_::preferredSize>(_label);
	}

	/** Returns the button's label property.
		It is safe to use from any thread.
		*/
	Property<String>& label()
	{
		return _label;
	}

	const ReadProperty<String>& label() const
	{
		return _label;
	}
		

	ISyncNotifier<const ClickEvent&>& onClick()
	{
		return *_pOnClick;
	}


	/** Static function that returns the type name for #Button objects.*/
	static String getButtonCoreTypeName()
	{
		return "bdn.ButtonCore";
	}

	String getCoreTypeName() const override
	{
		return getButtonCoreTypeName();
	}


	DefaultProperty<String>		            _label;

	P< SimpleNotifier<const ClickEvent&> >  _pOnClick;
};


}

#endif
