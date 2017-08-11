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

		
    
    Size calcPreferredSize( const Size& availableSpace ) const override
    {
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
