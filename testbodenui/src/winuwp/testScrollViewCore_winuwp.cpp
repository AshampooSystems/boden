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
        // XXX
    }
    
    void verifyScrollsHorizontally( bool expectedVisible) override
    {
        // XXX
    }

    void verifyScrollsVertically( bool expectedVisible) override
    {
        // XXX
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        // XXX
    }

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        // XXX
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        // XXX
    }               
             
    void verifyCorePadding() override
    {
        // XXX
    }


};

TEST_CASE("winuwp.ScrollViewCore")
{
    P<TestWinuwpScrollViewCore> pTest = newObj<TestWinuwpScrollViewCore>();

    pTest->runTests();
}

