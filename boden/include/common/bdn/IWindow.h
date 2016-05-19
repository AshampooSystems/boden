#ifndef BDN_IWINDOW_H_
#define BDN_IWINDOW_H_

#include <bdn/Property.h>

namespace bdn
{
    
class IWindow : virtual public IBase
{
public:

	
	/** Returns the Window's 'visible' property. This can be used
		to show/hide the window.

		It is safe to access this from any thread.
		*/
	virtual Property<bool>& visible()=0;
	virtual ReadProperty<bool>& visible() const=0;
    
};
    
    
}


#endif


