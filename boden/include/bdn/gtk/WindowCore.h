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
        _pContentParentWidget = gtk_layout_new(nullptr, nullptr);
        
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
        // GTK will assume that the requested size is without any window decorations
        // and borders. That is OK, since we ignore these nonclient sizes as well.
        
        GtkAllocation alloc = rectToGtkRect(bounds, getGtkScaleFactor() );
        
        // the X window system is not always precise when it comes to sizing and positioning.
        // So if the size and/or position did not change then we should not reset it.
        // If we were to set the "current" size or position again then we would risk that
        // the result is slightly different that the actual current size and position.
        
                       
        if(bounds.getSize() != _currBounds.getSize())
            gtk_window_resize( getGtkWindow(), alloc.width, alloc.height );
        
        if(bounds.getPosition() != _currBounds.getPosition() )            
            gtk_window_move( getGtkWindow(), alloc.x, alloc.y );            
    }



	Rect getContentArea() override
    {
        // content area size is the same as our size (since window borders etc. are not
        // included in our size).
        
        Rect area( Point(0,0), _currBounds.getSize() );
        
        // note that we cannot use the current size of _pContentParentWidget, because it might
        // lag behind a little bit when the size of the overall window changes.
        // If we need to change to the _pContentParentWidget size in the future then we need
        // to react to the resizing of the content parent.
        
        /*GtkAllocation alloc;
        gtk_widget_get_allocation(_pContentParentWidget, &alloc );

        Rect area = gtkRectToRect(alloc, getGtkScaleFactor() );        */
        
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
        
        GdkWindow* pGdkWindow = gtk_widget_get_window( getGtkWidget() );
        
        gint monitorNum = gdk_screen_get_monitor_at_window( pScreen, pGdkWindow );
        
        
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
        
        gtk_layout_put( GTK_LAYOUT(_pContentParentWidget), pChildCore->getGtkWidget(), rect.x, rect.y);
    }
    
    
    
    void _moveChildViewCore(ViewCore* pChildCore, int gtkX, int gtkY) override
    {
        gtk_layout_move( GTK_LAYOUT(_pContentParentWidget), pChildCore->getGtkWidget(), gtkX, gtkY);
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
        
        _currBounds = _getBounds();
        
        // update the property. Note that this will not cause
        // another configure event, because the setBounds method will only
        // reposition or resize the window if the new size/position are different
        // from the current ones.
        getOuterView()->bounds() = _currBounds;                
    }
    
    
    static gboolean _reconfiguredCallback(    GtkWidget* pWidget,
                                            GdkEvent* pEvent,
                                            gpointer  pUser)
    {
        ((WindowCore*)pUser)->_reconfigured();
        
        return FALSE;
    }
    
    
    GtkWidget*  _pContentParentWidget;
    
    Size        _minSize;    
    Rect        _currBounds;
};



}
}

#endif
