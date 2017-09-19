#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/gtk/UiProvider.h>
#include <bdn/gtk/ScrollViewCore.h>
#include "TestGtkViewCoreMixin.h"


using namespace bdn;

class TestGtkScrollViewCore : public bdn::test::TestGtkViewCoreMixin< bdn::test::TestScrollViewCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestGtkViewCoreMixin< bdn::test::TestScrollViewCore >::initCore();
        
        _pGtkScrolledWindow = GTK_SCROLLED_WINDOW( _pGtkWidget );
        _pGtkScrolledArea = gtk_bin_get_child( GTK_BIN(_pGtkWidget) );
    }
    
    void verifyScrollBarLayoutSpaces(bool overlayAllowed)
    {
        int vertBarSpaceWidth=-1;
        int horzBarSpaceHeight=-1;
        bdn::gtk::ScrollViewCore::getScrollBarLayoutSpaces(_pGtkScrolledWindow, vertBarSpaceWidth, horzBarSpaceHeight );
        
                
        if(overlayAllowed)
        {        
            REQUIRE( vertBarSpaceWidth>=0 );
            REQUIRE( horzBarSpaceHeight>=0 );
        
            // overlay scrolling should be active for BOTH scrollbars or none of them.
            REQUIRE( (vertBarSpaceWidth==0) == (horzBarSpaceHeight==0) );
        }
        else
        {
            // overlay is not allowed, so the scrollbars MUST take up actual layout space
            REQUIRE( vertBarSpaceWidth>0 );
            REQUIRE( horzBarSpaceHeight>0 );            
        }
        
        // if the scrollbars take up space then the scrollbar spacing must be included
        if(vertBarSpaceWidth>0)
        {
            GtkWidget* pHBar = gtk_scrolled_window_get_hscrollbar( GTK_SCROLLED_WINDOW(_pGtkScrolledWindow) );
            GtkWidget* pVBar = gtk_scrolled_window_get_vscrollbar( GTK_SCROLLED_WINDOW(_pGtkScrolledWindow) );
            
            REQUIRE( pHBar!=nullptr );
            REQUIRE( pVBar!=nullptr );
            
            gint minHorzBarHeight=0;
            gint naturalHorzBarHeight=0;
            gtk_widget_get_preferred_height ( pHBar, &minHorzBarHeight, &naturalHorzBarHeight );
            
            gint minVertBarWidth=0;
            gint naturalVertBarWidth=0;
            gtk_widget_get_preferred_width ( pVBar, &minVertBarWidth, &naturalVertBarWidth );
            
            int scrollBarSpacing = bdn::gtk::ScrollViewCore::getScrollBarSpacing( _pGtkScrolledWindow );
            
            REQUIRE( vertBarSpaceWidth == naturalVertBarWidth + scrollBarSpacing );
            REQUIRE( horzBarSpaceHeight == minHorzBarHeight + scrollBarSpacing );                        
        }        
    }
    
    void runPostInitTests() override
    {
        P<TestGtkScrollViewCore> pThis = this;
        
        SECTION("scrollbar spacing")
        {
            GtkScrolledWindowClass* pScrollClass = GTK_SCROLLED_WINDOW_GET_CLASS( _pGtkScrolledWindow );
            
            int oldVal = pScrollClass->scrollbar_spacing;
            
            try
            {          
                SECTION("class scrollbar_spacing = 0")
                {
                    pScrollClass->scrollbar_spacing = 0;
                    
                    // the class value apparently always override any CSS style.
                    // It is unclear if that is a GTK bug, but at the time of this writing
                    // that is how GTK behaves, so that is what our code should return.
                    
                    int spacing = bdn::gtk::ScrollViewCore::getScrollBarSpacing(_pGtkScrolledWindow);
                    REQUIRE( spacing == 0 );
                }
                
                SECTION("class scrollbar_spacing > 0")
                {
                    pScrollClass->scrollbar_spacing = 7;
                    
                    // the class value apparently always override any CSS style.
                    // It is unclear if that is a GTK bug, but at the time of this writing
                    // that is how GTK behaves, so that is what our code should return.
                    
                    int spacing = bdn::gtk::ScrollViewCore::getScrollBarSpacing(_pGtkScrolledWindow);
                    REQUIRE( spacing == 7 );
                }
        
            }
            catch(...)
            {
                pScrollClass->scrollbar_spacing = oldVal;
                throw;
            }
            
            pScrollClass->scrollbar_spacing = oldVal;            
        }
        
        SECTION("scrollbar layout spaces")
        {
            SECTION("overlayScrolling=TRUE")
            {
                gtk_scrolled_window_set_overlay_scrolling( _pGtkScrolledWindow, TRUE );
                
                BDN_CONTINUE_SECTION_WHEN_IDLE( pThis )
                {
                    // overlayScrolling = TRUE means that the scrolled window is ALLOWED
                    // to use overlay scrollbars. The documentation states that it might
                    // do so when no mouse is connected.    
                    
                    pThis->verifyScrollBarLayoutSpaces(true);
                };
            }
            
            SECTION("overlayScrolling=FALSE")
            {
                gtk_scrolled_window_set_overlay_scrolling( _pGtkScrolledWindow, FALSE );
                
                BDN_CONTINUE_SECTION_WHEN_IDLE( pThis )
                {
                    // overlayScrolling = FALSE means that overlay scrolling is forbidden.
                    // So the scrollbars MUST take up space.
                    
                    pThis->verifyScrollBarLayoutSpaces(false);
                };
            }            
            
        }
        
        bdn::test::TestGtkViewCoreMixin< bdn::test::TestScrollViewCore >::runPostInitTests();
    }
    
    
  
    double getVertBarWidth()
    {
        int vertBarSpaceWidth=-1;
        int horzBarSpaceHeight=-1;
        bdn::gtk::ScrollViewCore::getScrollBarLayoutSpaces(_pGtkScrolledWindow, vertBarSpaceWidth, horzBarSpaceHeight );
        
        return vertBarSpaceWidth;
    }


    double getHorzBarHeight()
    {
        int vertBarSpaceWidth=-1;
        int horzBarSpaceHeight=-1;
        bdn::gtk::ScrollViewCore::getScrollBarLayoutSpaces(_pGtkScrolledWindow, vertBarSpaceWidth, horzBarSpaceHeight );
        
        return horzBarSpaceHeight;
    }
                
                
    Size initiateScrollViewResizeToHaveViewPortSize( const Size& viewPortSize)
    {
        Size adjustedSize = _pScrollView->adjustBounds( Rect( _pScrollView->position(), viewPortSize), RoundType::nearest, RoundType::nearest ).getSize();

        _pScrollView->preferredSizeMinimum() = adjustedSize;
        _pScrollView->preferredSizeMaximum() = adjustedSize;
        
        _pWindow->requestAutoSize();

        return adjustedSize;
    }

    void verifyScrollsHorizontally( bool expectedScrolls ) override
    {
        // we cannot check the scrollbars, since they might only be shown
        // as overlays during scroll operations.
        // Instead we check if the size of the scrolled area exceeds the viewport size.
        
        Size scrolledAreaSize = getScrolledAreaSize();
        Size viewPortSize = getViewPortSize();
        
        bool scrolls = (scrolledAreaSize.width > viewPortSize.width);
        REQUIRE( scrolls==expectedScrolls );
    }

    void verifyScrollsVertically( bool expectedScrolls) override
    {
        Size scrolledAreaSize = getScrolledAreaSize();
        Size viewPortSize = getViewPortSize();
        
        bool scrolls = (scrolledAreaSize.height > viewPortSize.height);
        REQUIRE( scrolls==expectedScrolls );
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        maxDeviation += Dip::significanceBoundary();

        P<View> pContentView =  _pScrollView->getContentView();

        if(pContentView!=nullptr)
        {
            bdn::Rect bounds( _pScrollView->getContentView()->position(), pContentView->size() );
            
            if(maxDeviation==0)
                REQUIRE( bounds == expectedBounds );
            else
            {
                REQUIRE_ALMOST_EQUAL(  bounds.x, expectedBounds.x, maxDeviation );
                REQUIRE_ALMOST_EQUAL(  bounds.y, expectedBounds.y, maxDeviation );
                REQUIRE_ALMOST_EQUAL(  bounds.width, expectedBounds.width, maxDeviation );
                REQUIRE_ALMOST_EQUAL(  bounds.height, expectedBounds.height, maxDeviation );
            }
        }
    }

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        Size scrolledAreaSize = getScrolledAreaSize();
                
        REQUIRE( scrolledAreaSize == expectedSize);
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        Size viewPortSize = getViewPortSize();
                
        REQUIRE( viewPortSize == expectedSize);
    }               


    Size getScrolledAreaSize()
    {
        guint scrolledWidth;
        guint scrolledHeight;
        gtk_layout_get_size( GTK_LAYOUT(_pGtkScrolledArea), &scrolledWidth, &scrolledHeight);
        
        return Size(scrolledWidth, scrolledHeight);
    }
    
    Size getViewPortSize()
    {   
        // the scrolled window does not have a border, so the viewport size
        // is the whole view size.
        
        gint width;
        gint height;
        gtk_widget_get_size_request( _pGtkWidget, &width, &height);
        
        return Size(width, height);
    }

protected:
    GtkScrolledWindow* _pGtkScrolledWindow;
    GtkWidget*         _pGtkScrolledArea;
};




TEST_CASE("gtk.ScrollViewCore")
{
    P<TestGtkScrollViewCore> pTest = newObj<TestGtkScrollViewCore>();

    pTest->runTests();
}



