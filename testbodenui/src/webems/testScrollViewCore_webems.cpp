#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestScrollViewCore.h>
#include "TestWebemsViewCoreMixin.h"


using namespace bdn;

class TestWebemsScrollViewCore : public bdn::test::TestWebemsViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestWebemsViewCoreMixin< bdn::test::TestScrollViewCore >::initCore();

        _pWebScrollViewCore = cast<bdn::webems::ScrollViewCore>( _pWebCore );
    }
    
  
    double getVertBarWidth() override
    {
        double vertBarWidth=-1;
        double horzBarHeight=-1;
        _pWebScrollViewCore->getScrollBarLayoutSpaces(vertBarWidth, horzBarHeight);

        return vertBarWidth;
    }


    double getHorzBarHeight() override
    {
        double vertBarWidth=-1;
        double horzBarHeight=-1;
        _pWebScrollViewCore->getScrollBarLayoutSpaces(vertBarWidth, horzBarHeight);

        return horzBarHeight;
    }
                
    
    void runPostInitTests() override
    {
        P<TestWebemsScrollViewCore> pThis = this;
                
        SECTION("scrollbar layout spaces")
        {
            double vertBarWidth=-1;
            double horzBarHeight=-1;
            _pWebScrollViewCore->getScrollBarLayoutSpaces(vertBarWidth, horzBarHeight);

            // verify that the scrollbar sizes are in a reasonable range.
            // Note that they are in DIPs, so we can take some guess as to what 
            // is reasonable.
            REQUIRE( vertBarWidth>0 )
            REQUIRE( vertBarWidth<=50 );

            REQUIRE( horzBarHeight>0 )
            REQUIRE( horzBarHeight<=50 );
        }
        
        bdn::test::TestWebemsViewCoreMixin< bdn::test::TestScrollViewCore >::runPostInitTests();
    }
    


    void initiateScrollViewResizeToHaveViewPortSize( const Size& viewPortSize)
    {
        // resize the scroll view so that it has exactly the desired scroll view size

        bdn::Rect newBounds( _pScrollView->position(), viewPortSize );

        _pScrollView->adjustAndSetBounds(newBounds);
    }

    Size getScrollAreaSizePixels()
    {
        REQUIRE( false );
    }
    
    void verifyScrollsHorizontally( bool expectedScrolls ) override
    {
        REQUIRE( false );
    }

    void verifyScrollsVertically( bool expectedScrolls) override
    {
        REQUIRE( false );
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
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

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        REQUIRE( false );
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        REQUIRE( false );

private:
    P<bdn::webems::ScrollViewCore> _pWebScrollViewCore;
};




TEST_CASE("webems.ScrollViewCore")
{
    P<TestWebemsScrollViewCore> pTest = newObj<TestWebemsScrollViewCore>();

    pTest->runTests();
}



