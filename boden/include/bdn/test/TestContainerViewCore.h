#ifndef BDN_TEST_TestContainerViewCore_H_
#define BDN_TEST_TestContainerViewCore_H_

#include <bdn/test/TestViewCore.h>
#include <bdn/ContainerView.h>
#include <bdn/ColumnView.h>

namespace bdn
{
namespace test
{




/** Helper for tests that verify IContainerViewCore implementations.*/
class TestContainerViewCore : public TestViewCore<ContainerView>
{

protected:
    
    P<View> createView() override
    {
        return newObj<TestContainerView>();
    }

    void setView(View* pView) override
    {
        TestViewCore::setView(pView);

        _pContainerView = cast<TestContainerView>( pView );
    }

    void runInitTests() override
    {
        TestViewCore::runInitTests();

        // nothing containerview-specific to test here.
    }

    void runPostInitTests() override
    {
        TestViewCore::runPostInitTests();
        
        P<TestContainerViewCore> pThis = this;

        SECTION("calcPreferredSize forwarded to outer")
        {
            // container view cores should call calcContainerPreferredSize from the outer object

            int initialCount = _pContainerView->getCalcContainerPreferredSizeCount();

            SECTION("infinite availableSpace")
            {
                _pCore->calcPreferredSize();

                REQUIRE( _pContainerView->getCalcContainerPreferredSizeCount() == initialCount+1 );
                REQUIRE( _pContainerView->getLastCalcContainerPreferredSizeAvailableSpace() == Size::none() );
            }

            SECTION("finite availableSpace")
            {
                _pCore->calcPreferredSize( Size(100, 200) );

                REQUIRE( _pContainerView->getCalcContainerPreferredSizeCount() == initialCount+1 );
                REQUIRE( _pContainerView->getLastCalcContainerPreferredSizeAvailableSpace() == Size(100, 200) );
            }
        }

        SECTION("layout forwarded to outer")
        {
            int initialCount = _pContainerView->getCalcContainerLayoutCount();
            
            _pContainerView->needLayout( View::InvalidateReason::customDataChanged );


            CONTINUE_SECTION_WHEN_IDLE( pThis, this, initialCount)
            {
                int currLayoutCount = _pContainerView->getCalcContainerLayoutCount();
                REQUIRE( currLayoutCount == initialCount+1 );
                REQUIRE( _pContainerView->getLastCalcContainerLayoutContainerSize() == _pContainerView->size() );
            };
        }
    }

    void verifyCorePadding() override
    {
        // padding is not reflected by the container view core. It is managed entirely by the outer view.
        // So nothing to check.       
    }

    bool coreCanCalculatePreferredSize() override
    {
        return false;
    }

private:
    class TestContainerView : public ColumnView
    {
    public:

        Size calcContainerPreferredSize( const Size& availableSpace ) const override
        {
            _calcContainerPreferredSizeCount++;
            _lastCalcContainerPreferredSizeAvailableSpace = availableSpace;

            return ColumnView::calcContainerPreferredSize(availableSpace);
        }

        int getCalcContainerPreferredSizeCount() const        
        {
            return _calcContainerPreferredSizeCount;
        }

        Size getLastCalcContainerPreferredSizeAvailableSpace() const
        {
            return _lastCalcContainerPreferredSizeAvailableSpace;
        }


        P<ViewLayout> calcContainerLayout( const Size& containerSize ) const override
        {
            _calcContainerLayoutCount++;
            _lastCalcContainerLayoutContainerSize = containerSize;

            return ColumnView::calcContainerLayout(containerSize);
        }

        int getCalcContainerLayoutCount() const        
        {
            return _calcContainerLayoutCount;
        }

        Size getLastCalcContainerLayoutContainerSize() const
        {
            return _lastCalcContainerLayoutContainerSize;
        }

    private:
        mutable int _calcContainerPreferredSizeCount = 0;
        mutable Size _lastCalcContainerPreferredSizeAvailableSpace;

        mutable int _calcContainerLayoutCount = 0;
        mutable Size _lastCalcContainerLayoutContainerSize;
    };

    P<TestContainerView> _pContainerView;
};




}
}

#endif

