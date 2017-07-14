#ifndef BDN_TEST_MockContainerViewCore_H_
#define BDN_TEST_MockContainerViewCore_H_


#include <bdn/test/MockViewCore.h>

#include <bdn/ContainerView.h>

#include <bdn/ProgrammingError.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" container view core that does not actually show anything
    visible, but behaves otherwise like a normal container view core.
    
    See MockUiProvider.
    */
class MockContainerViewCore : public MockViewCore
{
public:
	MockContainerViewCore(ContainerView* pView)
		: MockViewCore(pView)
	{
	}
    

   
    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
    {
        MockViewCore::calcPreferredSize(availableSpace);

        // call the outer container's preferred size calculation
        P<ContainerView> pOuterView = cast<ContainerView>( getOuterViewIfStillAttached() );
        if(pOuterView!=nullptr)
            return pOuterView->calcContainerPreferredSize( availableSpace );
        else
            return Size(0,0);
    }


	void layout() override
	{
        BDN_REQUIRE_IN_MAIN_THREAD();
		
		_layoutCount++;

        if( !_overrideLayoutFunc || !_overrideLayoutFunc() )
        {
		    P<ContainerView> pView = cast<ContainerView>( getOuterViewIfStillAttached() );
		
		    P<ViewLayout> pLayout = pView->calcContainerLayout( pView->size() );
		    pLayout->applyTo( pView );
        }
	}


};

}
}

#endif