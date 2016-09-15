#ifndef BDN_WEB_ContainerViewCore_HH_
#define BDN_WEB_ContainerViewCore_HH_

#include <bdn/ContainerView.h>
#include <bdn/web/ViewCore.h>


namespace bdn
{
namespace web
{

class ContainerViewCore : public ViewCore
{
public:
	ContainerViewCore(	ContainerView* pOuter)
		: ViewCore(pOuter, "div" )
	{
	}

		
	Size calcPreferredSize(int availableWidth=-1, int availableHeight=-1) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}

};

}
}

#endif
