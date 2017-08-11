#include <bdn/init.h>
#include <bdn/Base.h>


#include <bdn/gtk/bridge.h>
#include <bdn/gtk/util.h>
#include <bdn/gtk/ViewCore.h>

// XXX
#include "../../../../gtksrc/gtkwidgetprivate.h"

extern "C"
{


struct _BdnGtkBridge
{
    GtkLayout       layout;

    bdn::View*      pChildView;
    bdn::Margin*    pPadding;
};


static void bdn_gtk_bridge_get_preferred_width(
    GtkWidget           *widget,
    gint                *minimum_width,
    gint                *natural_width);

static void bdn_gtk_bridge_get_preferred_height(
    GtkWidget           *widget,
    gint                *minimum_height,
    gint                *natural_height);

static void bdn_gtk_bridge_get_preferred_width_for_height(
    GtkWidget           *widget,
    gint                 height,
    gint                *minimum_width,
    gint                *natural_width);

static void bdn_gtk_bridge_get_preferred_height_for_width(
    GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);
    
static void bdn_gtk_bridge_get_preferred_height_and_baseline_for_width (
    GtkWidget     *widget,
    gint           width,
    gint          *minimum_height,
    gint          *natural_height,
    gint          *minimum_baseline,
    gint          *natural_baseline);
    
static void bdn_gtk_bridge_size_allocate(
    GtkWidget           *widget,
    GtkAllocation       *allocation);
    
static GtkSizeRequestMode bdn_gtk_bridge_get_request_mode (GtkWidget *widget);
    

G_DEFINE_TYPE (BdnGtkBridge, bdn_gtk_bridge, GTK_TYPE_LAYOUT)

static void bdn_gtk_bridge_class_init (BdnGtkBridgeClass *klass);

static void bdn_gtk_bridge_init (BdnGtkBridge* self);

static void bdn_gtk_bridge_dispose (GObject *gobject);
static void bdn_gtk_bridge_finalize (GObject *gobject);

}   // extern "C"


static void bdn_gtk_bridge_class_init (BdnGtkBridgeClass *klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS( klass );
    
    object_class->dispose = bdn_gtk_bridge_dispose;
    object_class->finalize = bdn_gtk_bridge_finalize;

    widget_class->get_preferred_width = bdn_gtk_bridge_get_preferred_width;
    widget_class->get_preferred_height = bdn_gtk_bridge_get_preferred_height;
    widget_class->get_preferred_width_for_height = bdn_gtk_bridge_get_preferred_width_for_height;
    widget_class->get_preferred_height_for_width = bdn_gtk_bridge_get_preferred_height_for_width;
    widget_class->get_preferred_height_and_baseline_for_width = bdn_gtk_bridge_get_preferred_height_and_baseline_for_width;
    
    widget_class->size_allocate = bdn_gtk_bridge_size_allocate;       
    
    widget_class->get_request_mode = bdn_gtk_bridge_get_request_mode;
}

static void bdn_gtk_bridge_init (BdnGtkBridge* self)
{
    self->pPadding = new bdn::Margin;
}


static void bdn_gtk_bridge_dispose (GObject *gobject)
{
    BdnGtkBridge* pBridge = BDN_GTK_BRIDGE( gobject );
     
    
    // In dispose(), you are supposed to free all types referenced from this
    // object which might themselves hold a reference to self. Generally,
    // the most simple solution is to unref all members on which you own a 
    // reference.
    
    if(pBridge->pChildView!=nullptr)
    {
        pBridge->pChildView->releaseRef();
        pBridge->pChildView = nullptr;
    }

    // Always chain up to the parent class; there is no need to check if
    // the parent class implements the dispose() virtual function: it is
    // always guaranteed to do so
    
    G_OBJECT_CLASS (bdn_gtk_bridge_parent_class)->dispose (gobject);
}

static void bdn_gtk_bridge_finalize (GObject *gobject)
{
    BdnGtkBridge* pBridge = BDN_GTK_BRIDGE( gobject );
     
    delete pBridge->pPadding;
    
    G_OBJECT_CLASS (bdn_gtk_bridge_parent_class)->finalize (gobject);
}

GtkSizeRequestMode bdn_gtk_bridge_get_request_mode (GtkWidget *widget)
{
    BdnGtkBridge* pBridge = BDN_GTK_BRIDGE( widget );
    
    GtkSizeRequestMode result = GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH;
    
    if(pBridge->pChildView)
    {
        bdn::P<bdn::gtk::ViewCore> pChildCore = bdn::tryCast<bdn::gtk::ViewCore>( pBridge->pChildView->getViewCore() );
        if(pChildCore!=nullptr)
        {
            GtkWidget* pChildWidget = pChildCore->getGtkWidget();
            if(pChildWidget!=nullptr)
                result = gtk_widget_get_request_mode( pChildWidget );
        }
    }
    
    return result;
}


static void
bdn_gtk_bridge_get_preferred_width (
    GtkWidget *widget,
    gint      *minimum_width,
    gint      *natural_width)
{
    BdnGtkBridge* pBridge = BDN_GTK_BRIDGE( widget );

    *minimum_width = 0;
    *natural_width = 0;
    
    if(pBridge->pChildView)
    {
        bdn::Margin childMargin = pBridge->pChildView->uiMarginToDipMargin( pBridge->pChildView->margin() );
        
        bdn::Size prefSize = pBridge->pChildView->calcPreferredSize();
        
        // add the child's margin
        prefSize += childMargin;
        
        // and any additional padding we may have
        prefSize += *pBridge->pPadding;
        
        *minimum_width = prefSize.width;
        *natural_width = prefSize.width;
    }
}

static void
bdn_gtk_bridge_get_preferred_height (
    GtkWidget *widget,
    gint      *minimum_height,
    gint      *natural_height)
{
    BdnGtkBridge* pBridge = BDN_GTK_BRIDGE( widget );

    *minimum_height = 0;
    *natural_height = 0;
    
    if(pBridge->pChildView)
    {
        bdn::Margin childMargin = pBridge->pChildView->uiMarginToDipMargin( pBridge->pChildView->margin() );
        
        bdn::Size prefSize = pBridge->pChildView->calcPreferredSize();
        
        // add the child's margin
        prefSize += childMargin;
        
        // and any additional padding we may have
        prefSize += *pBridge->pPadding;
        
        *minimum_height = prefSize.height;
        *natural_height = prefSize.height;
    }
}


static void 
bdn_gtk_bridge_get_preferred_width_for_height (
    GtkWidget *widget,
    gint       height,
    gint      *minimum_width,
    gint      *natural_width)
{
    BdnGtkBridge* pBridge = BDN_GTK_BRIDGE( widget );

    *minimum_width = 0;
    *natural_width = 0;
    
    if(pBridge->pChildView)
    {
        bdn::Margin childMargin = pBridge->pChildView->uiMarginToDipMargin( pBridge->pChildView->margin() );
        
        double availHeight = height;
        if(availHeight==-1)
            availHeight = bdn::Size::componentNone();
        else
        {
            availHeight -= childMargin.top + childMargin.bottom + pBridge->pPadding->top + pBridge->pPadding->bottom;
            if(availHeight<0)
                availHeight = 0;
        }
        
        bdn::Size availableSpace( bdn::Size::componentNone(), availHeight );
        
        bdn::Size prefSize = pBridge->pChildView->calcPreferredSize( availableSpace );
        
        // add the child's margin
        prefSize += childMargin;
        
        // and any additional padding we may have
        prefSize += *pBridge->pPadding;
        
        *minimum_width = prefSize.width;
        *natural_width = prefSize.width;
    }
}


static void
bdn_gtk_bridge_get_preferred_height_for_width  (
    GtkWidget *widget,
    gint       width,
    gint      *minimum_height,
    gint      *natural_height)
{
    bdn_gtk_bridge_get_preferred_height_and_baseline_for_width(widget, width, minimum_height, natural_height, nullptr, nullptr);    
}


static void
bdn_gtk_bridge_get_preferred_height_and_baseline_for_width (
    GtkWidget     *widget,
    gint           width,
    gint          *minimum_height,
    gint          *natural_height,
    gint          *minimum_baseline,
    gint          *natural_baseline)
{
    BdnGtkBridge* pBridge = BDN_GTK_BRIDGE( widget );

    *minimum_height = 0;
    *natural_height = 0;
    
    if(minimum_baseline)
        *minimum_baseline = 0;
    if(natural_baseline)
        *natural_baseline = 0;
    
    
    if(pBridge->pChildView)
    {
        bdn::Margin childMargin = pBridge->pChildView->uiMarginToDipMargin( pBridge->pChildView->margin() );
        
        double availWidth = width;
        if(availWidth==-1)
            availWidth = bdn::Size::componentNone();
        else
        {
            availWidth -= childMargin.top + childMargin.bottom + pBridge->pPadding->top + pBridge->pPadding->bottom;
            if(availWidth<0)
                availWidth = 0;
        }
            
        bdn::Size availableSpace( availWidth, bdn::Size::componentNone() );
        
        bdn::Size prefSize = pBridge->pChildView->calcPreferredSize( availableSpace );
        
        // add the child's margin
        prefSize += childMargin;
        
        // and any additional padding we may have
        prefSize += *pBridge->pPadding;
        
        *minimum_height = prefSize.height;        
        *natural_height = prefSize.height;
        
        if(minimum_baseline || natural_baseline)
        {        
            // we get the baseline directly from the child widget
            bdn::P<bdn::gtk::ViewCore> pChildCore = bdn::tryCast<bdn::gtk::ViewCore>( pBridge->pChildView->getViewCore() );
            if(pChildCore!=nullptr)
            {
                GtkWidget* pChildWidget = pChildCore->getGtkWidget();
                if(pChildWidget!=nullptr)
                {
                    gint childMinHeight=0;
                    gint childNatHeight=0;
                    gint childMinBaseline=0;
                    gint childNatBaseline=0;
                    
                    GTK_WIDGET_GET_CLASS (pChildWidget)->get_preferred_height_and_baseline_for_width (
                        pChildWidget,
                        std::isfinite(availWidth) ? availWidth : -1,
                        &childMinHeight,
                        &childNatHeight,
                        &childMinBaseline,
                        &childNatBaseline );
                        
                    if(minimum_baseline)
                        *minimum_baseline = childMinBaseline + childMargin.top + pBridge->pPadding->top;
                    if(natural_baseline)
                        *natural_baseline = childNatBaseline + childMargin.top + pBridge->pPadding->top;
                }                    
            }        
        }
    }
}




static void
bdn_gtk_bridge_size_allocate (
    GtkWidget     *widget,
    GtkAllocation *allocation)
{
    BdnGtkBridge* pBridge = BDN_GTK_BRIDGE( widget );

    gtk_widget_set_allocation (widget, allocation);
    
    // all we need to do here is let the bdn::View know that it has
    // been resized.
    if(pBridge->pChildView)
    {
        bdn::Margin childMargin = pBridge->pChildView->uiMarginToDipMargin( pBridge->pChildView->margin() );
        
        bdn::Rect childBounds = bdn::gtk::gtkRectToRect( *allocation );
        
        childBounds.x = 0;
        childBounds.y = 0;
        
        childBounds -= *pBridge->pPadding;
        childBounds -= childMargin;
        
        pBridge->pChildView->adjustAndSetBounds( childBounds );
    }
    
    // call the base class method. This will resize our child view.
    GTK_WIDGET_CLASS (bdn_gtk_bridge_parent_class) -> size_allocate( widget, allocation );
}



GtkWidget* bdn_gtk_bridge_new(void)
{
    return GTK_WIDGET( g_object_new (BDN_GTK_TYPE_BRIDGE, NULL) );
}


void bdn_gtk_bridge_set_child (
    BdnGtkBridge* bridge,
	bdn::View* pChildView,
    GtkWidget* pWidget)
{
    if(pChildView!=nullptr)
        pChildView->addRef();
    
    if(bridge->pChildView!=nullptr)
        bridge->pChildView->releaseRef();
        
    bridge->pChildView = pChildView;
    
    gtk_container_add ( GTK_CONTAINER(bridge), pWidget );
    
    // our preferred size may have changed
    gtk_widget_queue_resize( GTK_WIDGET(bridge) );
}


void bdn_gtk_bridge_set_padding (
    BdnGtkBridge* bridge,
	const bdn::Margin& padding)
{
    // our preferred size may have changed
    gtk_widget_queue_resize( GTK_WIDGET(bridge) );
    
    *bridge->pPadding = padding;
}


