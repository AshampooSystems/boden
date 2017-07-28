#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/winuwp/UiProvider.h>
#include "TestWinuwpViewCoreMixin.h"


using namespace bdn;

class TestWinuwpScrollViewCore : public bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:


    void initCore() override
    {
        bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestScrollViewCore >::initCore();

        _pWinScrollViewer = (::Windows::UI::Xaml::Controls::ScrollViewer^) _pWinFrameworkElement;
        _pWinContentWrapper = (::Windows::UI::Xaml::Controls::Panel^)_pWinScrollViewer->Content;
    }
    
  
    double getVertBarWidth()
    {
        return 0;
    }


    double getHorzBarHeight()
    {
        return 0;
    }
                

    void initiateScrollViewResizeToHaveViewPortSize( const Size& viewPortSize)
    {        
        // we cannot resize the scroll view directly with adjustAndSetBounds.
        // That would not have any effect outside of a layout cycle.
        // Instead we set the preferred size min and max to force the outer view
        // to resize it to the specified size.

        _pScrollView->preferredSizeMinimum() = viewPortSize;
        _pScrollView->preferredSizeMaximum() = viewPortSize;

        // also request a re-layout here. With the normal propagation of the property changes
        // it would take two event cycles until the layout happens. But we want it to happen
        // immediately after the properties have been changed.
        _pScrollView->getParentView()->needLayout( View::InvalidateReason::customDataChanged );
    }
    
    void verifyScrollsHorizontally( bool expectedScrolls) override
    {
        // the view will scroll when the scrolled area view is bigger than the scroll view.
        double scrolledAreaWidth = _pWinContentWrapper->ActualWidth;        
        double viewWidth = _pWinScrollViewer->ActualWidth;

        bool scrolls = (scrolledAreaWidth > viewWidth);

        REQUIRE( scrolls == expectedScrolls);
    }

    void verifyScrollsVertically( bool expectedScrolls) override
    {
        double scrolledAreaHeight = _pWinContentWrapper->ActualHeight;        
        double viewHeight = _pWinScrollViewer->ActualHeight;

        bool scrolls = (scrolledAreaHeight > viewHeight);

        REQUIRE( scrolls == expectedScrolls);
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        P<View> pContentView = cast<ScrollView>(_pView)->getContentView();

        if(pContentView!=nullptr)
        {
            REQUIRE( pContentView!=nullptr );
        
            P<bdn::winuwp::IUwpViewCore> pContentViewCore = cast<bdn::winuwp::IUwpViewCore>( pContentView->getViewCore() );

            ::Windows::UI::Xaml::FrameworkElement^ pCoreEl = pContentViewCore->getFrameworkElement();

            // we cannot verify the position of the framework element because UWP does not expose it.
            // So we only verify the size

            double width = pCoreEl->ActualWidth;
            double height = pCoreEl->ActualHeight;

            REQUIRE_ALMOST_EQUAL( width, expectedBounds.width, maxDeviation );
            REQUIRE_ALMOST_EQUAL( height, expectedBounds.height, maxDeviation );
        }
    }

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        double width = _pWinContentWrapper->ActualWidth;
        double height = _pWinContentWrapper->ActualHeight;

        REQUIRE( Size(width, height) == expectedSize );
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        double width = _pWinScrollViewer->ViewportWidth;
        double height = _pWinScrollViewer->ViewportHeight;

        REQUIRE( Size(width, height) == expectedSize );
    }               
             
    void verifyCorePadding() override
    {
        // the padding is not represented in the Xaml controls. We implement it manually.
        // So there is nothing to test here.
    }


    ::Windows::UI::Xaml::Controls::ScrollViewer^ _pWinScrollViewer;
    ::Windows::UI::Xaml::Controls::Panel^        _pWinContentWrapper;


};

TEST_CASE("winuwp.ScrollViewCore")
{
    P<TestWinuwpScrollViewCore> pTest = newObj<TestWinuwpScrollViewCore>();

    pTest->runTests();
}

