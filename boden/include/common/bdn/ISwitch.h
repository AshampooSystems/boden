#ifndef BDN_ISwitch_H_
#define BDN_ISwitch_H_

#include <bdn/IWindow.h>

namespace bdn
{
    

/** An "on/off" switch. The actual look depends on the implementation.
	For example, it might be a checkbox, toggle-button or slider button.
	
	Objects can be created with createSwitch().
	*/
class ISwitch : BDN_IMPLEMENTS IWindow
{
public:
	
	/** The switch's label.
		It is safe to access this from any thread.
		*/
	virtual Property<String>& label()=0;
	virtual ReadProperty<String>& label() const=0;
    
};
    

/** Creates a switch object.*/
P<ISwitch> createSwitch(IWindow* pParent, const String& label);
    
}


#endif


