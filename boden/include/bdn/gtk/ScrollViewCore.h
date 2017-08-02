#ifndef BDN_GTK_ScrollViewCore_H_
#define BDN_GTK_ScrollViewCore_H_


#include <bdn/IScrollViewCore.h>
#include <bdn/ScrollView.h>
#include <bdn/ScrollViewLayoutHelper.h>

#include <gtk/gtk.h>


namespace bdn
{
namespace gtk
{

class ScrollViewCore : public ViewCore, BDN_IMPLEMENTS IScrollViewCore
{
private:
    static GtkWidget* _createScrolledWindow( ScrollView* pOuter )
    {
        GtkWidget* pScrolledWindow = gtk_scrolled_window_new(nullptr, nullptr);
        
        return pScrolledWindow;
    }
    
public:
    ScrollViewCore(View* pOuter)
    : ViewCore( pOuter,
                _createScrolledWindow( cast<ScrollView>(pOuter) ) )
    {   
        // note: we do not use our GTK Bridge widget here since
        // GtkScrolledWindow does not fully pass size requests on to it anyway
        // (Width-for-height and height-for-width tradeoffs are not used).
        // Because of that we have to explicitly set the desired content size
        // anyway, so the bridge would not offer any benefits. In fact, it would
        // interfere because its unconditional preferred size IS taken into account by
        // the scroll window. In contrast, a simple GtkLayout allows us to have full control
        // at a single point.
        _pScrolledArea = gtk_layout_new(nullptr, nullptr);
        gtk_container_add( GTK_CONTAINER(getGtkWidget()), _pScrolledArea);
        
        gtk_widget_set_visible( _pScrolledArea, TRUE );
        
        // see createLayoutHelper for an explanation of why we need this dummy window
        
        _pDummyScrolledWindow = gtk_scrolled_window_new(nullptr, nullptr);
        gtk_container_add( GTK_CONTAINER(_pScrolledArea), _pDummyScrolledWindow);
                
        // always enable scrolling in our dummy scroll window
        gtk_scrolled_window_set_policy(
                GTK_SCROLLED_WINDOW( _pDummyScrolledWindow ),
                GTK_POLICY_AUTOMATIC,
                GTK_POLICY_AUTOMATIC );
        
        // add a dummy layout to the dummy scrolled window, just so that it has a client area
        GtkWidget* pDummyScrolledArea = gtk_layout_new(nullptr, nullptr);
        
        gtk_container_add( GTK_CONTAINER(_pDummyScrolledWindow), pDummyScrolledArea);
        
        gtk_widget_set_visible( _pDummyScrolledWindow, TRUE);
        gtk_widget_set_visible( pDummyScrolledArea, TRUE);        
        
        // move the dummy window outside of the visible area
        gtk_layout_move( GTK_LAYOUT(_pScrolledArea), _pDummyScrolledWindow, -1000, -1000);
                                                        
        updateScrollPolicy();
    }
    
    
    void setPadding(const Nullable<UiMargin>& uiPadding) override
    {
        // nothing to do. We apply the padding in calcPreferredSize and layout.
        /*Margin padding;
        
        if(!uiPadding.isNull())
            padding = uiMarginToDipMargin(uiPadding);
        
        bdn_gtk_bridge_set_padding( BDN_GTK_BRIDGE( _pScrolledAreaBridge ), padding );        */
    }
    
        
    void setHorizontalScrollingEnabled(const bool& enabled) override
    {
        updateScrollPolicy();
    }

    void setVerticalScrollingEnabled(const bool& enabled) override
    {
        updateScrollPolicy();
    }
    
    
    
    
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
        Size preferredSize;
        
        P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );
        if(pOuter!=nullptr)
        {        
            P<ScrollViewLayoutHelper> pHelper = createLayoutHelper();
                
            preferredSize = pHelper->calcPreferredSize( pOuter, availableSpace );
        }
        
        return preferredSize;
	}

        
	void layout() override
	{
        // we have to calculate the size we want our content view to have and then
        // set that as the min content size in the gtk scrolled window.
        
        P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );
        if(pOuter!=nullptr)
        {        
            // we do not have any borders, so the viewport size without scrollbars
            // is the full scrollview size.
            Size viewPortSize = pOuter->size();
            
            P<ScrollViewLayoutHelper> pHelper = createLayoutHelper();        
            pHelper->calcLayout( pOuter, viewPortSize );
            
            Size scrolledAreaSize = pHelper->getScrolledAreaSize();
            
            gtk_layout_set_size( GTK_LAYOUT(_pScrolledArea), scrolledAreaSize.width, scrolledAreaSize.height);
            
            //GtkScrolledWindow* pScrolledWindow = GTK_SCROLLED_WINDOW( getGtkWidget() );
            
            //gtk_scrolled_window_set_min_content_width( pScrolledWindow, scrolledAreaSize.width );
            //gtk_scrolled_window_set_min_content_height( pScrolledWindow, scrolledAreaSize.height );
            
            P<View> pContentView = pOuter->getContentView();
            if(pContentView!=nullptr)
                pContentView->adjustAndSetBounds( pHelper->getContentViewBounds() );
        }
	}
    
    
    void _addChildViewCore(ViewCore* pChildCore) override
    {
        gtk_container_add( GTK_CONTAINER(_pScrolledArea), pChildCore->getGtkWidget() );
        
        //P<View> pChildView = pChildCore->getOuterViewIfStillAttached();

        //bdn_gtk_bridge_set_child( BDN_GTK_BRIDGE(_pScrolledAreaBridge), pChildView, pChildCore->getGtkWidget() );        
    }
    
    void _moveChildViewCore(ViewCore* pChildCore, int gtkX, int gtkY) override
    {
        gtk_layout_move( GTK_LAYOUT(_pScrolledArea), pChildCore->getGtkWidget(), gtkX, gtkY);
                
        /*
        // the content view CANNOT be moved. This is under the control of the scroll view
        // and the bridge widget.        */
    }    
        
        
        
protected:
    bool canAdjustWidthToAvailableSpace() const override
    {
        return true;
    }


    bool canAdjustHeightToAvailableSpace() const override
    {
        return true;
    }
    
private:
    void updateScrollPolicy()
    {
        P<ScrollView> pOuter = cast<ScrollView>( getOuterViewIfStillAttached() );
        if(pOuter!=nullptr)
        {        
            gtk_scrolled_window_set_policy(
                GTK_SCROLLED_WINDOW( getGtkWidget() ),
                pOuter->horizontalScrollingEnabled() ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER,
                pOuter->verticalScrollingEnabled() ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER );
        }
    }
    
    P<ScrollViewLayoutHelper> createLayoutHelper() const
    {
        // GtkScrolledWindow does not actually take the child size into account, unless
        // gtk_scrolled_window_get_propagate_natural_width etc. are set to TRUE.
        // But gtk_scrolled_window_get_propagate_natural_XYZ are relatively new (GTK 3.22 or newer)
        // So we do not want to depend on those.
        // Instead we would have to set the child's size as the min content size in the scrolled window.
        // That is taken into account.
        
        // BUT there is a second problem. GtkScrolledWindow does not implement the "height for width" or
        // "width for height" sizing. It does not pass the available width/height on to the child, so the child
        // has no way to adapt to it.
        // Since we set the content size manually, we can fix that too, by calculating the content size
        // correctly for the amount of available space.
        
        // BUT if we want to do that then we need to know the size of the nonclient stuff that scrolled window
        // shows. Border sizes, sizes of scrollbars, etc.
        // Whether or not a border is shown is controlled by the scrolled child window in GTK. Since we provide that
        // (our bridge) that means that no border is shown.
        
        // The sizes of the scrollbars are a little more complicated. GtkScrolledWindow supports "overlay" scrollbars
        // (which do not take up any layout space) as well as old-school fixed scrollbars.
        // According to the GTK documentation, which model is used depends on settings and on whether or not a mouse is connected.
        // At the time of this writing there is no "mouse connected" check in the GTK source code, but we must assume that they
        // might implement it in the future.
        // Since the triggers for overlay scrolling are unclear and subject to change, we cannot check for those.
        // We must look at the results and detect from that whether or not the overlay scrolling is used or not.
        
        // Looking at the GtkScrolledWindow source code (at the time of this writing) we can check the scrollbar parent
        // to see if overlay scrolling is used. If the parent is the scrolled window then scrollbars take up real
        // layout space. If it is some other window then overlay scrolling is used.
        // However, it seems risky to depend on this fact.
        
        // So, how else can we detect if scrollbars take up space?
        
        // We set a fixed content size, force a viewport size that is smaller than that and        
        // then ask the scroll window to give us its preferred size. Then we can deduce the
        // scrollbar layout space from that.
        
        // We need a dummy scrolled window to do this.
        // The GTK documentation states that this behaviour depends on whether a mouse
        // is connected or not - and that can obviously change at runtime.
        // Note that this change detection does not seem to be implemented at the time of
        // this writing, so we do not know what signal might triggers the scrollbar behaviour change
        // in the future.
        // So, we must assume that the scrollbar layout behaviour can change at any time. I.e.
        // we need to check whenever we do a layout operation.
        
        // The problem is now that we cannot use our real scrolled window for this because the
        // cached layout sizes from past operations would interfere with the scrollbar space detection.
        // And clearing the cache is something that happens asynchronously when events are handled, so
        // we cannot do that on demand either.
        
        // So, the bottom line is that we need a GtkScrolledWindow instance that is not affected by our
        // normal layout, just to detect the scrollbar stuff whenever we need that.
        
        // We cannot create this dummy instance on demand, since the layout only works properly when the
        // widget is realized (i.e. connected to the screen) and that in turn also only happens asynchronously.
        // So that means the dummy instance must already be there and be realized.
        
        // So, what we do is we add a 0x0 scrolled window to the inner scrolled area of our scrolled window.
        // This is _pDummyScrolledWindow.
        
        // the dummy window has scrolling enabled in both directions. It has a dummy content size.
        
        int horzBarHeight = 0;
        int vertBarWidth = 0;
        
        // if scrollbars are overlays then the scrolled window will have a minimum size of 0.
        // If scrollbars are not overlays then the scrolled window has a somewhat complicated minimum size,
        // a combination of multiple values.
        
        // in addition to the scrollbar sizes, there can also be spacing configured between the scrollbar
        // and the content. We must include that in our calculation as well.        
        
        // Note that we get that property from the "real" scroll window.
        GtkScrolledWindowClass* pScrollClass;
        
        GtkWidget* pRealScrolledWindow = getGtkWidget();

        pScrollClass = GTK_SCROLLED_WINDOW_GET_CLASS( GTK_SCROLLED_WINDOW(pRealScrolledWindow) );
        
        int scrollBarSpacing=0;

        if (pScrollClass->scrollbar_spacing >= 0)
            scrollBarSpacing = pScrollClass->scrollbar_spacing;
        else
        {
            gint spacing = 0;

            gtk_widget_style_get ( pRealScrolledWindow, "scrollbar-spacing", &spacing, NULL);
            
            scrollBarSpacing = spacing;
        }        
        
        {
            gint minHeight = 0;
            gint naturalHeight = 0;        
            gtk_widget_get_preferred_height( _pDummyScrolledWindow, &minHeight, &naturalHeight );
            
            if(minHeight>0)
            {
                // horizontal scroll bar does take up space.
                GtkWidget* pHBar = gtk_scrolled_window_get_hscrollbar( GTK_SCROLLED_WINDOW(_pDummyScrolledWindow) );
                if(pHBar!=nullptr)
                {                
                    gint barMinHeight=0;
                    gint barNaturalHeight=0;
                    gtk_widget_get_preferred_height ( pHBar, &barMinHeight, &barNaturalHeight );
                    
                    horzBarHeight = barNaturalHeight + scrollBarSpacing;                
                }
            }
        }
        
        // same for the height
        {
            gint minWidth = 0;
            gint naturalWidth = 0;       
            gtk_widget_get_preferred_width( _pDummyScrolledWindow, &minWidth, &naturalWidth );
            
            if(minWidth>0)
            {
                // horizontal scroll bar does take up space.
                GtkWidget* pVBar = gtk_scrolled_window_get_vscrollbar( GTK_SCROLLED_WINDOW(_pDummyScrolledWindow) );
                if(pVBar!=nullptr)
                {                
                    gint barMinWidth=0;
                    gint barNaturalWidth=0;
                    gtk_widget_get_preferred_width ( pVBar, &barMinWidth, &barNaturalWidth );
                
                    vertBarWidth = barNaturalWidth + scrollBarSpacing;                
                }                
            }            
        }
                
        // now we can let our scrollview layout helper determine the preferred size.
        return newObj<ScrollViewLayoutHelper>( vertBarWidth, horzBarHeight);
    }
       
    GtkWidget* _pScrolledArea;
    GtkWidget* _pDummyScrolledWindow;
    

};


}
}

#endif
