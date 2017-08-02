#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/gtk/UiProvider.h>
#include "TestGtkViewCoreMixin.h"


using namespace bdn;

class TestGtkScrollViewCore : public bdn::test::TestGtkViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestGtkViewCoreMixin< bdn::test::TestScrollViewCore >::initCore();
    }
    
  
    double getVertBarWidth()
    {
        REQUIRE(false);
        return 0;
    }


    double getHorzBarHeight()
    {
        REQUIRE(false);
        return 0;
    }
                
                
    void initiateScrollViewResizeToHaveViewPortSize( const Size& viewPortSize)
    {
        REQUIRE(false);
        // resize the scroll view so that it has exactly the desired scroll view size
        bdn::Rect newBounds( _pScrollView->position(), viewPortSize );

        _pScrollView->adjustAndSetBounds(newBounds);
    }

    void verifyScrollsHorizontally( bool expectedScrolls ) override
    {
        REQUIRE(false);
    }

    void verifyScrollsVertically( bool expectedScrolls) override
    {
        REQUIRE(false);
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        REQUIRE(false);
    }

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        REQUIRE(false);
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        REQUIRE(false);
    }               



};




TEST_CASE("gtk.ScrollViewCore")
{
    P<TestGtkScrollViewCore> pTest = newObj<TestGtkScrollViewCore>();

    pTest->runTests();
}



