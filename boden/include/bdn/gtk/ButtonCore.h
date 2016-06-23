#ifndef BDN_GTK_ButtonCore_H_
#define BDN_GTK_ButtonCore_H_


#include <bdn/gtk/ButtonCoreBase.h>
#include <bdn/IButtonCore.h>

#include <gtk/gtk.h>


namespace bdn
{
namespace gtk
{

class ButtonCore : public ButtonCoreBase, BDN_IMPLEMENTS IButtonCore
{
public:/*
    ButtonCore(Frame* pParent, const String& label)
    {
        GtkWidget* pWidget = gtk_button_new();
        
        gtk_container_add( GTK_CONTAINER(pParent->getClientContainer() ), pWidget);
        
        initButton( pWidget, label );
        g_signal_connect( _pWidget, "clicked", G_CALLBACK(clickedCallback), this );
    }


    Property<String>& label() override
    {
        return ButtonBase::label();
    }
    
	ReadProperty<String>& label() const override
    {
        return ButtonBase::label();
    }
        

    void generateClick()
    {
        ClickEvent evt(this);

        _onClick.notify(evt);
    }

    Notifier<const ClickEvent&>& onClick() override
    {        
        return _onClick;
    }


protected:
    static void clickedCallback(GtkWidget* pWidget, gpointer pParam)
    {
        ((Button*)pParam)->generateClick();
    }
    
    
    Notifier<const ClickEvent&> _onClick;
     */
};


}
}

#endif
