#ifndef BDN_MAC_ContainerViewCore_HH_
#define BDN_MAC_ContainerViewCore_HH_

#include <bdn/ContainerView.h>
#import <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/IParentViewCore.h>



namespace bdn
{
namespace mac
{

class ContainerViewCore : public ChildViewCore, BDN_IMPLEMENTS IParentViewCore
{
private:
	static NSView* _createContainer(ContainerView* pOuter);

public:
	ContainerViewCore(	ContainerView* pOuter)
		: ChildViewCore(pOuter, _createContainer(pOuter) )
	{
	}

		
	Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}

};

}
}

#endif
