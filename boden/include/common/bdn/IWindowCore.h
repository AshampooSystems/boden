#ifndef BDN_IWindowCore_H_
#define BDN_IWindowCore_H_

#include <bdn/IViewCore.h>

namespace bdn
{


/** The core for a top level window.*/
class IWindowCore : BDN_IMPLEMENTS IViewCore
{
public:

	/** Changes the window's title.*/
	virtual void setTitle(const String& title)=0;
	
};


}

#endif
