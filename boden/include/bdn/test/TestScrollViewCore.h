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

    void prepareCalcLayout(const Size& viewPortSize) override
    {
        // we must force the viewport to have the requested size.
        initiateScrollViewResizeToHaveViewPortSize(viewPortSize);

        _viewPortSizeRequestedInPrepare = viewPortSize;
    }

    void calcLayoutAfterPreparation() override
    {
        // verify that the scroll view has a plausible size.
        Size scrollViewSize = _pScrollView->size();

        REQUIRE( scrollViewSize>=_viewPortSizeRequestedInPrepare );

        // we do not know how big the viewport border is, but we assume that
        // it is less than 50 DIPs. If the following fails then it might be the
        // case that the border is actually bigger - in that case this test must be
        // adapted.
        REQUIRE( scrollViewSize <= _viewPortSizeRequestedInPrepare+Size(50,50) );
        
        // request layout explicitly again. Usually the resizing will have caused one,
        // but we want to make sure.
        return _pScrollView->needLayout( View::InvalidateReason::customDataChanged );
    }

    
    /** Causes the scroll view to have the specified viewport size when no scroll bars are shown.

        It is ok if the resizing happens asynchronously after the function has already returned.
    */
    virtual void initiateScrollViewResizeToHaveViewPortSize( const Size& viewPortSize)=0;



    P<ScrollView>       _pScrollView;
    P<IScrollViewCore>  _pScrollViewCore;

private:
    Size _viewPortSizeRequestedInPrepare;
};




}
}

#endif

