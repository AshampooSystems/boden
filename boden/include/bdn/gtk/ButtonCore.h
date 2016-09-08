#ifndef BDN_GTK_ButtonCore_H_
#define BDN_GTK_ButtonCore_H_


#include <bdn/gtk/ButtonCoreBase.h>
#include <bdn/IButtonCore.h>
#include <bdn/Button.h>

#include <gtk/gtk.h>


namespace bdn
{
namespace gtk
{

class ButtonCore : public ButtonCoreBase, BDN_IMPLEMENTS IButtonCore
{
public:
    ButtonCore(View* pOuter)
    : ButtonCoreBase( pOuter, gtk_button_new() )
    {
        setLabel( cast<Button>(pOuter)->label() );        
        
        g_signal_connect( getGtkWidget(), "clicked", G_CALLBACK(clickedCallback), this );
    }
    
    
    void setPadding(const Nullable<UiMargin>& uiPadding)
    {
        // future: could use child-displacement-x or CSS padding property in future.
        // For now we do nothing with the padding at this point. But we DO add it into the preferred
        // size (see ViewCore::_calcPreferredSize), so         
        
    }
    
    void setLabel(const String& label) override
    {
        ButtonCoreBase::setLabel(label);
    }
        

    void generateClick()
    {
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
            ClickEvent evt( pView );

            cast<Button>( pView )->onClick().notify(evt);
        }
    }



protected:
    static void clickedCallback(GtkWidget* pWidget, gpointer pParam)
    {
        ((ButtonCore*)pParam)->generateClick();
    }   
    
};


}
}

#endif
