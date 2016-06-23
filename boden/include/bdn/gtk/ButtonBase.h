#ifndef BDN_ButtonBase_H_
#define BDN_ButtonBase_H_


#include <bdn/View.h>
#include <bdn/Notifier.h>
#include <bdn/ClickEvent.h>


namespace bdn
{

class ButtonBase : public View
{
public:
    ButtonBase()
    {       
    }


    Property<String>& label()
    {
        return *_pLabel;
    }
    
	ReadProperty<String>& label() const
    {
        return *_pLabel;
    }
    
    
protected:

    void initButton(GtkWidget* pWidget, const String& label)
    {
        initView( pWidget );
        
        _pLabel = newObj< PropertyWithMainThreadDelegate<String> >( newObj<LabelDelegate>( GTK_BUTTON(pWidget) ), label );
    }

    class LabelDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<String>::IDelegate
	{
	public:
		LabelDelegate(GtkButton* pButton)
		{
			_pButton = pButton;
		}

		void set(const String& val) override
		{
			// this is only called from the main thread
			if(_pButton!=nullptr)
                gtk_button_set_label( _pButton, val.asUtf8Ptr() );
		}

		String get() const override
		{
            if(_pButton!=nullptr)
            {
                const gchar* p = gtk_button_get_label(_pButton);
                if(p!=nullptr)
                    return String(p);
            }
            
            return "";
		}

	protected:
		GtkButton* _pButton;
	};
    
    
    P< PropertyWithMainThreadDelegate<String> > _pLabel;
};
    
}

#endif