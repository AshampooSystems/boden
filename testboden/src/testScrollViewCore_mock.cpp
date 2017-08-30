#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Dip.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/test/MockScrollViewCore.h>
#include "TestMockViewCoreMixin.h"


using namespace bdn;

class TestMockScrollViewCore : public bdn::test::TestMockViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:


    
    void initCore() override
    {
        bdn::test::TestMockViewCoreMixin< bdn::test::TestScrollViewCore >::initCore();

        _pMockScrollViewCore = cast<bdn::test::MockScrollViewCore>( _pMockCore );
    }

    
  
    double getVertBarWidth()
    {
        return 10;
    }


    double getHorzBarHeight()
    {
        return 10;
    }
     

    void initiateScrollViewResizeToHaveViewPortSize( const Size& viewPortSize) override
    {
        _pScrollView->preferredSizeMinimum() = viewPortSize;
        _pScrollView->preferredSizeMaximum() = viewPortSize;

        _pWindow->requestAutoSize();
    }
    
    void verifyScrollsHorizontally( bool expectedVisible) override
    {
        REQUIRE( _pMockScrollViewCore->getHorizontalScrollBarVisible() == expectedVisible );
    }

    void verifyScrollsVertically( bool expectedVisible) override
    {
        REQUIRE( _pMockScrollViewCore->getVerticalScrollBarVisible() == expectedVisible );
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        maxDeviation += Dip::significanceBoundary();

        P<View> pContentView =  _pScrollView->getContentView();

        if(pContentView!=nullptr)
        {
            Rect bounds( _pScrollView->getContentView()->position(), pContentView->size() );

            // the bounds are rounded to pixel boundaries, so there can always be some deviation.
            // At most 1 pixel, which is 0.33 DIPs for the mock UI. Add that to maxDeviation.
            maxDeviation += 1.0/3;
            
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
        Size scrolledAreaSize = _pMockScrollViewCore->getClientSize();

        // the bounds are rounded to pixel boundaries, so there can always be some deviation.
        // At most 1 pixel, which is 0.33 DIPs for the mock UI. Add that to maxDeviation.
        double maxDeviation = 1.0/3;

        // add a little more to account for floating point rounding errors.
        maxDeviation += 0.0001;

        REQUIRE_ALMOST_EQUAL(  scrolledAreaSize.width, expectedSize.width, maxDeviation );
        REQUIRE_ALMOST_EQUAL(  scrolledAreaSize.height, expectedSize.height, maxDeviation );
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        Size viewPortSize = _pMockScrollViewCore->getViewPortSize();
        
        // the bounds are rounded to pixel boundaries, so there can always be some deviation.
        // At most 1 pixel, which is 0.33 DIPs for the mock UI. Add that to maxDeviation.
        double maxDeviation = 1.0/3;

        REQUIRE_ALMOST_EQUAL(  viewPortSize.width, expectedSize.width, maxDeviation );
        REQUIRE_ALMOST_EQUAL(  viewPortSize.height, expectedSize.height, maxDeviation );
    }               
             
    P<bdn::test::MockScrollViewCore> _pMockScrollViewCore;
};

TEST_CASE("mock.ScrollViewCore")
{
    P<TestMockScrollViewCore> pTest = newObj<TestMockScrollViewCore>();

    pTest->runTests();
}

