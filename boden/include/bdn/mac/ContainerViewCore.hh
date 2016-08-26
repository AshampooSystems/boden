#ifndef BDN_MAC_ContainerViewCore_HH_
#define BDN_MAC_ContainerViewCore_HH_

#include <bdn/ContainerView.h>
#import <bdn/mac/ViewCore.hh>
#include <bdn/mac/IParentViewCore.h>



namespace bdn
{
namespace mac
{

class ContainerViewCore : public ViewCore, BDN_IMPLEMENTS IParentViewCore
{
private:
	static NSView* _createContainer(ContainerView* pOuter);

public:
	ContainerViewCore(	ContainerView* pOuter)
		: ViewCore(pOuter, _createContainer(pOuter) )
	{
	}

		
	Size calcPreferredSize() const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}

	int calcPreferredHeightForWidth(int width) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredHeightForWidth must be overloaded in derived class.");
	}

	int calcPreferredWidthForHeight(int height) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredWidthForHeight must be overloaded in derived class.");
	}

};

}
}

#endif
