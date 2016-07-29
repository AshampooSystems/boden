#ifndef BDN_GTK_SwitchCore_H_
#define BDN_GTK_SwitchCore_H_

/*
#include <bdn/ISwitchCore.h>
#include <bdn/gtk/ButtonCoreBase.h>


namespace bdn
{
namespace gtk
{

class SwitchCore : public ButtonCoreBase, BDN_IMPLEMENTS ISwitchCore
{
public:
    Switch(Frame* pParent, const String& label)
    {
        GtkWidget* pWidget = gtk_check_button_new();
        
        gtk_container_add( GTK_CONTAINER(pParent->getClientContainer() ), pWidget);
        
        initButton( pWidget, label );
    }


    Property<String>& label() override
    {
        return ButtonBase::label();
    }
    
	ReadProperty<String>& label() const override
    {
        return ButtonBase::label();
    }
    
};


}
}
*/
#endif
