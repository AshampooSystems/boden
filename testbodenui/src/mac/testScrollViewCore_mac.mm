#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Dip.h>
#include <bdn/test/TestScrollViewCore.h>
#import "TestMacChildViewCoreMixin.hh"

#import <bdn/mac/util.hh>

#import <bdn/mac/UiProvider.hh>

#import <bdn/mac/ScrollViewCore.hh>


using namespace bdn;

class TestMacScrollViewCore : public bdn::test::TestMacChildViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:


    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin< bdn::test::TestScrollViewCore >::initCore();
        
        _nsScrollView = (NSScrollView*) _pNSView;
    }
    
    double getScrollerWidth()
    {
        // whether or not scroll bars are an overlay that do not take up any space
        // or a normal child component depends on the scroller style. The scroller style
        // is a system-controlled property that can dynamically change based on the used input
        // method.
        
        NSScrollerStyle style = [NSScroller preferredScrollerStyle];
        
        double width = [NSScroller scrollerWidthForControlSize: NSControlSizeRegular
                                                 scrollerStyle: style];
        
        return width;
    }
    
  
    double getVertBarWidth() override
    {
        return getScrollerWidth();
    }


    double getHorzBarHeight() override
    {
        return getScrollerWidth();
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
        // the scroller object always exists if scrolling is generally enabled.
        // To check whether the view actually scrolls we have to check if the content size is
        // bigger than the view size.
        NSSize viewPortSize = _nsScrollView.contentSize;
        NSSize docSize{0,0};
        
        if(_nsScrollView.documentView!=nil)
            docSize = _nsScrollView.documentView.frame.size;
        
        bool scrolls = (docSize.width > viewPortSize.width);
        
        REQUIRE( scrolls == expectedScrolls );
    }

    void verifyScrollsVertically( bool expectedScrolls) override
    {
        // the scroller object always exists if scrolling is generally enabled.
        // To check whether the view actually scrolls we have to check if the content size is
        // bigger than the view size.
        NSSize viewPortSize = _nsScrollView.contentSize;
        NSSize docSize{0,0};
        
        if(_nsScrollView.documentView!=nil)
            docSize = _nsScrollView.documentView.frame.size;
        
        bool scrolls = (docSize.height > viewPortSize.height);
        
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
        bdn::Size scrolledAreaSize = bdn::mac::macSizeToSize( _nsScrollView.documentView.frame.size );
        
        bdn::Size viewPortSize = bdn::mac::macSizeToSize( _nsScrollView.contentSize );

        scrolledAreaSize.applyMinimum(viewPortSize);
        
        REQUIRE(  Dip::equal( scrolledAreaSize, expectedSize ) );
    }

    void verifyViewPortSize( const bdn::Size& expectedSize) override
    {
        // NSScrollView refers to the viewport as the "content view".
        // The actual inner view with the scrolled content is the "document view".
        bdn::Size viewPortSize = bdn::mac::macSizeToSize( _nsScrollView.contentSize );
        
        REQUIRE(  Dip::equal( viewPortSize, expectedSize ) );
    }               
    
    
private:
    NSScrollView* _nsScrollView;
};

TEST_CASE("mac.ScrollViewCore")
{
    P<TestMacScrollViewCore> pTest = newObj<TestMacScrollViewCore>();

    pTest->runTests();
}



