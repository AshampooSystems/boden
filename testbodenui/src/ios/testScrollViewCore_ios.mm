#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Dip.h>
#include <bdn/test/TestScrollViewCore.h>
#import "TestIosViewCoreMixin.hh"

#import <bdn/ios/util.hh>

#import <bdn/ios/UiProvider.hh>

#import <bdn/ios/ScrollViewCore.hh>


using namespace bdn;

class TestIosScrollViewCore : public bdn::test::TestIosViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:


    void initCore() override
    {
        bdn::test::TestIosViewCoreMixin< bdn::test::TestScrollViewCore >::initCore();
        
        _uiScrollView = (UIScrollView*) _pUIView;
    }
    
  
    double getVertBarWidth() override
    {
        // scroll bars are only shown during scrolling as an overlay.
        // So they do not take up space in layout.
        return 0;
    }


    double getHorzBarHeight() override
    {
        // scroll bars are only shown during scrolling as an overlay.
        // So they do not take up space in layout.
        return 0;
    }
                

    bdn::Size getNonClientSize()
    {
        // our scroll views do not have a border.
        return bdn::Size(0,0);
    }

    void resizeScrollViewToViewPortSize( const bdn::Size& viewPortSize) override
    {
        // resize the scroll view so that it has exactly the desired scroll view size

        bdn::Rect newBounds( _pScrollView->position(), viewPortSize + getNonClientSize() );

        _pScrollView->adjustAndSetBounds(newBounds);
    }
    
    void verifyScrollsHorizontally( bool expectedScrolls) override
    {
        // To check whether the view actually scrolls we have to check if the content size is
        // bigger than the view size.
        
        CGSize viewPortSize = _uiScrollView.frame.size;
        CGSize contentSize = _uiScrollView.contentSize;
        
        bool scrolls = (contentSize.width > viewPortSize.width && _uiScrollView.scrollEnabled);
        
        REQUIRE( scrolls == expectedScrolls );
    }

    void verifyScrollsVertically( bool expectedScrolls) override
    {
        // To check whether the view actually scrolls we have to check if the content size is
        // bigger than the view size.
        CGSize viewPortSize = _uiScrollView.frame.size;
        CGSize contentSize = _uiScrollView.contentSize;
        
        bool scrolls = (contentSize.height > viewPortSize.height && _uiScrollView.scrollEnabled);
        
        REQUIRE( scrolls == expectedScrolls );
    }

    void verifyContentViewBounds( const bdn::Rect& expectedBounds, double maxDeviation=0) override
    {
        maxDeviation += Dip::significanceBoundary();

        P<View> pContentView =  _pScrollView->getContentView();

        if(pContentView!=nullptr)
        {
            bdn::Rect bounds( _pScrollView->getContentView()->position(), pContentView->size() );
            
            if(maxDeviation==0)
                REQUIRE( bounds == expectedBounds );
            else
            {
                REQUIRE_ALMOST_EQUAL(  bounds.x, expectedBounds.x, maxDeviation );
                REQUIRE_ALMOST_EQUAL(  bounds.y, expectedBounds.y, maxDeviation );
                REQUIRE_ALMOST_EQUAL(  bounds.width, expectedBounds.width, maxDeviation );
                REQUIRE_ALMOST_EQUAL(  bounds.height, expectedBounds.height, maxDeviation );
            }
        }
    }

    void verifyScrolledAreaSize( const bdn::Size& expectedSize) override
    {
        bdn::Size scrolledAreaSize = bdn::ios::iosSizeToSize( _uiScrollView.contentSize );
        bdn::Size viewPortSize = bdn::ios::iosSizeToSize( _uiScrollView.frame.size );

        scrolledAreaSize.applyMinimum(viewPortSize);
        
        REQUIRE(  Dip::equal( scrolledAreaSize, expectedSize ) );
    }

    void verifyViewPortSize( const bdn::Size& expectedSize) override
    {
        bdn::Size viewPortSize = bdn::ios::iosSizeToSize( _uiScrollView.frame.size );
        
        REQUIRE(  Dip::equal( viewPortSize, expectedSize ) );
    }               
    
    
private:
    UIScrollView* _uiScrollView;
};

TEST_CASE("ios.ScrollViewCore")
{
    P<TestIosScrollViewCore> pTest = newObj<TestIosScrollViewCore>();

    pTest->runTests();
}



