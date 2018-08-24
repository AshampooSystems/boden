#ifndef BDN_GTK_ToggleCoreBase_H
#define BDN_GTK_ToggleCoreBase_H


#include <bdn/gtk/ViewCore.h>
#include <bdn/IToggleCoreBase.h>
#include <bdn/Toggle.h>

#include <gtk/gtk.h>


namespace bdn
{
namespace gtk
{

class ToggleCoreBase : public ViewCore, BDN_IMPLEMENTS IToggleCoreBase
{
public:
    ToggleCoreBase(View* pOuter, GtkWidget* pWidget)
    : ViewCore( pOuter, pWidget )
    {
        g_signal_connect( getGtkWidget(), "clicked", G_CALLBACK(clickedCallback), this );
    }
    
    void setPadding(const Nullable<UiMargin>& uiPadding) override
    {
        // future: could use child-displacement-x or CSS padding property in future.
        // For now we do nothing with the padding at this point. But we DO add it into the preferred
        // size (see ViewCore::_calcPreferredSize), so
    }

    virtual void generateClick() = 0;

protected:

    Margin getDefaultPaddingDips() const override
    {        
        return uiMarginToDipMargin( UiMargin(UiLength::sem(0.12), UiLength::sem(0.5) ) );
    }
    
    static void clickedCallback(GtkWidget* pWidget, gpointer pParam)
    {
        ((ToggleCoreBase*)pParam)->generateClick();
    }
   
};


}
}

#endif
