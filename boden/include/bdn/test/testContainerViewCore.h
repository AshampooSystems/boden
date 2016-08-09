#ifndef BDN_TEST_testContainerViewCore_H_
#define BDN_TEST_testContainerViewCore_H_

#include <bdn/test/testViewCore.h>
#include <bdn/ContainerView.h>

namespace bdn
{
namespace test
{


/** Performs generic tests for the container view core that is currently associated with the specified ContainerView.

    Note that these tests cannot test the effects of some of the functions on the actual UI element
    implementation that the core accesses. So the unit tests for the concrete implementation should verify these
    effects in addition to executing these generic tests.
*/
inline void testContainerViewCore(ContainerView* pView)
{
    // note that there currently is no IContainerViewCore
    P<IViewCore> pCore = pView->getViewCore();
    REQUIRE( pCore!=nullptr );

    SECTION("ViewCore-base")
        testViewCore(pView, false);

    SECTION("preferredSize")
    {
        // container view cores do not calculate a preferred size. That is always done
        // by the other container view implementation.
        // As such, the container view core should throw exceptions when size calculation
        // is used.

        REQUIRE_THROWS_PROGRAMMING_ERROR( pCore->calcPreferredSize() );
        REQUIRE_THROWS_PROGRAMMING_ERROR( pCore->calcPreferredWidthForHeight(100) );
        REQUIRE_THROWS_PROGRAMMING_ERROR( pCore->calcPreferredHeightForWidth(100) );
    }
}

}
}

#endif

