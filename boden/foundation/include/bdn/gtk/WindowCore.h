#ifndef BDN_GTK_WindowCore_H_
#define BDN_GTK_WindowCore_H_

#include <bdn/gtk/ViewCore.h>
#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <bdn/windowCoreUtil.h>
#include <bdn/debug.h>

#include <gtk/gtk.h>

#include <chrono>

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
                
        _lastKnownGtkBounds = _getGtkBounds();
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


    Rect adjustAndSetBounds(const Rect& requestedBounds) override
    {
        if(_inGtkBoundsChanged)
        {
            // adjustAndSetBounds is currently called in the context of a notification that the
            // GTK bounds have changed. That is just done to notify the outer view of the new bounds and no change
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
        {
            //BDN_DEBUGGER_PRINT( "Resizing window "+std::to_string((uintptr_t)this)+" to "+std::to_string(alloc.width)+", "+std::to_string(alloc.height));
            
            gtk_window_resize( getGtkWindow(), alloc.width, alloc.height );
        }            
            
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
            // note that _currBounds has the size and position without window borders and decorations.
            // That is ok, since we completely ignore the nonclient area of the window and pretend
            // that it is 0 anyway. All GTK functions work that way, so that is OK.
        
            Rect contentArea( Point(), _currBounds.getSize() );
            
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
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
            defaultWindowAutoSizeImpl(pWindow, getScreenWorkArea().getSize() );
    }

   
    void center() override
    {
        P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
        if(pWindow!=nullptr)
            defaultWindowCenterImpl(pWindow, getScreenWorkArea() );
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


    Rect _getGtkBounds()
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
    
    
    
    void _reconfigured(GdkEvent* pEvent)
    {
        // a single window size change will typically cause multiple reconfigure
        // events to be sent in quick succession. For example, when we resize a window
        // with gtk_window_resize then we get 4-5 events. The first 4 are still with the
        // old size, only the last has the new size.
        // We want to ignore the intermediate events that are fired when the final
        // size is not yet set. Otherwise we will update our layout multiple times,
        // which may slow the UI down noticeably.
        
        // so we simply compare the current size with the old size and only react if it changes.
        
        checkGtkBoundsChanged();
    }
    
    void checkGtkBoundsChanged()
    {
        Rect bounds = _getGtkBounds();
        
        bool posChange = (bounds.getPosition() != _lastKnownGtkBounds.getPosition() );
        bool sizeChange = (bounds.getSize() != _lastKnownGtkBounds.getSize());
        
        if( posChange || sizeChange )
        {
            _lastKnownGtkBounds = bounds;
                        
            gtkBoundsChanged( bounds, posChange, sizeChange );            
        }
    }
    
    void gtkBoundsChanged(const Rect& newBounds, bool posChange, bool sizeChange)
    {        
        _inGtkBoundsChanged = true;
        
        try
        {                           
            // BDN_DEBUGGER_PRINT( "Window "+std::to_string((uintptr_t)this)+" reconfigured to "+std::to_string(newBounds.x)+", "+std::to_string(newBounds.y)+" "+std::to_string(newBounds.width)+", "+std::to_string(newBounds.height)+" posChange="+std::to_string(posChange)+" sizeChange="+std::to_string(sizeChange) );
            
            // we ignore the size we get from GTK, UNLESS it has changed.
            // The reason for this is that the GTK window size lags behind our
            // size considerably and is only updated with an irregular, unpredictable
            // delay. To work around this, we immediately treat the window as having
            // the desired size after we issue the resize request.
            
            // Following the resize request we usually get several reconfigure events
            // for which only the position has changed and the size is still at the old value.
            // When handling these events we want to use our new size that the window will eventually
            // have.
            // Note that we DO react when the size changes. Usually this change happens when
            // the desired size is finally applied, but it may also be that it happens when the
            // user has manually resized the window. In both cases we want to update our own
            // size and use the one from GTK from that point on (until we issue the next
            // resize request).
            
            Rect newBoundsToUse;
            if(sizeChange)
                newBoundsToUse = newBounds;
            else
                newBoundsToUse = Rect(newBounds.getPosition(), _currBounds.getSize() );
                
            // BDN_DEBUGGER_PRINT( "Window "+std::to_string((uintptr_t)this)+" using "+std::to_string(newBoundsToUse.x)+", "+std::to_string(newBoundsToUse.y)+" "+std::to_string(newBoundsToUse.width)+", "+std::to_string(newBoundsToUse.height) );
            
            // the bounds may not have changed
            if(newBoundsToUse!=_currBounds)
            {   
                _currBounds = newBoundsToUse;            
            
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
            _inGtkBoundsChanged = false;
            throw;
        }
        
        _inGtkBoundsChanged = false;
    }
    
    
    static gboolean _reconfiguredCallback(    GtkWidget* pWidget,
                                            GdkEvent* pEvent,
                                            gpointer  pUser)
    {
        ((WindowCore*)pUser)->_reconfigured( pEvent);
        
        return FALSE;
    }
    
    
    GtkWidget*  _pContentParentWidget;
    
    Size        _minSize;    
    Rect        _currBounds;
    
    bool        _inGtkBoundsChanged = false;
    
    Rect        _lastKnownGtkBounds;
};



}
}

#endif
