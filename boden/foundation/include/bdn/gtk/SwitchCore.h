#ifndef BDN_GTK_SwitchCore_H
#define BDN_GTK_SwitchCore_H


#include <bdn/ISwitchCore.h>
#include <bdn/Switch.h>
#include <bdn/gtk/ViewCore.h>

#include <gtk/gtk.h>


namespace bdn
{
namespace gtk
{

template <class T>
class SwitchCore : public ViewCore, BDN_IMPLEMENTS ISwitchCore
{
public:
    SwitchCore(View* pOuter)
    : ViewCore(pOuter, gtk_layout_new(nullptr, nullptr))
    {
        GtkWidget* layout = getGtkWidget();
        _switch = gtk_switch_new();
        _label = gtk_label_new("");
        gtk_layout_put(GTK_LAYOUT(layout), _switch, 0, 0);
        gtk_layout_put(GTK_LAYOUT(layout), _label, 0, 0);

        setOn( cast<T>(pOuter)->on() );
        setLabel( cast<T>(pOuter)->label() );

        g_signal_connect( _switch, "notify::active", G_CALLBACK(toggledCallback), this );
    }
    
    void setPadding(const Nullable<UiMargin>& uiPadding)
    {
        // future: could use child-displacement-x or CSS padding property in future.
        // For now we do nothing with the padding at this point. But we DO add it into the preferred
        // size (see ViewCore::_calcPreferredSize), so
    }

    void setLabel(const String& label) override
    {
        gtk_label_set_text( GTK_LABEL(_label), label.asUtf8Ptr() );
    }

    void setOn(const bool& on) override
    {
        gtk_switch_set_active( GTK_SWITCH(_switch), on);
    }

    void notifyOn()
    {
        P<View> pView = getOuterViewIfStillAttached();
        if (pView != nullptr) {
            cast<T>(pView)->setOn( gtk_switch_get_active( GTK_SWITCH(_switch) ) );
        }
    }

    void generateClick()
    {
        P<View> pView = getOuterViewIfStillAttached();
        if (pView != nullptr)
        {
            ClickEvent evt( pView );

            cast<T>( pView )->onClick().notify(evt);
        }
    }

    Size calcPreferredSize(const Size& availableSpace = Size::none()) const override
    {
        Size switchSize = minGtkWidgetSize(_switch);
        Size labelSize = minGtkWidgetSize(_label);
        
        float margin;
        if (String(gtk_label_get_text(GTK_LABEL(_label))).length() > 0) {
            margin = 10.;
        } else {
            margin = 0.;
            labelSize.width = 0.; // GTK reports width of label as 1. even if empty
        }

        Size result = Size(switchSize.width + labelSize.width + margin, fmax(switchSize.height, labelSize.height));

        result += _getPaddingIntegerDips();

        P<View> pOuterView = getOuterViewIfStillAttached();
        if(pOuterView!=nullptr)
        {
            result.applyMinimum( pOuterView->preferredSizeMinimum() );
            result.applyMaximum( pOuterView->preferredSizeMaximum() );
        }

        return result;
    }
    
    void layout() override
    {
        Size preferredSize = calcPreferredSize(Size(0, 0));
        Size labelSize = minGtkWidgetSize(_label);
        
        float margin;
        if (String(gtk_label_get_text(GTK_LABEL(_label))).length() > 0) {
            margin = 10.;
        } else {
            margin = 0.;
            labelSize.width = 0.; // GTK reports width of label as 1. even if empty
        }
        
        gtk_layout_move(GTK_LAYOUT(getGtkWidget()), _switch, labelSize.width + margin, 0);
        gtk_layout_move(GTK_LAYOUT(getGtkWidget()), _label, 0, (preferredSize.height - labelSize.height) / 2);
    }

    GtkWidget* _getLabelWidget()
    {
        return _label;
    }

    GtkWidget* _getSwitchWidget()
    {
        return _switch;
    }

protected:

    Margin getDefaultPaddingDips() const override
    {        
        return uiMarginToDipMargin( UiMargin(UiLength::sem(0.12), UiLength::sem(0.5) ) );
    }
    
    static void toggledCallback(GObject* gobject, GParamSpec* pspec, gpointer user_data)
    {
        ((SwitchCore*)user_data)->notifyOn();
    }
    
    Size minGtkWidgetSize(GtkWidget* gtkWidget) const
    {
        gint oldWidth;
        gint oldHeight;
        gtk_widget_get_size_request(gtkWidget, &oldWidth, &oldHeight);

        gboolean oldVisible = gtk_widget_get_visible(gtkWidget);
        if(oldVisible==FALSE)
            gtk_widget_set_visible(gtkWidget, TRUE);

        gtk_widget_set_size_request(gtkWidget, 0, 0);

        GtkRequisition unrestrictedMinSize;
        GtkRequisition unrestrictedNaturalSize;
        gtk_widget_get_preferred_size(gtkWidget, &unrestrictedMinSize, &unrestrictedNaturalSize);
        return Size(unrestrictedMinSize.width, unrestrictedMinSize.height);       
    }

    GtkWidget* _switch;
    GtkWidget* _label;

};


}
}

#endif
