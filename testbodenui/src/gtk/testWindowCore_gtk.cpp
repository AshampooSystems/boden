#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/testWindowCore.h>
#include <bdn/gtk/UiProvider.h>
#include <bdn/gtk/WindowCore.h>
#include "testGtkViewCore.h"

using namespace bdn;


bool windowExists(GtkWindow* pWindow)
{
    GList* pList = gtk_window_list_toplevels();
    
    GList* pEl = g_list_find(pList, pWindow);
    
    g_list_free(pList);
    
    return ( pEl != nullptr );
}

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
    
    SECTION("widget destroyed when object destroyed")
    {
        // there may be pending sizing info updates for the window, which keep it alive.
        // Ensure that those are done first.

        // wrap pWindow in a struct so that we can destroy all references
        // in the continuation.
        struct CaptureData : public Base
        {
            P<Window> pWindow;
        };
        P<CaptureData> pData = newObj<CaptureData>();
        pData->pWindow = pWindow;
        pWindow = nullptr;
        
        CONTINUE_SECTION_AFTER_PENDING_EVENTS(pData)
        {
            P<bdn::gtk::WindowCore> pCore = cast<bdn::gtk::WindowCore>( pData->pWindow->getViewCore() );
            REQUIRE( pCore!=nullptr );

            GtkWidget* pWidget = pCore->getGtkWidget();
            REQUIRE( pWidget!=nullptr );
            
            GtkWindow* pGtkWindow = GTK_WINDOW(pWidget);
            
            // sanity check
            REQUIRE( windowExists(pGtkWindow) );

            SECTION("core not kept alive")
            {
                pCore = nullptr;
            }

            SECTION("core kept alive")
            {
                // do nothing
            }        

            pData->pWindow = nullptr;

            // widget should have been destroyed
            REQUIRE( ! windowExists(pGtkWindow) );
        };
    }
}




