#ifndef BDN_TEST_MockScrollViewCore_H_
#define BDN_TEST_MockScrollViewCore_H_

#include <bdn/IScrollViewCore.h>
#include <bdn/ScrollView.h>
#include <bdn/ScrollViewLayoutHelper.h>

#include <bdn/test/MockViewCore.h>

namespace bdn
{
namespace test
{


/** Implementation of a "fake" scrollview core that does not actually show anything
    visible, but behaves otherwise like a normal scrollview core.
    
    See MockUiProvider.
    */
class MockScrollViewCore : public MockViewCore, BDN_IMPLEMENTS IScrollViewCore
{
public:
	MockScrollViewCore(ScrollView* pView)
		: MockViewCore(pView)
	{
		BDN_REQUIRE_IN_MAIN_THREAD();       

        _horizontalScrollingEnabled = pView->horizontalScrollingEnabled();
        _verticalScrollingEnabled = pView->verticalScrollingEnabled();
    }


    
	void setHorizontalScrollingEnabled(const bool& enabled) override
    {
        _horizontalScrollingEnabled = enabled;
        _horizontalScrollingEnabledChangeCount++;        
    }

    int getHorizontalScrollingEnabledChangeCount() const
    {
        return _horizontalScrollingEnabledChangeCount;
    }

    
	void setVerticalScrollingEnabled(const bool& enabled) override
    {
        _verticalScrollingEnabled = enabled;
        _verticalScrollingEnabledChangeCount++;        
    }
    
    int getVerticalScrollingEnabledChangeCount() const
    {
        return _verticalScrollingEnabledChangeCount;
    }


    bool getHorizontalScrollBarVisible() const
    {
        return _horizontalScrollBarVisible;
    }

    bool getVerticalScrollBarVisible() const
    {
        return _verticalScrollBarVisible;
    }

    void layout() override
    {
        BDN_REQUIRE_IN_MAIN_THREAD();

        P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );

        ScrollViewLayoutHelper helper(10, 10);
        helper.calcLayout( pOuterView, _bounds.getSize() );
        
        if(pOuterView!=nullptr)
        {
            P<View> pContentView = pOuterView->getContentView();
            if(pContentView!=nullptr)
                pContentView->adjustAndSetBounds( helper.getContentViewBounds() );
        }

        _horizontalScrollBarVisible = helper.getHorizontalScrollBarVisible();
        _verticalScrollBarVisible = helper.getVerticalScrollBarVisible();
    }


    

	
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		BDN_REQUIRE_IN_MAIN_THREAD();

        P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );

        ScrollViewLayoutHelper helper(10, 10);
        Size prefSize = helper.calcPreferredSize( pOuterView, availableSpace );

        return prefSize;
    }


private:
    bool _horizontalScrollingEnabled = false;
    int  _horizontalScrollingEnabledChangeCount = 0;    
    bool _verticalScrollingEnabled = false;
    int  _verticalScrollingEnabledChangeCount = 0;
    
    bool _horizontalScrollBarVisible = false;
    bool _verticalScrollBarVisible = false;
};


}
}

#endif