#ifndef BDN_WEBEMS_ContainerViewCore_HH_
#define BDN_WEBEMS_ContainerViewCore_HH_

#include <bdn/ContainerView.h>
#include <bdn/webems/ViewCore.h>


namespace bdn
{
namespace webems
{

class ContainerViewCore : public ViewCore
{
public:
	ContainerViewCore(	ContainerView* pOuter)
		: ViewCore(pOuter, "div" )
	{
	}

		
	Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}

};

}
}

#endif
