#ifndef BDN_Switch_H_
#define BDN_Switch_H_


#include <bdn/ISwitch.h>
#include <bdn/ButtonBase.h>
#include <bdn/Frame.h>


namespace bdn
{

class Switch : public ButtonBase, BDN_IMPLEMENTS ISwitch
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

#endif
