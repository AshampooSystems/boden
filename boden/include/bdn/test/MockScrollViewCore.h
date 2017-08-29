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


    void scrollClientRectToVisible(const Rect& targetRect) override
    {
        // update the scroll position of the outer view
        P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );

        if(pOuterView!=nullptr)
        {
            Rect visibleRect = pOuterView->visibleClientRect();
            
            double targetLeft = targetRect.x;
            double targetRight = targetRect.x + targetRect.width;
            double targetTop = targetRect.y;
            double targetBottom = targetRect.y + targetRect.height;

            // clip the target rect to the client rect
            if(targetLeft<0)
                targetLeft = 0;
            if(targetRight<0)
                targetRight = 0;
            if(targetTop<0)
                targetTop = 0;
            if(targetBottom<0)
                targetBottom = 0;

            if(targetLeft > _clientSize.width)
                targetLeft = _clientSize.width;
            if(targetRight > _clientSize.width)
                targetRight = _clientSize.width;
            if(targetTop > _clientSize.height)
                targetTop = _clientSize.height;
            if(targetBottom > _clientSize.height)
                targetBottom = _clientSize.height;
            
            double visibleLeft = visibleRect.x;
            double visibleRight = visibleRect.x + visibleRect.width;
            double visibleTop = visibleRect.y;
            double visibleBottom = visibleRect.y + visibleRect.height;            

            // move the visible rect so that it shows the desired target area
            if(targetRight > visibleRight)
            {
                visibleRight = targetRight;
                visibleLeft = visibleRight - visibleRect.width;
            }
            if(targetLeft < visibleLeft)
            {
                visibleLeft = targetLeft;
                visibleRight = visibleLeft + visibleRect.width;
            }

            if(targetBottom > visibleBottom)
            {
                visibleBottom = targetBottom;
                visibleTop = visibleBottom - visibleRect.height;
            }
            if(targetTop < visibleTop)
            {
                visibleTop = targetTop;
                visibleBottom = visibleTop + visibleRect.height;
            }

            visibleRect = Rect( visibleLeft, visibleTop, visibleRight-visibleLeft, visibleBottom-visibleTop);
            
            pOuterView->_setVisibleClientRect( visibleRect );
        }
    }

    void layout() override
    {
        BDN_REQUIRE_IN_MAIN_THREAD();

        if( !_overrideLayoutFunc || !_overrideLayoutFunc() )
        {
            P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );

            ScrollViewLayoutHelper helper(10, 10);
            helper.calcLayout( pOuterView, _bounds.getSize() );

            _clientSize = helper.getScrolledAreaSize();
        
            if(pOuterView!=nullptr)
            {
                P<View> pContentView = pOuterView->getContentView();
                if(pContentView!=nullptr)
                    pContentView->adjustAndSetBounds( helper.getContentViewBounds() );
            }

            _horizontalScrollBarVisible = helper.getHorizontalScrollBarVisible();
            _verticalScrollBarVisible = helper.getVerticalScrollBarVisible();

            Size viewPortSize = helper.getViewPortSize();

            // update the size of the visible client rect
            Rect visibleClientRect = pOuterView->visibleClientRect();
            visibleClientRect.width = viewPortSize.width;
            visibleClientRect.height = viewPortSize.height;

            // make sure that the visible client rect is inside the client area            
            if(visibleClientRect.x + visibleClientRect.width > _clientSize.width)
                visibleClientRect.x = _clientSize.width - visibleClientRect.width;
            if(visibleClientRect.y + visibleClientRect.height > _clientSize.height)
                visibleClientRect.y = _clientSize.height - visibleClientRect.height;
            
            pOuterView->_setVisibleClientRect( visibleClientRect );
        }
    }


    

	
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
        MockViewCore::calcPreferredSize(availableSpace);

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

    Size _clientSize;
};


}
}

#endif