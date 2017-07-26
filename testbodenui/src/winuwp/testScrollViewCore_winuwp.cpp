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
    }
    
  
    double getVertBarWidth()
    {
        return 0;
    }


    double getHorzBarHeight()
    {
        return 0;
    }
                

    void resizeScrollViewToViewPortSize( const Size& viewPortSize)
    {        
        bdn::Rect newBounds( _pScrollView->position(), viewPortSize );

        _pScrollView->adjustAndSetBounds(newBounds);
    }
    
    void verifyScrollsHorizontally( bool expectedScrolls) override
    {
        bool scrolls = (_pWinScrollViewer->ComputedVerticalScrollBarVisibility == ::Windows::UI::Xaml::Visibility::Visible );

        REQUIRE( scrolls == expectedScrolls);
    }

    void verifyScrollsVertically( bool expectedScrolls) override
    {
        bool scrolls = (_pWinScrollViewer->ComputedHorizontalScrollBarVisibility == ::Windows::UI::Xaml::Visibility::Visible );

        REQUIRE( scrolls == expectedScrolls);
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        P<View> pContentView = cast<ScrollView>(_pView)->getContentView();

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

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        double width = _pWinScrollViewer->ExtentWidth;
        double height = _pWinScrollViewer->ExtentHeight;

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


};

TEST_CASE("winuwp.ScrollViewCore")
{
    P<TestWinuwpScrollViewCore> pTest = newObj<TestWinuwpScrollViewCore>();

    pTest->runTests();
}

