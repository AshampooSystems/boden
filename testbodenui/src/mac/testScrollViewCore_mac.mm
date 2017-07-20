#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Dip.h>
#include <bdn/test/TestScrollViewCore.h>
#import "TestMacChildViewCoreMixin.hh"

#import <bdn/mac/UiProvider.hh>

#import <bdn/mac/ScrollViewCore.hh>


using namespace bdn;

class TestMacScrollViewCore : public bdn::test::TestMacChildViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:

    
  
    double getVertBarWidth() override
    {
        return 0;
    }


    double getHorzBarHeight() override
    {
        return 0;
    }
                

    bdn::Size getNonClientSize()
    {
        return bdn::Size(0,0);
    }

    void resizeScrollViewToViewPortSize( const bdn::Size& viewPortSize) override
    {
        // resize the scroll view so that it has exactly the desired scroll view size

        bdn::Rect newBounds( _pScrollView->position() ,viewPortSize + getNonClientSize() );

        _pScrollView->adjustAndSetBounds(newBounds);
    }
    
    void verifyHorzBarVisible( bool expectedVisible) override
    {
        REQUIRE(false);
    }

    void verifyVertBarVisible( bool expectedVisible) override
    {
        REQUIRE(false);
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
        REQUIRE(false);
       
        bdn::Size scrolledAreaSize;
       
        // add a little more to account for floating point rounding errors.
        

        REQUIRE(  Dip::equal( scrolledAreaSize, expectedSize ) );
    }

    void verifyViewPortSize( const bdn::Size& expectedSize) override
    {
        REQUIRE(false);

        bdn::Size viewPortSize;

        viewPortSize -= getNonClientSize();
        

        REQUIRE(  Dip::equal( viewPortSize, expectedSize ) );
    }               
             

};

TEST_CASE("mac.ScrollViewCore")
{
    P<TestMacScrollViewCore> pTest = newObj<TestMacScrollViewCore>();

    pTest->runTests();
}



