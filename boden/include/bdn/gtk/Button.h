#ifndef BDN_Button_H_
#define BDN_Button_H_


#include <bdn/ButtonBase.h>
#include <bdn/Notifier.h>
#include <bdn/ClickEvent.h>
#include <bdn/IButton.h>
#include <bdn/Frame.h>

#include <gtk/gtk.h>


namespace bdn
{

class Button : public ButtonBase, BDN_IMPLEMENTS IButton
{
public:
    Button(Frame* pParent, const String& label)
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
};


}

#endif
