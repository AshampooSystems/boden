#ifndef BDN_GTK_TEST_testGtkViewCore_H_
#define BDN_GTK_TEST_testGtkViewCore_H_


#include <bdn/test/testViewCore.h>

#include <bdn/View.h>
#include <bdn/gtk/ViewCore.h>

namespace bdn
{
namespace gtk
{
namespace test
{

/** Tests the gtk view core that is associated with the specified view.

    \param canCalculatePreferredSize indicates whether the view core supports calculating a preferred size.
        Some view cores (e.g. ContainerViewCore) do not support that and instead require the outer view
        object to provide the preferred size.
        */
inline void testGtkViewCore(P<Window> pWindow, P<View> pView, bool canCalculatePreferredSize)
{
    SECTION("generic")
        bdn::test::testViewCore(pWindow, pView, canCalculatePreferredSize);

    SECTION("gtk-specific")
    {
        P<bdn::gtk::ViewCore> pCore = cast<bdn::gtk::ViewCore>( pView->getViewCore() );
        REQUIRE( pCore!=nullptr );

        GtkWidget* pWidget = pCore->getGtkWidget();
        REQUIRE( pWidget!=nullptr );

        SECTION("visible")
        {
            
            REQUIRE( (gtk_widget_get_visible(pWidget)!=FALSE) == pView->visible().get() );
            
            pView->visible() = true;
            REQUIRE( (gtk_widget_get_visible(pWidget)!=FALSE) );

            pView->visible() = false;
            REQUIRE( (gtk_widget_get_visible(pWidget)==FALSE) );

            pView->visible() = true;
            REQUIRE( (gtk_widget_get_visible(pWidget)!=FALSE) );
        }

        
        SECTION("padding")
        {
            // padding is not translated to a win32 HWND property.
            // So there is nothing to test for it.
        }

        SECTION("bounds")
        {
            // bounds should translate 1:1
            pView->bounds() = Rect(110, 220, 880, 990);
            
            if( tryCast<Window>(pView)!=nullptr )            
            {                
                gint width;
                gint height;                        
                gtk_window_get_size( GTK_WINDOW(pWidget), &width, &height);
                
                REQUIRE( width == 880 );
                REQUIRE( height == 990 );
                
                gint x;
                gint y;
                gtk_window_get_position( GTK_WINDOW(pWidget), &x, &y );
                
                REQUIRE( x == 110 );
                REQUIRE( y == 220 );
            }
            else            
            {
                gint width;
                gint height;            
                gtk_widget_get_size_request( pWidget, &width, &height);
                
                REQUIRE( width == 880 );
                REQUIRE( height == 990 );
                
                // apparently there is no generic way to verify the position for Gtk Widgets.
            }            
        }
    }
}


}
}
}


#endif

