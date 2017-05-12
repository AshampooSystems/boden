#ifndef BDN_IOS_ContainerViewCore_HH_
#define BDN_IOS_ContainerViewCore_HH_

#include <bdn/ContainerView.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
namespace ios
{

class ContainerViewCore : public ViewCore
{
private:
	static UIView* _createContainer(ContainerView* pOuter)
	{
		return [[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
	}

public:
	ContainerViewCore(	ContainerView* pOuter)
		: ViewCore(pOuter, _createContainer(pOuter) )
	{
	}

		
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}


};

}
}

#endif
