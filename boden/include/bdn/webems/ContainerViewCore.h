#ifndef BDN_WEBEMS_ContainerViewCore_H_
#define BDN_WEBEMS_ContainerViewCore_H_

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

		
	
	Size calcPreferredSize( const Size& availableSpace ) const override
	{
		// XXX
            std::cout << String( typeid(*this).name() ).asUtf8() << " available width: "<<availableSpace.width << std::endl;

		// call the outer container's preferred size calculation

		P<ContainerView> pOuterView = cast<ContainerView>( getOuterViewIfStillAttached() );
		if(pOuterView!=nullptr)
			return pOuterView->calcContainerPreferredSize( availableSpace );
		else
			return Size(0,0);
	}

	void layout() override
	{
		// call the outer container's layout function

		P<ContainerView> pOuterView = cast<ContainerView>( getOuterViewIfStillAttached() );
		if(pOuterView!=nullptr)
		{
			P<ViewLayout> pLayout = pOuterView->calcContainerLayout( pOuterView->size() );
			pLayout->applyTo(pOuterView);
		}
	}


};

}
}

#endif
