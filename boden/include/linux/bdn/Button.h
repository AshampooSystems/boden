#ifndef BDN_Button_H_
#define BDN_Button_H_


#include <bdn/Base.h>
#include <bdn/Frame.h>
#include <bdn/Notifier.h>
#include <bdn/ClickEvent.h>


namespace bdn
{

	class Button : public Base, virtual public IWindow
	{
	public:
		Button(Frame* pParent, const String& label)
		{
            _pButton = gtk_button_new();

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


		void generateClick()
		{
			ClickEvent evt(this);

			_onClick.notify(evt);
		}

		Notifier<const ClickEvent&>& onClick()
		{
			return _onClick;
		}

	protected:

		GtkWidget* _pButton;

		Notifier<const ClickEvent&> _onClick;
	};


}

#endif
