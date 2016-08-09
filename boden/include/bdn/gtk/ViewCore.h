#ifndef BDN_GTK_ViewCore_H_
#define BDN_GTK_ViewCore_H_

#include <bdn/IViewCore.h>
#include <bdn/View.h>
#include <bdn/gtk/UiProvider.h>
#include <bdn/gtk/util.h>

#include <gtk/gtk.h>

namespace bdn
{
namespace gtk
{

class ViewCore : public Base, BDN_IMPLEMENTS IViewCore
{
public:
    ViewCore(View* pOuterView, GtkWidget* pWidget)
    {
        _pOuterViewWeak = pOuterView;
        
        _pWidget = pWidget;
        
        setVisible( pOuterView->visible() );
        
        _addToParent();        
    }
    
    
	void setVisible(const bool& visible) override
    {
        gtk_widget_set_visible(_pWidget, visible ? TRUE : FALSE);
    }
        
	
	
	void setPadding(const UiMargin& padding)
    {
    }

	
	void setBounds(const Rect& bounds)
    {
        GtkAllocation alloc = rectToGtkRect(bounds, getGtkScaleFactor() );
        
        if(alloc.width<0)
            alloc.width = 0;
        if(alloc.height<0)
            alloc.height = 0;
        
        gtk_widget_set_size_request( _pWidget, alloc.width, alloc.height );
        
        P<View> pParentView = getOuterView()->getParentView();
        if(pParentView!=nullptr)
        {
            P<ViewCore> pParentViewCore = cast<ViewCore>( pParentView->getViewCore() );
            
            pParentViewCore->_moveChildViewCore( this, alloc.x, alloc.y );            
        }
    }


	
	int uiLengthToPixels(const UiLength& uiLength) const
    {
        return UiProvider::get().uiLengthToPixelsForWidget( getGtkWidget(), uiLength);
    }
	

	Margin uiMarginToPixelMargin(const UiMargin& margin) const
    {
        return UiProvider::get().uiMarginToPixelMarginForWidget( getGtkWidget(), margin);
    }

	

	Size calcPreferredSize() const
    {
        return _calcPreferredSize(-1, -1);
    }

	
	int calcPreferredHeightForWidth(int width) const
    {
        return _calcPreferredSize(width, -1).height;
    }

	
	int calcPreferredWidthForHeight(int height) const
    {
        return _calcPreferredSize(-1, height).width;
    }
	

	bool tryChangeParentView(View* pNewParent)
    {
        _addToParent();
        
        return true;
    }
    
    
    GtkWidget*  getGtkWidget() const
    {
        return _pWidget;
    }
    
    
    const View* getOuterView() const
    {
        return _pOuterViewWeak;
    }
    
    View* getOuterView()
    {
        return _pOuterViewWeak;
    }
    
    virtual void _addChildViewCore(ViewCore* pChildCore)
    {
        throw ProgrammingError("ViewCore::_addChildViewCore called on view that does not support child views.");
    }
    
    virtual void _moveChildViewCore(ViewCore* pChildCore, int gtkX, int gtkY)
    {
        throw ProgrammingError("ViewCore::_moveChildViewCore called on view that does not support child views.");
    }
        
        
    double getGtkScaleFactor() const
    {
        return gtk_widget_get_scale_factor(_pWidget);
    }
    

private:

    Size _calcPreferredSize(int forWidth, int forHeight) const
    {
        GtkRequisition minSize;
        GtkRequisition naturalSize;
        
        // we must clear our "size request". Otherwise the current size will be
        // the basis for the preferred size.
        gint oldWidth;
        gint oldHeight;
        gtk_widget_get_size_request( _pWidget, &oldWidth, &oldHeight);
                             
        gtk_widget_set_size_request( _pWidget, 0, 0);
        
        if(forWidth!=-1)
        {
            gint minHeight=0;
            gint naturalHeight=0;
            
            int forGtkWidth = forWidth / getGtkScaleFactor();
            
            if(forGtkWidth<0)
                forGtkWidth = 0;
            
            gtk_widget_get_preferred_height_for_width( _pWidget, forGtkWidth, &minHeight, &naturalHeight );
            
            naturalSize.width = forGtkWidth;
            naturalSize.height = naturalHeight;
        }
        else if(forHeight!=-1)
        {
            gint minWidth=0;
            gint naturalWidth=0;
            
            int forGtkHeight = forHeight / getGtkScaleFactor();
            
            if(forGtkHeight<0)
                forGtkHeight = 0;
                        
            gtk_widget_get_preferred_width_for_height( _pWidget, forGtkHeight, &minWidth, &naturalWidth );
            
            naturalSize.width = naturalWidth;
            naturalSize.height = forGtkHeight;
        }
        else        
            gtk_widget_get_preferred_size (_pWidget, &minSize, &naturalSize );
        
        // restore the old size
        gtk_widget_set_size_request( _pWidget, oldWidth, oldHeight);        
        
        Size size = gtkSizeToSize(naturalSize, getGtkScaleFactor() );
        
        // add the padding
        Margin padding = uiMarginToPixelMargin( getOuterView()->padding() );
        size += padding;
        
        return size;
    }
    

    void _addToParent()
    {
        P<View> pParent = getOuterView()->getParentView();
        if(pParent==nullptr)
        {
            // nothing to do. We are a top level window.            
        }
        else
        {        
            P<ViewCore> pParentCore = cast<ViewCore>( pParent->getViewCore() );
            if(pParentCore==nullptr)
            {
                // should never happen
                throw ProgrammingError("Error: constructed a gtk::ViewCore for a view whose parent does not have a core.");
            }
            
            pParentCore->_addChildViewCore( this );            
        }
    }
    


    GtkWidget*  _pWidget;
    View*       _pOuterViewWeak;
};



}
}

#endif
