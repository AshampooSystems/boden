#ifndef BDN_GTK_WindowCore_H_
#define BDN_GTK_WindowCore_H_

#include <bdn/gtk/ViewCore.h>
#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <bdn/windowCoreUtil.h>

#include <gtk/gtk.h>

namespace bdn
{
namespace gtk
{
    

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore, BDN_IMPLEMENTS LayoutCoordinator::IWindowCoreExtension
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
        
        _minSize = gtkSizeToSize(gtkMinSize );
        
        
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


    Rect adjustAndSetBounds(const Rect& requestedBounds)
    {
        if(_inReconfigured)
        {
            // adjustAndSetBounds is currently called in the context of a _reconfigured
            // event. That is just done to notify the outer view of the new bounds and no change
            // is actually intended. Just return the current bounds as the "adjusted" bounds.
            return _currBounds;
        }
        
        Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);
        
        GtkAllocation alloc = rectToGtkRect( adjustedBounds );
        
        // the X window system is not always precise when it comes to sizing and positioning.
        // So if the size and/or position did not change then we should not reset it.
        // If we were to set the "current" size or position again then we would risk that
        // the result is slightly different that the actual current size and position.
        
        
        if(adjustedBounds.getPosition() != _currBounds.getPosition() )            
            gtk_window_move( getGtkWindow(), alloc.x, alloc.y );            
            
        if(adjustedBounds.getSize() != _currBounds.getSize())
            gtk_window_resize( getGtkWindow(), alloc.width, alloc.height );
            
            
        this->_currBounds = adjustedBounds;
        
        Rect testy = _currBounds;
        
        return adjustedBounds;
    }
    
    
    
    
    Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
            return defaultWindowCalcPreferredSizeImpl( pWindow, availableSpace, Margin(), Size() );
        else
            return Size(0,0);
    }
    
    void layout() override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
        {
            gint width;
            gint height;
            gtk_window_get_size( GTK_WINDOW( getGtkWidget() ), &width, &height );        
            // note that we get the size and position without window borders and decorations here.
            // That is ok, since we completely ignore the nonclient area of the window and pretend
            // that it is 0 anyway. All GTK functions work that way, so that is OK.
            
            GdkRectangle rect;
            rect.x = 0;
            rect.y = 0;
            rect.width = width;
            rect.height = height;
        
            Rect contentArea = gtkRectToRect( rect );
            
            defaultWindowLayoutImpl( pWindow, contentArea );
        }
    }
    
    
    
    
    void requestAutoSize() override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
        {
            P<UiProvider> pProvider = tryCast<UiProvider>( pWindow->getUiProvider() );
            if(pProvider!=nullptr)
                pProvider->getLayoutCoordinator()->windowNeedsAutoSizing( pWindow );
        }
    }
    
    void requestCenter() override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
        {
            P<UiProvider> pProvider = tryCast<UiProvider>( pWindow->getUiProvider() );
            if(pProvider!=nullptr)
                pProvider->getLayoutCoordinator()->windowNeedsCentering( pWindow );
        }
    }
    
    
    void autoSize() override
    {
        // we cannot change our size. So, do nothing
    }
    
    void center() override
    {
        // we cannot change our position. So, do nothing.
    }



	
	
    GtkWindow* getGtkWindow() const
    {
        return GTK_WINDOW( getGtkWidget() );
    }
    

    
    
    void _addChildViewCore(ViewCore* pChildCore) override
    {
        Point position;
        P<View> pChildView = pChildCore->getOuterViewIfStillAttached();
        if(pChildView!=nullptr)
            position = pChildView->position();
        
        GdkRectangle rect = rectToGtkRect( Rect(position, Size(1,1) ) );
        
        gtk_layout_put( GTK_LAYOUT(_pContentParentWidget), pChildCore->getGtkWidget(), rect.x, rect.y);
    }
    
    
    
    void _moveChildViewCore(ViewCore* pChildCore, int gtkX, int gtkY) override
    {
        gtk_layout_move( GTK_LAYOUT(_pContentParentWidget), pChildCore->getGtkWidget(), gtkX, gtkY);
    }    

    
protected:
    
	Rect getScreenWorkArea() const
    {
        GdkScreen* pScreen = gtk_window_get_screen( getGtkWindow() );
        
        GdkWindow* pGdkWindow = gtk_widget_get_window( getGtkWidget() );
        
        gint monitorNum;
        
        if(pGdkWindow!=nullptr)
            monitorNum = gdk_screen_get_monitor_at_window( pScreen, pGdkWindow );
        else
        {
            // widget is not realized. Just return the default monitor
            monitorNum = gdk_screen_get_primary_monitor( pScreen );
        }
        
        
        GdkRectangle workArea;
        gdk_screen_get_monitor_workarea( pScreen, monitorNum, &workArea );
        
        return gtkRectToRect(workArea );        
    }

	Size getMinimumSize() const
    {
        return _minSize;
    }


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
        
        return gtkRectToRect( rect );
    }
    
    void _reconfigured()
    {
        _inReconfigured = true;
        
        try
        {                    
            // size, position or "stacking" has changed. Update the bounds() property
            
            Rect newBounds = _getBounds();
            
            // the bounds may not have changed
            if(newBounds!=_currBounds)
            {   
                _currBounds = newBounds;            
            
                // update the properties. Note that this will not cause
                // another configure event, because the setBounds method will only
                // reposition or resize the window if the new size/position are different
                // from the current ones.
                P<View> pView = getOuterViewIfStillAttached();
                if(pView!=nullptr)
                    pView->adjustAndSetBounds( _currBounds );
            }
        }
        catch(...)
        {
            _inReconfigured = false;
            throw;
        }
        
        _inReconfigured = false;
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
    
    bool        _inReconfigured = false;
};



}
}

#endif
