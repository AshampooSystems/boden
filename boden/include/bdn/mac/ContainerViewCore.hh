#ifndef BDN_MAC_ContainerViewCore_HH_
#define BDN_MAC_ContainerViewCore_HH_

#include <bdn/ContainerView.h>
#import <bdn/mac/ViewCore.hh>
#include <bdn/mac/IParentViewCore.h>


/** NSView implementation that is used internally by bdn::mac::ContainerViewCore.

    Sets the flipped property so that the coordinate system has its origin in the top left,
    rather than the bottom left.
*/
@interface BdnMacContainerView_ : NSView

@end

@implementation BdnMacContainerView_

- (BOOL) isFlipped
{
    return YES;
}

@end


namespace bdn
{
namespace mac
{

class ContainerViewCore : public ViewCore, BDN_IMPLEMENTS IParentViewCore
{
private:
	static NSView* _createContainer(ContainerView* pOuter)
	{
		return [[BdnMacContainerView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
	}

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
