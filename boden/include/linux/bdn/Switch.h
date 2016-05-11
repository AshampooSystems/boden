#ifndef BDN_Switch_H_
#define BDN_Switch_H_


#include <bdn/Base.h>
#include <bdn/Frame.h>

namespace bdn
{

	class Switch : public Base, virtual public IWindow
	{
	public:
		Switch(Frame* pParent, const String& label)
		{
            _pButton = gtk_check_button_new();

            setLabel(label);
		}

		void setLabel(const String& label)
		{
            gtk_button_set_label( GTK_BUTTON(_pButton), label.asUtf8Ptr() );
		}


        virtual void show(bool visible = true) override
        {
            if(visible)
                gtk_widget_show(_pButton);
            else
                gtk_widget_hide(_pButton);
        }

        virtual void hide() override
        {
            show(false);
        }


	protected:

		GtkWidget* _pButton;
	};


}

#endif
