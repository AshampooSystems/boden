#ifndef BDN_TEST_TestScrollViewCore_H_
#define BDN_TEST_TestScrollViewCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>

namespace bdn
{
namespace test
{
    

/** Helper for tests that verify IScrollViewCore implementations.*/
class TestScrollViewCore : public TestViewCore<Window>
{

protected:


    P<View> createView() override
    {
        P<ScrollView> pScrollView = newObj<ScrollView>();
                
        return pScrollView;
    }

    void initCore() override
    {
        TestViewCore::initCore();
        
        _pScrollViewCore = cast<IScrollViewCore>( _pCore );
    }

    void setView(View* pView) override
    {
        TestViewCore::setView(pView);

        _pScrollView = cast<ScrollView>( pView );
    }

    void runInitTests() override
    {
        TestViewCore::runInitTests();
    }

    void runPostInitTests() override
    {
        TestViewCore::runPostInitTests();

    }


    P<ScrollView>       _pScrollView;
    P<IScrollViewCore>  _pScrollViewCore;
};




}
}

#endif

