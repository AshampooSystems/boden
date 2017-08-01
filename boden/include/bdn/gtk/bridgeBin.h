#ifndef BDN_GTK_bridgeBin_H_
#define BDN_GTK_bridgeBin_H_

#include <bdn/View.h>

#include <glib-object.h>

#include <gtk/gtk.h>




G_BEGIN_DECLS

#define BDN_GTK_TYPE_BRIDGE_BIN (bdn_gtk_bridge_bin_get_type ())


G_DECLARE_FINAL_TYPE (BdnGtkBridgeBin, bdn_gtk_bridge_bin, BDN_GTK, BRIDGE_BIN, GtkBin)


/** Creates a new bridge widget for GTK. The bridge widget
 *  is a GTK widget that wraps a bdn::View object and makes
 *  it usable within the GTK layout system. I.e. the bridge will communicate
 *  the preferred size of the bdn::View to GTK, so that the GTK layout can
 *  size the bridge correctly.
 * 
 *  The bridge will include the child view's margin in its preferred size.
 *  You can also set an additinal padding for the bridge (see bdn_gtk_bridge_bin_set_padding)
 * 
 * 
 *  The bdn::View object can be set with bdn_gtk_bridge_bin_set_view.
 * */
GtkWidget* bdn_gtk_bridge_bin_new(void);



G_END_DECLS



/** Sets the child view object that the bridge wraps.
 *  Can also be null to simply remove the current view.
 * 
 *  The bridge object will hold a strong reference to the bdn::View object as long
 *  as it is set.
 * 
 *  pChildView is the Boden View object
 *  pChildWidget is the view's corresponding Gtk Widget
 * */
void bdn_gtk_bridge_bin_set_child(BdnGtkBridgeBin* bridge, bdn::View* pChildView, GtkWidget* pChildWidget);


/** Sets the padding for the bridge.
 * 
 *  The padding is copied.
 * */
void bdn_gtk_bridge_bin_set_padding (
    BdnGtkBridgeBin* bridge,
	const bdn::Margin& padding);



#endif


