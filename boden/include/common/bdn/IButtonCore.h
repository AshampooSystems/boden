#ifndef BDN_IButtonCore_H_
#define BDN_IButtonCore_H_

#include <bdn/IViewCore.h>

namespace bdn
{

class IButtonCore : BDN_IMPLEMENTS IViewCore
{
public:

	/** Changes the button's label text.*/
	virtual void setLabel(const String& label)=0;
	
};


}

#endif
