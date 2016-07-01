#ifndef BDN_GTK_WindowCore_H_
#define BDN_GTK_WindowCore_H_

#include <bdn/gtk/ViewCore.h>
#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <gtk/gtk.h>

namespace bdn
{
namespace gtk
{
    

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore
{
public:
	WindowCore(View* pOuter)
        : ViewCore(pOuter, gtk_window_new(GTK_WINDOW_TOPLEVEL) )
	{        
        setTitle( cast<Window>(pOuter)->title() );
        
        // we add a dummy content window, so that we can manually position our "real" content view inside it.
        _pContentParentWidget = gtk_fixed_new();
        
        g_signal_connect( getGtkWidget(), "configure-event", G_CALLBACK(_reconfiguredCallback), this );
        
        gtk_container_add( GTK_CONTAINER( getGtkWidget() ), _pContentParentWidget );
        
        gtk_widget_show( _pContentParentWidget );
        
        // we set the "minimum size" to get widgets to be a certain size.
        // For GTK that means that it will not let the window get shrunk to less than the current size
        // of its content.
        // To override this we have to set geometry hints for the window.
        
        
        // first determine the minimum size.
        GtkRequisition gtkMinSize;
        GtkRequisition gtkNaturalSize;
        
        gtk_widget_get_preferred_size( getGtkWidget(), &gtkMinSize, &gtkNaturalSize );
        
        _minSize = gtkSizeToSize(gtkMinSize, getGtkScaleFactor() );
        
        
        GdkGeometry geo{};
        geo.min_width = gtkNaturalSize.width;
        geo.min_height = gtkNaturalSize.height;

        gtk_window_set_geometry_hints( GTK_WINDOW(getGtkWidget()), NULL, &geo, GDK_HINT_MIN_SIZE );
	}

	~WindowCore()
	{
        // windows must be destroyed explicitly. Child widgets are destroyed
        // automatically.
        GtkWidget* pWidget = getGtkWidget();
		if(pWidget!=nullptr)
            gtk_widget_destroy( pWidget );
	}
    
    
    void setTitle(const String& title) override
    {
        gtk_window_set_title( getGtkWindow(), title.asUtf8Ptr() );
    }


    void setBounds(const Rect& bounds)
    {   
        // if _ignoreSetBounds is true then we are the ones who have set the bounds as a reaction to a size change event.
        // Ignore this, so that we do not get in a loop.
        if(!_ignoreSetBounds)
        {        
            // GTK will assume that the requested size is without any window decorations
            // and borders. That is OK, since we ignore these nonclient sizes as well.
            
            GtkAllocation alloc = rectToGtkRect(bounds, getGtkScaleFactor() );
            
            gtk_window_resize( getGtkWindow(), alloc.width, alloc.height );
            
            gtk_window_move( getGtkWindow(), alloc.x, alloc.y );        
        }
        
        getOuterView()->needLayout();
    }



	Rect getContentArea() override
    {
        GtkAllocation alloc;
        gtk_widget_get_allocation(_pContentParentWidget, &alloc );

        Rect area = gtkRectToRect(alloc, getGtkScaleFactor() );        
        
        return area;
    }


	Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
    {
        // the "window size" in GTK is the size without borders and window decorations.
        // So the window size is equal to the content size.
        return contentSize;
    }


	Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
    {
        // the "window size" in GTK is the size without borders and window decorations.
        // So the window size is equal to the content size.
        return windowSize;
    }


	Size calcMinimumSize() const override
    {
        return _minSize;
    }

	
	Rect getScreenWorkArea() const
    {
        GdkScreen* pScreen = gtk_window_get_screen( getGtkWindow() );
        
        gint monitorNum = gdk_screen_get_monitor_at_window( pScreen, GDK_WINDOW( getGtkWidget() ) );
        
        
        GdkRectangle workArea;
        gdk_screen_get_monitor_workarea( pScreen, monitorNum, &workArea );
        
        return gtkRectToRect(workArea, getGtkScaleFactor() );        
    }
	
    GtkWindow* getGtkWindow() const
    {
        return GTK_WINDOW( getGtkWidget() );
    }
    

    
    
    void _addChildViewCore(ViewCore* pChildCore) override
    {
        Rect bounds = pChildCore->getOuterView()->bounds();
        
        GdkRectangle rect = rectToGtkRect(bounds, getGtkScaleFactor() );
        
        gtk_fixed_put( GTK_FIXED(_pContentParentWidget), pChildCore->getGtkWidget(), rect.x, rect.y);
    }
    
    
    
    void _moveChildViewCore(ViewCore* pChildCore, int gtkX, int gtkY) override
    {
        gtk_fixed_move( GTK_FIXED(_pContentParentWidget), pChildCore->getGtkWidget(), gtkX, gtkY);
    }    

    
protected:
    Rect _getBounds()
    {
        gint x;
        gint y;        
        gtk_window_get_position( GTK_WINDOW( getGtkWidget() ), &x, &y );
        
        gint width;
        gint height;
        gtk_window_get_size( GTK_WINDOW( getGtkWidget() ), &width, &height );
        
        // note that we get the size and position without window borders and decorations here.
        // That is ok, since we completely ignore the nonclient area of the window and pretend
        // that it is 0 anyway. All GTK functions work that way, so that is OK.
        
        GdkRectangle rect;
        rect.x = x;
        rect.y = y;
        rect.width = width;
        rect.height = height;
        
        return gtkRectToRect( rect, getGtkScaleFactor() );
    }
    
    void _reconfigured()
    {
        // size, position or "stacking" has changed. Update the bounds() property
        
        _ignoreSetBounds = true;
        
        try
        {        
            getOuterView()->bounds() = _getBounds();        
        }
        catch(...)
        {
            _ignoreSetBounds = false;
            throw;
        }
        
        _ignoreSetBounds = false;
    }
    
    
    static gboolean _reconfiguredCallback(    GtkWidget* pWidget,
                                            GdkEvent* pEvent,
                                            gpointer  pUser)
    {
        ((WindowCore*)pUser)->_reconfigured();
        
        return FALSE;
    }
    
    
    GtkWidget*  _pContentParentWidget;
    
    bool        _ignoreSetBounds = false;
    Size        _minSize;
};



}
}

#endif
