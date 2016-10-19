#ifndef BDN_GTK_ViewCore_H_
#define BDN_GTK_ViewCore_H_

#include <bdn/IViewCore.h>
#include <bdn/View.h>
#include <bdn/PixelAligner.h>
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
    
    
    Rect adjustAndSetBounds(const Rect& requestedBounds) override
    {
        Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);
        
        GtkAllocation alloc = rectToGtkRect( adjustedBounds );
        
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
        
        return adjustedBounds;
    }
    
    
    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType) const override
    {
        // GTK uses DIPs, but with inteher values. So while there can be more than 1 pixel per DIP,
        // we cannot address them. So we have to adjust to DIP boundaries, rather than pixel boundaries.
        // Note that the GTK scale factor (factor from DIPs to pixels) is always an integer, so DIP
        // boundaries are always also pixel boundaries. But there may be additional pixel boundaries
        // between DIP boundaries.
        return PixelAligner( 1 ).alignRect(requestedBounds, positionRoundType, sizeRoundType);
    }


	double uiLengthToDips(const UiLength& uiLength) const override
    {        
        switch( uiLength.unit )
        {
        case UiLength::Unit::none:
            return 0;

        case UiLength::Unit::dip:
            return uiLength.value;

        case UiLength::Unit::em:
            return uiLength.value * getEmSizeDips();

        case UiLength::Unit::sem:
			return uiLength.value * getSemSizeDips();

        default:
			throw InvalidArgumentError("Invalid UiLength unit passed to ViewCore::uiLengthToDips: "+std::to_string((int)uiLength.unit) );
        }
	}

    
	Margin uiMarginToDipMargin(const UiMargin& margin) const override
    {
        return Margin(
            uiLengthToDips(margin.top),
            uiLengthToDips(margin.right),
            uiLengthToDips(margin.bottom),
            uiLengthToDips(margin.left) );
    }

	

	Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override
    {
        GtkRequisition resultSize;
        
        //std::cout << typeid(*this).name() << " available: " << availableWidth << "x" << availableHeight << std::endl;
        
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
            
        Margin padding = _getPaddingIntegerDips();
           
                             
        gtk_widget_set_size_request( _pWidget, 0, 0);
        
        // we always need to know the unrestricted size of the widget
        GtkRequisition unrestrictedMinSize;
        GtkRequisition unrestrictedNaturalSize;
        gtk_widget_get_preferred_size (_pWidget, &unrestrictedMinSize, &unrestrictedNaturalSize );
        
        if(availableWidth!=-1 && canAdjustWidthToAvailableSpace() )
        {
            availableWidth -= padding.left + padding.right;
            
            if(availableWidth>=unrestrictedNaturalSize.width)
            {
                // the unrestructed width  is smaller or equal to the available width.
                // So we can just use that.
                resultSize = unrestrictedNaturalSize;                
            }
            else
            {            
                gint minHeight=0;
                gint naturalHeight=0;
            
                // GTK also uses DIPs. So no scaling necessary
                int forGtkWidth = (int)std::lround(availableWidth);
                
                if(forGtkWidth<0)
                    forGtkWidth = 0;
                    
                gtk_widget_get_preferred_height_for_width( _pWidget, forGtkWidth, &minHeight, &naturalHeight );
                
                
                resultSize.height = naturalHeight;            
                
                // now we know our height for the case in which the width is exactly availableWidth.
                // Note that we also know that availableWidth is smaller than the natural size (see check above).
                // So we know that the control pretty much uses all the available space here.
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
                resultSize.width = availableWidth;                   
            }
        }
        else if(availableHeight!=-1 && canAdjustHeightToAvailableSpace() )
        {
            availableHeight -= padding.top + padding.bottom;
            
            if(availableHeight>=unrestrictedNaturalSize.width)
            {
                // the unrestructed height is smaller or equal to the available height.
                // So we can just use that.
                resultSize = unrestrictedNaturalSize;                
            }
            else
            {
                gint minWidth=0;
                gint naturalWidth=0;
                
                // no scaling necessary
                int forGtkHeight = (int)std::lround(availableHeight);
                
                if(forGtkHeight<0)
                    forGtkHeight = 0;
                    
                // see availableWidth!=-1 case for an explanation of what we do here.
                
                gtk_widget_get_preferred_width_for_height( _pWidget, forGtkHeight, &minWidth, &naturalWidth );
                
                
                resultSize.width = naturalWidth;            
                resultSize.height = availableHeight;std::min(forGtkHeight, unrestrictedNaturalSize.height);            
            }
        }            
        else
            resultSize = unrestrictedNaturalSize;
            
            
        // restore the old visibility
        if(oldVisible==FALSE)
            gtk_widget_set_visible(_pWidget, oldVisible);
        
        // restore the old size
        gtk_widget_set_size_request( _pWidget, oldWidth, oldHeight);        
        
        Size size = gtkSizeToSize(resultSize);        

        size += padding;
        
        P<const View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
            size = pView->applySizeConstraints(size);
                
        //std::cout << typeid(*this).name() << " preferred: " << size.width << "x" << size.height << " padding: " << padding.top << "," << padding.right <<","<< padding.bottom <<","<< padding.left << std::endl;
        
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
    virtual Margin getDefaultPaddingDips() const
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

    Margin _getPaddingIntegerDips() const
    {
        P<const View> pView = getOuterViewIfStillAttached();
        Nullable<UiMargin> pad;
        if(pView!=nullptr)
            pad = pView->padding();
            
        Margin paddingDips;

        if(pad.isNull())
            paddingDips = getDefaultPaddingDips();
        else            
            paddingDips = uiMarginToDipMargin( pad.get() );
            
        // round up to full integers, since GTK cannot represent anything else
        paddingDips.top = std::ceil(paddingDips.top);
        paddingDips.right = std::ceil(paddingDips.right);
        paddingDips.bottom = std::ceil(paddingDips.bottom);
        paddingDips.left = std::ceil(paddingDips.left);
        
        return paddingDips;
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
    
    double getEmSizeDips() const
    {
        if(_emDipsIfInitialized==-1)
        {
            double size=0;

            if(_pWidget!=nullptr)
            {
                GtkStyleContext* pStyleContext = gtk_widget_get_style_context( _pWidget );
                if(pStyleContext!=nullptr)
                {
                    const PangoFontDescription* pFontDesc = nullptr;
                    gtk_style_context_get(pStyleContext, GTK_STATE_FLAG_NORMAL, "font", &pFontDesc, NULL);
                    if(pFontDesc!=nullptr)
                    {
                        size = pango_font_description_get_size(pFontDesc);
                        if(size>0)
                        {
                            size /= PANGO_SCALE;
                            
                            // might be in points or pixels (what they call "absolute")
                            if( ! pango_font_description_get_size_is_absolute(pFontDesc) )
                            {
                                // the unit is points. Convert to DIPs
                                size = size / 72.0 * 96.0;
                            }
                        }
                    }
                }
            }

            if(size<=0)
            {
                // size not set explicitly => system font size is used => em same as sem
                size = UiProvider::get().getSemSizeDips();
            }

            _emDipsIfInitialized = size;
        }

        return _emDipsIfInitialized;
    }


    double getSemSizeDips() const
    {
        if(_semDipsIfInitialized==-1)
            _semDipsIfInitialized = UiProvider::get().getSemSizeDips();

        return _semDipsIfInitialized;
    }
    

    GtkWidget*  _pWidget;
    WeakP<View> _outerViewWeak;

    mutable double      _emDipsIfInitialized = -1;
    mutable double      _semDipsIfInitialized = -1;
};



}
}

#endif
