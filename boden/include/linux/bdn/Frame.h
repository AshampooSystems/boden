#ifndef BDN_Frame_H_
#define BDN_Frame_H_

#include <bdn/init.h>
#include <bdn/IWindow.h>

#include <gtk/gtk.h>

namespace bdn
{

class Frame : public Base, virtual public IWindow
{
public:
	Frame(const String& title)
	{
	    _pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	    setTitle(title);
	}

	~Frame()
	{
		if(_pWindow!=nullptr)
		{
            gtk_widget_destroy( _pWindow );
			_pWindow = nullptr;
		}
	}



	void setTitle(const String& title)
	{
        gtk_window_set_title( GTK_WINDOW( _pWindow ), title.asUtf8Ptr() );
	}


	virtual void show(bool visible = true) override
	{
        if(visible)
            gtk_widget_show(_pWindow);
        else
            gtk_widget_hide(_pWindow);
	}

	virtual void hide() override
	{
		show(false);
	}

protected:
	GtkWidget* _pWindow;
};


}

#endif
