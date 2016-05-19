#ifndef BDN_ISwitch_H_
#define BDN_ISwitch_H_

#include <bdn/IWindow.h>

namespace bdn
{
    
class ISwitch : BDN_IMPLEMENTS IWindow
{
public:

	
	/** The switch's label.
		It is safe to access this from any thread.
		*/
	virtual Property<String>& label()=0;
	virtual ReadProperty<String>& label() const=0;
    
};
    
    
}


#endif


