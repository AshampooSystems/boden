#ifndef BDN_GTK_ContainerViewCore_H_
#define BDN_GTK_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/gtk/ViewCore.h>

namespace bdn
{
namespace gtk
{

class ContainerViewCore : public ViewCore
{
public:
	ContainerViewCore(	View* pOuter)
		: ViewCore(pOuter, gtk_layout_new(nullptr, nullptr)  )
	{
	}


	void setPadding(const Nullable<UiMargin>& uiPadding) override
	{
		// the outer class automatically handles our own padding. So nothing to do here.
	}

		
	Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override
	{
		// this core function should never have been called.
		// The outer window is responsible for everything layout-related.
		throw ProgrammingError("ContainerView::calcPreferredSize must be overloaded in derived class.");
	}
    
    void _addChildViewCore(ViewCore* pChildCore) override
    {
        P<View> pChildView = pChildCore->getOuterViewIfStillAttached();

        Rect bounds;
        if(pChildView!=nullptr)
            bounds = pChildView->bounds();
        
        GdkRectangle rect = rectToGtkRect(bounds, getGtkScaleFactor() );
        
        gtk_layout_put( GTK_LAYOUT(getGtkWidget()), pChildCore->getGtkWidget(), rect.x, rect.y);
    }
    
    void _moveChildViewCore(ViewCore* pChildCore, int gtkX, int gtkY) override
    {
        gtk_layout_move( GTK_LAYOUT(getGtkWidget()), pChildCore->getGtkWidget(), gtkX, gtkY);
    }    

};



}
}

#endif
