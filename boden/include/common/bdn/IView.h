#ifndef BDN_IView_H_
#define BDN_IView_H_

#include <bdn/Property.h>

namespace bdn
{
    
    
/** Interface for a "View". Views are the building blocks of the visible user interface.
    A view presents data or provides some user interface functionality.
    For example, buttons, text fields etc are all view objects.
   */
class IView : virtual public IBase
{
public:

	
	/** Returns the property which controls wether the view is
        visible or not.
        
        Note that a view with visible=true might still not show on
        the screen if one of its parents is invisible. In other words:
        this visible property only refers to the view itself, not
        the parent hierarchy.

		It is safe to access this from any thread.
		*/
	virtual Property<bool>& visible()=0;
	virtual ReadProperty<bool>& visible() const=0;
    
};
    
    
}


#endif


