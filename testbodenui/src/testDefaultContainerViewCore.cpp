#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ColumnView.h>
#include <bdn/Window.h>
#include <bdn/test/testContainerViewCore.h>

using namespace bdn;

// generic tests for the default button core implementation
TEST_CASE("ContainerViewCore-default")
{
    P<Window> pWindow = newObj<Window>();

    // ContainerView is an abstract base class, so we cannot instantiate it.
    // But we can use any ContainerView subclass for these tests because
    // they all use the same core.

    P<ColumnView> pColumnView = newObj<ColumnView>();

    pWindow->setContentView(pColumnView);

    bdn::test::testContainerViewCore(pWindow, pColumnView );        
}




