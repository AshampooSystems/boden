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
        _outerViewWeak = pOuterView;
        
        _pWidget = pWidget;
        
        setVisible( pOuterView->visible() );
        
        _addToParent();        
    }
    
        
	void setVisible(const bool& visible) override
    {
        gtk_widget_set_visible(_pWidget, visible ? TRUE : FALSE);
    }
        
	
	
	void setPadding(const Nullable<UiMargin>& padding) override
    {
    }

	
	void setBounds(const Rect& bounds) override
    {
        GtkAllocation alloc = rectToGtkRect(bounds, getGtkScaleFactor() );
        
        if(alloc.width<0)
            alloc.width = 0;
        if(alloc.height<0)
            alloc.height = 0;
        
        gtk_widget_set_size_request( _pWidget, alloc.width, alloc.height );
        
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
        {
            P<View> pParentView = pView->getParentView();
            if(pParentView!=nullptr)
            {
                P<ViewCore> pParentViewCore = cast<ViewCore>( pParentView->getViewCore() );
            
                pParentViewCore->_moveChildViewCore( this, alloc.x, alloc.y );            
            }
        }
    }


	XXX
	int uiLengthToPixels(const UiLength& uiLength) const override
    {
        return UiProvider::get().uiLengthToPixelsForWidget( getGtkWidget(), uiLength);
    }
	
    XXX
	Margin uiMarginToPixelMargin(const UiMargin& margin) const override
    {
        return UiProvider::get().uiMarginToPixelMarginForWidget( getGtkWidget(), margin);
    }

	

	Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override
    {
        GtkRequisition resultSize;
        
        // we must clear our "size request". Otherwise the current size will be
        // the basis for the preferred size.
        gint oldWidth;
        gint oldHeight;
        gtk_widget_get_size_request( _pWidget, &oldWidth, &oldHeight);
        
        // invisible widgets do not take up any size in the layout.
        // So if it is 
        gboolean oldVisible = gtk_widget_get_visible(_pWidget);
        if(oldVisible==FALSE)
            gtk_widget_set_visible(_pWidget, TRUE);
            
                             
        gtk_widget_set_size_request( _pWidget, 0, 0);
        
        // we always need to know the unrestricted size of the widget
        GtkRequisition unrestrictedMinSize;
        GtkRequisition unrestrictedNaturalSize;
        gtk_widget_get_preferred_size (_pWidget, &unrestrictedMinSize, &unrestrictedNaturalSize );
        
        if(availableWidth!=-1 && canAdjustWidthToAvailableSpace() )
        {
            gint minHeight=0;
            gint naturalHeight=0;
            
            int forGtkWidth = availableWidth / getGtkScaleFactor();
            
            if(forGtkWidth<0)
                forGtkWidth = 0;
                
            gtk_widget_get_preferred_height_for_width( _pWidget, forGtkWidth, &minHeight, &naturalHeight );
            
            
            resultSize.height = naturalHeight;            
            
            // now we know our height for the case in which the width is exactly availableWidth.
            // However, availableWidth may actually be wider than what we need. Check that.
            if(forGtkWidth < unrestrictedNaturalSize.width)
            {
                // the unrestricted size exceeds the available size.
                // Note that it might seem like a good idea to use gtk_widget_get_preferred_width_for_height here
                // to find out if the widget really does use all the available space in the restricted case.
                // For example, text views that wrap their text to accomodate for availWidth will usually
                // not wrap exactly at availWidth, but slightly before that.
                // However, we cannot use gtk_widget_get_preferred_width_for_height to find out how much
                // space the view really needs. Since gtk_widget_get_preferred_width_for_height does not
                // have a paremeter to specify a width limit, the natural width we get will be exactly
                // the unrestricted width (since the height limit we specify is usually actually bigger than
                // the unrestricted height).
                // And the minimum width will be a width for a case in which the control tries to absolutely
                // minimize its width. For example, text views will wrap text as much as possible and also
                // try to choose the wrap point so that all lines are roughly the same width. That is not the
                // same text wrapping that the control used to calculate the naturalHeight for the case with the
                // restricted width.
                
                // So, there is no way to calculate the actual used amount of available space.
            
                resultSize.width = forGtkWidth;
            }
            else
                resultSize.width = unrestrictedNaturalSize.width;               
            
        }
        else if(availableHeight!=-1 && canAdjustHeightToAvailableSpace() )
        {
            gint minWidth=0;
            gint naturalWidth=0;
            
            int forGtkHeight = availableHeight / getGtkScaleFactor();
            
            if(forGtkHeight<0)
                forGtkHeight = 0;
                
            // see availableWidth!=-1 case for an explanation of what we do here.
            
            gtk_widget_get_preferred_width_for_height( _pWidget, forGtkHeight, &minWidth, &naturalWidth );
            
            
            resultSize.width = naturalWidth;            
            resultSize.height = std::min(forGtkHeight, unrestrictedNaturalSize.height);            
        }
        else
            resultSize = unrestrictedNaturalSize;
            
            
        // restore the old visibility
        if(oldVisible==FALSE)
            gtk_widget_set_visible(_pWidget, oldVisible);
        
        // restore the old size
        gtk_widget_set_size_request( _pWidget, oldWidth, oldHeight);        
        
        Size size = gtkSizeToSize(resultSize, getGtkScaleFactor() );
        

        Margin padding = _getPaddingDips();
        size += padding;
        
        return size;
    }
	
 
	bool tryChangeParentView(View* pNewParent) override
    {
        _addToParent();
        
        return true;
    }
    
    
    GtkWidget*  getGtkWidget() const
    {
        return _pWidget;
    }
    
    
    P<const View> getOuterViewIfStillAttached() const
    {
        return _outerViewWeak.toStrong();
    }
    
    P<View> getOuterViewIfStillAttached()
    {
        return _outerViewWeak.toStrong();
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
    
    
protected:    
    virtual Margin getDefaultPaddingPixels() const
    {
        return Margin();
    }
    
    
    /** Returns true if the view can adjust its width to fit into
		a certain size of available space.

		If this returns false then calcPreferredSize will ignore the
		availableWidth parameter.

		The default implementation returns false.
	*/
	virtual bool canAdjustWidthToAvailableSpace() const
	{
		return false;
	}

	/** Returns true if the view can adjust its height to fit into
		a certain size of available space.

		If this returns false then calcPreferredSize will ignore the
		availableHeight parameter.

		The default implementation returns false.
	*/
	virtual bool canAdjustHeightToAvailableSpace() const
	{
		return false;
	}
    

private:

    Margin _getPaddingPixels() const
    {
        P<const View> pView = getOuterViewIfStillAttached();
        Nullable<UiMargin> pad;
        if(pView!=nullptr)
            pad = pView->padding();

        if(pad.isNull())
            return getDefaultPaddingPixels();
        else            
            return uiMarginToPixelMargin( pad.get() );
    }

    

    void _addToParent()
    {
        P<View> pView = getOuterViewIfStillAttached();
        P<View> pParent;
        if(pView!=nullptr)
            pParent = pView->getParentView();
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
    WeakP<View> _outerViewWeak;
};



}
}

#endif
