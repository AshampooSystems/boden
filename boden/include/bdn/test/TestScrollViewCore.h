#ifndef BDN_TEST_TestScrollViewCore_H_
#define BDN_TEST_TestScrollViewCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/ScrollView.h>
#include <bdn/IScrollViewCore.h>

#include <bdn/test/ScrollViewLayoutTesterBase.h>

namespace bdn
{
namespace test
{


        

/** Helper for tests that verify IScrollViewCore implementations.*/
class TestScrollViewCore : public bdn::test::ScrollViewLayoutTesterBase< TestViewCore<Window> >
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

        SECTION("preferred size and layout")
        {
            this->doPreferredSizeAndLayoutTests();
        }
    }



    
    P<ScrollView> getScrollView() override
    {
        return _pScrollView;
    }


    Size callCalcPreferredSize( const Size& availableSpace = Size::none() ) override
    {
        return _pScrollViewCore->calcPreferredSize( availableSpace );
    }  

    void calcLayout(const Size& viewPortSize) override
    {
        // we must force the viewport to have the requested size.
        resizeScrollViewToViewPortSize(viewPortSize);

        // then we call layout
        return _pScrollView->needLayout( View::InvalidateReason::customDataChanged );
    }

    
    /** Resizes the scroll view to have the specified viewport size when no scroll bars are shown.
    */
    virtual void resizeScrollViewToViewPortSize( const Size& viewPortSize)=0;



    P<ScrollView>       _pScrollView;
    P<IScrollViewCore>  _pScrollViewCore;
};




}
}

#endif

