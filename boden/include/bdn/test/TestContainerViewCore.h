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
class TestContainerViewCore : public TestViewCore
{

protected:

    P<View> createView() override
    {
        return newObj<ColumnView>();
    }

    void setView(View* pView) override
    {
        TestViewCore::setView(pView);

        _pContainerView = cast<ContainerView>( pView );
    }

    void runInitTests() override
    {
        TestViewCore::runInitTests();

        // nothing containerview-specific to test here.
    }

    void runPostInitTests() override
    {
        TestViewCore::runPostInitTests();

        SECTION("preferredSize")
        {
            // container view cores do not calculate a preferred size. That is always done
            // by the other container view implementation.
            // As such, the container view core should throw exceptions when size calculation
            // is used.

            REQUIRE_THROWS_PROGRAMMING_ERROR( _pCore->calcPreferredSize() );
            REQUIRE_THROWS_PROGRAMMING_ERROR( _pCore->calcPreferredSize(Size::none(), Size(100, Size::componentNone()) ) );
            REQUIRE_THROWS_PROGRAMMING_ERROR( _pCore->calcPreferredSize(Size::none(), Size(Size::componentNone(), 100) ) );
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

    P<ContainerView> _pContainerView;
};




}
}

#endif

