#ifndef BDN_IWindowCore_H_
#define BDN_IWindowCore_H_

#include <bdn/IViewCore.h>
#include <bdn/ITitleCore.h>

namespace bdn
{


/** The core for a top level window.*/
class IWindowCore : BDN_IMPLEMENTS IViewCore,
					BDN_IMPLEMENTS ITitleCore
{
public:
	
};


}

#endif
