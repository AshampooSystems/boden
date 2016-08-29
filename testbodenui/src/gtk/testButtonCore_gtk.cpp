#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/testButtonCore.h>
#include <bdn/gtk/UiProvider.h>
#include <bdn/gtk/ButtonCore.h>
#include "testGtkViewCore.h"

using namespace bdn;

TEST_CASE("ButtonCore-gtk")
{
    P<Window> pWindow = newObj<Window>( &bdn::gtk::UiProvider::get() );

    P<Button> pButton = newObj<Button>();

    pWindow->setContentView(pButton);

    SECTION("generic")
        bdn::test::testButtonCore(pWindow, pButton );        

    SECTION("gtk-view")
        bdn::gtk::test::testGtkViewCore(pWindow, pButton, true);

    SECTION("gtk-button")
    {
        P<bdn::gtk::ButtonCore> pCore = cast<bdn::gtk::ButtonCore>( pButton->getViewCore() );
        REQUIRE( pCore!=nullptr );
        
        GtkWidget* pWidget = pCore->getGtkWidget();
        REQUIRE( pWidget!=nullptr );

        SECTION("label")
        {
            // setLabel should change the window test
            pButton->label() = "hello world";
            
            String label = gtk_button_get_label( GTK_BUTTON(pWidget) );

            REQUIRE( label == "hello world" );
        }                
    }
}




