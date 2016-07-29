#ifndef BDN_MAC_IParentViewCore_H_
#define BDN_MAC_IParentViewCore_H_

#include <Cocoa/Cocoa.h>

namespace bdn
{
namespace mac
{


/** Interface for view cores that can act as parents for other view cores.*/
class IParentViewCore : BDN_IMPLEMENTS IBase
{
public:

	/** Adds a child Ui element to the parent.*/
	virtual void addChildNsView( NSView* nsView )=0;
};
	
}
}

#endif