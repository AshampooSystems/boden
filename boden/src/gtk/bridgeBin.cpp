#include <bdn/init.h>
#include <bdn/Base.h>


#include <bdn/gtk/bridgeBin.h>
#include <bdn/gtk/util.h>


extern "C"
{


struct _BdnGtkBridgeBin
{
    GtkBin parent_instance;

    bdn::View*      pChildView;
    bdn::Margin*    pPadding;
};


static void bdn_gtk_bridge_bin_get_preferred_width(
    GtkWidget           *widget,
    gint                *minimum_width,
    gint                *natural_width);

static void bdn_gtk_bridge_bin_get_preferred_height(
    GtkWidget           *widget,
    gint                *minimum_height,
    gint                *natural_height);

static void bdn_gtk_bridge_bin_get_preferred_width_for_height(
    GtkWidget           *widget,
    gint                 height,
    gint                *minimum_width,
    gint                *natural_width);

static void bdn_gtk_bridge_bin_get_preferred_height_for_width(
    GtkWidget           *widget,
    gint                 width,
    gint                *minimum_height,
    gint                *natural_height);
    
static void bdn_gtk_bridge_bin_size_allocate(
    GtkWidget           *widget,
    GtkAllocation       *allocation);
    

G_DEFINE_TYPE (BdnGtkBridgeBin, bdn_gtk_bridge_bin, GTK_TYPE_BIN)

static void bdn_gtk_bridge_bin_class_init (BdnGtkBridgeBinClass *klass);

static void bdn_gtk_bridge_bin_init (BdnGtkBridgeBin* self);

static void bdn_gtk_bridge_bin_dispose (GObject *gobject);
static void bdn_gtk_bridge_bin_finalize (GObject *gobject);

}   // extern "C"


static void bdn_gtk_bridge_bin_class_init (BdnGtkBridgeBinClass *klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass* widget_class = GTK_WIDGET_CLASS( klass );
    GtkBinClass* bin_class = GTK_BIN_CLASS(klass);
    
    object_class->dispose = bdn_gtk_bridge_bin_dispose;
    object_class->finalize = bdn_gtk_bridge_bin_finalize;

    widget_class->get_preferred_width = bdn_gtk_bridge_bin_get_preferred_width;
    widget_class->get_preferred_height = bdn_gtk_bridge_bin_get_preferred_height;
    widget_class->get_preferred_width_for_height = bdn_gtk_bridge_bin_get_preferred_width_for_height;
    widget_class->get_preferred_height_for_width = bdn_gtk_bridge_bin_get_preferred_height_for_width;
    widget_class->size_allocate = bdn_gtk_bridge_bin_size_allocate;       
}

static void bdn_gtk_bridge_bin_init (BdnGtkBridgeBin* self)
{
    self->pPadding = new bdn::Margin;
}


static void bdn_gtk_bridge_bin_dispose (GObject *gobject)
{
    BdnGtkBridgeBin* pBridge = BDN_GTK_BRIDGE_BIN( gobject );
     
    
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
    
    G_OBJECT_CLASS (bdn_gtk_bridge_bin_parent_class)->dispose (gobject);
}

static void bdn_gtk_bridge_bin_finalize (GObject *gobject)
{
    BdnGtkBridgeBin* pBridge = BDN_GTK_BRIDGE_BIN( gobject );
     
    delete pBridge->pPadding;
    
    G_OBJECT_CLASS (bdn_gtk_bridge_bin_parent_class)->finalize (gobject);
}


static void
bdn_gtk_bridge_bin_get_preferred_width (
    GtkWidget *widget,
    gint      *minimum_width,
    gint      *natural_width)
{
    BdnGtkBridgeBin* pBridge = BDN_GTK_BRIDGE_BIN( widget );

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
bdn_gtk_bridge_bin_get_preferred_height (
    GtkWidget *widget,
    gint      *minimum_height,
    gint      *natural_height)
{
    BdnGtkBridgeBin* pBridge = BDN_GTK_BRIDGE_BIN( widget );

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
bdn_gtk_bridge_bin_get_preferred_width_for_height (
    GtkWidget *widget,
    gint       height,
    gint      *minimum_width,
    gint      *natural_width)
{
    BdnGtkBridgeBin* pBridge = BDN_GTK_BRIDGE_BIN( widget );

    *minimum_width = 0;
    *natural_width = 0;
    
    if(pBridge->pChildView)
    {
        bdn::Margin childMargin = pBridge->pChildView->uiMarginToDipMargin( pBridge->pChildView->margin() );
        
        double availHeight = height;
        availHeight -= childMargin.top + childMargin.bottom + pBridge->pPadding->top + pBridge->pPadding->bottom;
        if(availHeight<0)
            availHeight = 0;
        
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
bdn_gtk_bridge_bin_get_preferred_height_for_width  (
    GtkWidget *widget,
    gint       width,
    gint      *minimum_height,
    gint      *natural_height)
{
    BdnGtkBridgeBin* pBridge = BDN_GTK_BRIDGE_BIN( widget );

    *minimum_height = 0;
    *natural_height = 0;
    
    if(pBridge->pChildView)
    {
        bdn::Margin childMargin = pBridge->pChildView->uiMarginToDipMargin( pBridge->pChildView->margin() );
        
        double availWidth = width;
        availWidth -= childMargin.top + childMargin.bottom + pBridge->pPadding->top + pBridge->pPadding->bottom;
        if(availWidth<0)
            availWidth = 0;
            
        bdn::Size availableSpace( availWidth, bdn::Size::componentNone() );
        
        bdn::Size prefSize = pBridge->pChildView->calcPreferredSize( availableSpace );
        
        // add the child's margin
        prefSize += childMargin;
        
        // and any additional padding we may have
        prefSize += *pBridge->pPadding;
        
        *minimum_height = prefSize.height;
        *natural_height = prefSize.height;
    }
}

static void
bdn_gtk_bridge_bin_size_allocate (
    GtkWidget     *widget,
    GtkAllocation *allocation)
{
    BdnGtkBridgeBin* pBridge = BDN_GTK_BRIDGE_BIN( widget );

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
    GTK_WIDGET_CLASS (bdn_gtk_bridge_bin_parent_class) -> size_allocate( widget, allocation );
}



GtkWidget* bdn_gtk_bridge_bin_new(void)
{
    return GTK_WIDGET( g_object_new (BDN_GTK_TYPE_BRIDGE_BIN, NULL) );
}


void bdn_gtk_bridge_bin_set_child (
    BdnGtkBridgeBin* bridge,
	bdn::View* pChildView,
    GtkWidget* pWidget)
{
    if(pChildView!=nullptr)
        pChildView->addRef();
    
    if(bridge->pChildView!=nullptr)
        bridge->pChildView->releaseRef();
        
    bridge->pChildView = pChildView;
    
    gtk_container_add ( GTK_CONTAINER(bridge), pWidget );
}


void bdn_gtk_bridge_bin_set_padding (
    BdnGtkBridgeBin* bridge,
	const bdn::Margin& padding)
{
    *bridge->pPadding = padding;
}


