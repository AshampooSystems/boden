#ifndef BDN_GTK_WindowCore_H_
#define BDN_GTK_WindowCore_H_

#include <bdn/gtk/ViewCore.h>
#include <bdn/IWindowCore.h>

#include <gtk/gtk.h>

namespace bdn
{
namespace gtk
{
    

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore
{
        public:
/*
	WindowCore(const String& title)
	{
	    initView( gtk_window_new(GTK_WINDOW_TOPLEVEL), false );
        
        _pClientContainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add( GTK_CONTAINER( _pWidget ), _pClientContainer );
        
        gtk_widget_show( _pClientContainer );

        _pTitle = newObj< PropertyWithMainThreadDelegate<String> >( newObj<TitleDelegate>( GTK_WINDOW(_pWidget) ), title );
        
	}

	~WindowCore()
	{
		if(_pWidget!=nullptr)
		{
            gtk_widget_destroy( _pWidget );
			_pWidget = nullptr;
		}
	}
    
    
	Property<String>& title() override
    {
        return *_pTitle;
    }
    
	ReadProperty<String>& title() const override
    {
        return *_pTitle;
    }
    
    GtkWidget* getClientContainer()
    {
        return _pClientContainer;
    }



protected:

    class TitleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<String>::IDelegate
	{
	public:
		TitleDelegate(GtkWindow* pWindow)
		{
			_pWindow = pWindow;
		}

		void set(const String& val) override
		{
			// this is only called from the main thread
			if(_pWindow!=nullptr)
                gtk_window_set_title( _pWindow, val.asUtf8Ptr() );
		}

		String get() const override
		{
            if(_pWindow!=nullptr)
            {
                const gchar* p = gtk_window_get_title(_pWindow);
                if(p!=nullptr)
                    return String(p);
            }
            
            return "";
		}

	protected:
		GtkWindow* _pWindow;
	};
    
    
    P< PropertyWithMainThreadDelegate<String> > _pTitle;
    
    GtkWidget* _pClientContainer;
     */
};



}
}

#endif
