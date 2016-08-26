#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/test/testContainerViewCore.h>

#include <bdn/gtk/UiProvider.h>
#include <bdn/gtk/ContainerViewCore.h>
#include <bdn/gtk/test/testGtkViewCore.h>

using namespace bdn;

TEST_CASE("ContainerViewCore-gtk")
{
    P<Window> pWindow = newObj<Window>();

    // ContainerView is an abstract base class, so we cannot instantiate it.
    // But we can use any ContainerView subclass for these tests because
    // they all use the same core.

    P<ColumnView> pColumnView = newObj<ColumnView>();

    pWindow->setContentView(pColumnView);

    SECTION("generic")
        bdn::test::testContainerViewCore(pWindow, pColumnView );        

    SECTION("gtk-ViewCore")
        bdn::gtk::test::testGtkViewCore(pWindow, pColumnView, false);

    SECTION("gtk-ContainerViewCore")
    {
        P<bdn::gtk::ContainerViewCore> pCore = cast<bdn::gtk::ContainerViewCore>( pColumnView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        GtkWidget* pWidget = pCore->getGtkWidget();
        REQUIRE( pWidget!=nullptr );

        // there is nothing gtk-specific to test here.
        // The generic container view tests and the gtk view test have already tested
        // everything that the container view core does.        
    }
}




