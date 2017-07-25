#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/android/UiProvider.h>
#include "TestAndroidViewCoreMixin.h"


using namespace bdn;

class TestAndroidScrollViewCore : public bdn::test::TestAndroidViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin< bdn::test::TestScrollViewCore >::initCore();
    }
    
  
    double getVertBarWidth()
    {
        // scroll bars are overlays that do not take up layout space
        return 0;
    }


    double getHorzBarHeight()
    {
        // scroll bars are overlays that do not take up layout space
        return 0;
    }
                

    void resizeScrollViewToViewPortSize( const Size& viewPortSize)
    {
        // resize the scroll view so that it has exactly the desired scroll view size

        bdn::Rect newBounds( _pScrollView->position(), viewPortSize );

        _pScrollView->adjustAndSetBounds(newBounds);
    }
    
    void verifyScrollsHorizontally( bool expectedScrolls ) override
    {
        // the view will scroll if the width of the content area is bigger than the width of the
        // scroll view.

        // XXX
        REQUIRE( false );
    }

    void verifyScrollsVertically( bool expectedScrolls) override
    {
        // XXX
        REQUIRE( false );
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        // XXX
        REQUIRE( false );
    }

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        // XXX
        REQUIRE( false );
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        // XXX
        REQUIRE( false );
    }               
             
    void verifyCorePadding() override
    {
        // XXX
        REQUIRE( false );
    }


};




TEST_CASE("android.ScrollViewCore")
{
    P<TestAndroidScrollViewCore> pTest = newObj<TestAndroidScrollViewCore>();

    pTest->runTests();
}



