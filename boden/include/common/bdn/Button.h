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
		_pLabel = newObj< PropertyWithMainThreadDelegate<String> >( nullptr, "" );
	}

	/** Returns the button's label property.
		It is safe to use from any thread.
		*/
	Property<String>& label()
	{
		return *_pLabel;
	}

	ReadProperty<String>& label() const
	{
		return *_pLabel;
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
	void		setViewCore(IViewCore* pCore) override
	{
		_pCore = cast<IButtonCore>(pCore);

		if(pCore==nullptr)
			_pLabel->setDelegate(nullptr);
		else
		{
			// note that it might be tempting to add a parameter here that prevents the property
			// to update the delegate with its current value. After all, the core was just created,
			// so the delegate should already have the correct value.
			// BUT it is important to note that properties can be changed from ANY thread.
			// So the property value might have changed in the short time since the core was created
			_pLabel->setDelegate( newObj<ILabelCore::LabelDelegate>(pCore) );
		}

		View::setViewCore(pCore);
	}

	IViewCore*	getViewCore() override
	{
		return _pCore;
	}	
	
	P<IButtonCore> _pCore;

	P< PropertyWithMainThreadDelegate<String> > _pLabel;

	Notifier<const ClickEvent&> _onClick;
};


}

#endif
