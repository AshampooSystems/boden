#ifndef BDN_View_H_
#define BDN_View_H_

#include <bdn/IWindow.h>
#include <bdn/PropertyWithMainThreadDelegate.h>

#include <gtk/gtk.h>

namespace bdn
{

class View : public Base, BDN_IMPLEMENTS IWindow
{
public:
    View()
    {
    }
    
    
    Property<bool>& visible() override
    {
        return *_pVisible;
    }
    
	ReadProperty<bool>& visible() const override
    {
        return *_pVisible;
    }
           
           
    GtkWidget* getGtkWidget()
    {
        return _pWidget;
    }

protected:

    void initView(GtkWidget* pWidget, bool initiallyVisible=true )
    {
        _pWidget = pWidget;
        
        _pVisible = newObj< PropertyWithMainThreadDelegate<bool> >( newObj<VisibleDelegate>( _pWidget ), initiallyVisible );
    }


    class VisibleDelegate : public Base, BDN_IMPLEMENTS PropertyWithMainThreadDelegate<bool>::IDelegate
	{
	public:
		VisibleDelegate(GtkWidget* pWidget)
		{
			_pWidget = pWidget;
		}

		void set(const bool& val) override
		{
			// this is only called from the main thread
			if(_pWidget!=nullptr)
                gtk_widget_set_visible(_pWidget, val ? TRUE : FALSE);
		}

		bool get() const override
		{
            if(_pWidget!=nullptr)
                return gtk_widget_get_visible(_pWidget);
            
            return false;
		}

	protected:
		GtkWidget* _pWidget;
	};


    GtkWidget*  _pWidget;
    
    P<PropertyWithMainThreadDelegate<bool> > _pVisible;
};


}

#endif
