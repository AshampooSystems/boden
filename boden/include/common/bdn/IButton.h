#ifndef BDN_IButton_H_
#define BDN_IButton_H_

#include <bdn/IWindow.h>
#include <bdn/ClickEvent.h>

namespace bdn
{
    

/** A push-button.

	Objects can be created with createButton().
*/
class IButton : BDN_IMPLEMENTS IWindow
{
public:
	
	/** The button's label.
		It is safe to access this from any thread.
		*/
	virtual Property<String>& label()=0;
	virtual ReadProperty<String>& label() const=0;
    

	/** A notifier that fires when the button is clicked.*/
	virtual Notifier<const ClickEvent&>& onClick()=0;
};
    

/** Creates a button.*/
P<IButton> createButton(IWindow* pParent, const String& label);
    
}


#endif


