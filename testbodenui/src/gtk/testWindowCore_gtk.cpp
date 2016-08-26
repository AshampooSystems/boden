#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>
#include <bdn/gtk/UiProvider.h>
#include <bdn/gtk/WindowCore.h>
#include <bdn/gtk/test/testGtkViewCore.h>

using namespace bdn;

TEST_CASE("WindowCore-gtk")
{
    P<Window> pWindow = newObj<Window>( &bdn::gtk::UiProvider::get() );

    SECTION("generic")
        bdn::test::testWindowCore(pWindow );        

    SECTION("gtk-view")
        bdn::gtk::test::testGtkViewCore(pWindow, pWindow, false);

    SECTION("gtk-window")
    {
        P<bdn::gtk::WindowCore> pCore = cast<bdn::gtk::WindowCore>( pWindow->getViewCore() );
        REQUIRE( pCore!=nullptr );

        GtkWidget* pWidget = pCore->getGtkWidget();
        REQUIRE( pWidget!=nullptr );
        

        SECTION("title")
        {
            // setTitle should change the window test
            pWindow->title() = "hello world";

            String title = gtk_window_get_title( GTK_WINDOW(pWidget) );

            REQUIRE( title == "hello world" );
        }                
    }
}




