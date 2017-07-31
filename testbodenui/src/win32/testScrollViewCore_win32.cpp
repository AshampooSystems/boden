#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Dip.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/ScrollViewCore.h>
#include "TestWin32ViewCoreMixin.h"

#include <bdn/win32/util.h>

using namespace bdn;

class TestWin32ScrollViewCore : public bdn::test::TestWin32ViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:

    
  
    double getVertBarWidth()
    {
        return ::GetSystemMetrics(SM_CXVSCROLL);
    }


    double getHorzBarHeight()
    {
        return ::GetSystemMetrics(SM_CYHSCROLL);
    }
                

    Size getNonClientSize()
    {        
        RECT rect{0,0,0,0};
        ::AdjustWindowRect( &rect, ::GetWindowLongPtr( _hwnd, GWL_STYLE), FALSE );

        return Size( (-rect.left) + rect.right, (-rect.top + rect.bottom));
    }

    void initiateScrollViewResizeToHaveViewPortSize( const Size& viewPortSize) override
    {
        // resize the scroll view so that it has exactly the desired scroll view size

        Rect newBounds( _pScrollView->position() ,viewPortSize + getNonClientSize() );

        _pScrollView->adjustAndSetBounds(newBounds);
    }
    
    void verifyScrollsHorizontally( bool expectedVisible) override
    {
        int style = ::GetWindowLongPtr( _hwnd, GWL_STYLE);

        bool horzBarVisible = ((style & WS_HSCROLL)==WS_HSCROLL);

        REQUIRE( horzBarVisible == expectedVisible );
    }

    void verifyScrollsVertically( bool expectedVisible) override
    {
        int style = ::GetWindowLongPtr( _hwnd, GWL_STYLE);

        bool vertBarVisible = ((style & WS_VSCROLL)==WS_VSCROLL);

        REQUIRE( vertBarVisible == expectedVisible );
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        maxDeviation += Dip::significanceBoundary();

        P<View> pContentView =  _pScrollView->getContentView();

        if(pContentView!=nullptr)
        {
            Rect bounds( _pScrollView->getContentView()->position(), pContentView->size() );

            // the bounds are rounded to pixel boundaries, so there can always be some deviation.
            // At most 1 pixel. Add that to maxDeviation
            maxDeviation += 1.0/_pWin32Core->getUiScaleFactor();
            
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

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        HWND contentContainerHwnd = cast<bdn::win32::ScrollViewCore>( _pScrollViewCore )->getParentHwndForChildren();

        RECT winContentContainerRect;
        ::GetClientRect(contentContainerHwnd, &winContentContainerRect);

        Rect contentContainerRect = bdn::win32::win32RectToRect( winContentContainerRect, _pWin32Core->getUiScaleFactor() );

        Size scrolledAreaSize = contentContainerRect.getSize();

        // the bounds are rounded to pixel boundaries, so there can always be some deviation.
        // At most 1 pixel.
        double maxDeviation = 1.0/_pWin32Core->getUiScaleFactor();

        // add a little more to account for floating point rounding errors.
        maxDeviation += 0.0001;

        REQUIRE_ALMOST_EQUAL(  scrolledAreaSize.width, expectedSize.width, maxDeviation );
        REQUIRE_ALMOST_EQUAL(  scrolledAreaSize.height, expectedSize.height, maxDeviation );
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        HWND windowHwnd = _pWin32Core->getHwnd();

        RECT winRect;
        ::GetWindowRect(windowHwnd, &winRect);

        Rect rect = bdn::win32::win32RectToRect( winRect, _pWin32Core->getUiScaleFactor() );

        Size viewPortSize = rect.getSize();

        viewPortSize -= getNonClientSize();
        
        int style = GetWindowLong(windowHwnd, GWL_STYLE);

        if( (style & WS_HSCROLL)==WS_HSCROLL)
            viewPortSize.height -= getHorzBarHeight();

        if( (style & WS_VSCROLL)==WS_VSCROLL)
            viewPortSize.width -= getVertBarWidth();

        // the bounds are rounded to pixel boundaries, so there can always be some deviation.
        // At most 1 pixel.
        double maxDeviation = 1.0/_pWin32Core->getUiScaleFactor();

        REQUIRE_ALMOST_EQUAL(  viewPortSize.width, expectedSize.width, maxDeviation );
        REQUIRE_ALMOST_EQUAL(  viewPortSize.height, expectedSize.height, maxDeviation );
    }               
             

};

TEST_CASE("win32.ScrollViewCore")
{
    P<TestWin32ScrollViewCore> pTest = newObj<TestWin32ScrollViewCore>();

    pTest->runTests();
}

