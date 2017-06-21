#ifndef BDN_WINUWP_ChildViewCore_H_
#define BDN_WINUWP_ChildViewCore_H_

#include <bdn/View.h>

#include <bdn/winuwp/util.h>
#include <bdn/winuwp/IViewCoreParent.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/IFrameworkElementOwner.h>

#include <bdn/PixelAligner.h>

#include <cassert>

namespace bdn
{
namespace winuwp
{

/** Base implementation for Windows Universal view cores that are children
    of another view (see IViewCore).
	Note that top level windows do not derive from this - they provider their own
	implementation of IViewCore.	
*/
class ChildViewCore : public Base, BDN_IMPLEMENTS IViewCore, BDN_IMPLEMENTS IFrameworkElementOwner
{
public:	
	/** Used internally.*/
	ref class ViewCoreEventForwarder : public Platform::Object
	{
	internal:
		ViewCoreEventForwarder(ChildViewCore* pParent)
		{
			_pParentWeak = pParent;
		}

		ChildViewCore* getViewCoreIfAlive()
		{
			return _pParentWeak;
		}

	public:
		void dispose()
		{
			_pParentWeak = nullptr;
		}

		void sizeChanged( Platform::Object^ pSender,  ::Windows::UI::Xaml::SizeChangedEventArgs^ pArgs)
		{
            BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			ChildViewCore* pViewCore = getViewCoreIfAlive();
			if(pViewCore!=nullptr)
				pViewCore->_uwpSizeChanged();

            BDN_WINUWP_TO_PLATFORMEXC_END
		}

		void layoutUpdated( Platform::Object^ pSender, Platform::Object^ pArgs )
		{
            BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			ChildViewCore* pViewCore = getViewCoreIfAlive();
			if(pViewCore!=nullptr)
				pViewCore->_uwpLayoutUpdated();

            BDN_WINUWP_TO_PLATFORMEXC_END
		}

	private:
		ChildViewCore* _pParentWeak;
	};

	ChildViewCore(	View* pOuterView, 
				::Windows::UI::Xaml::FrameworkElement^ pFrameworkElement,
				ViewCoreEventForwarder^ pEventForwarder )
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		_outerViewWeak = pOuterView;
		_pFrameworkElement = pFrameworkElement;

		_pEventForwarder = pEventForwarder;

		// when windows updates the size of the content canvas then that
		// means that we have to update our layout.
		_pFrameworkElement->SizeChanged += ref new ::Windows::UI::Xaml::SizeChangedEventHandler( _pEventForwarder, &ViewCoreEventForwarder::sizeChanged );

		_pFrameworkElement->LayoutUpdated += ref new Windows::Foundation::EventHandler<Platform::Object^>
				( _pEventForwarder, &ViewCoreEventForwarder::layoutUpdated );

		setVisible( pOuterView->visible() );
				
		_addToParent( pOuterView->getParentView() );

        BDN_WINUWP_TO_STDEXC_END;
	}

	~ChildViewCore()
	{
		_pEventForwarder->dispose();
	}

	void setVisible(const bool& visible) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        try
        {
		    _pFrameworkElement->Visibility = visible ? ::Windows::UI::Xaml::Visibility::Visible : ::Windows::UI::Xaml::Visibility::Collapsed;
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // view was already destroyed. Ignore this.
        }

        BDN_WINUWP_TO_STDEXC_END;
	}


    void needSizingInfoUpdate() override
    {
        // we leave the layout coordination up to windows. See doc_input/winuwp_layout.md for more information on why
        // this is.
        BDN_WINUWP_TO_STDEXC_BEGIN;

        try
        {
		    _pFrameworkElement->InvalidateMeasure();
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // view was already destroyed. Ignore this.
        }

        BDN_WINUWP_TO_STDEXC_END;
    }


    void needLayout() override
    {
        // we leave the layout coordination up to windows. See doc_input/winuwp_layout.md for more information on why
        // this is.
        BDN_WINUWP_TO_STDEXC_BEGIN;

        try
        {
		    _pFrameworkElement->InvalidateArrange();
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // view was already destroyed. Ignore this.
        }

        BDN_WINUWP_TO_STDEXC_END;
    }

    


    /** An internal helper class that will notify the core of the specified view that
        it is currently being layouted by its parent.
        
        The flag will be set while the InUwpLayoutOperation object exists and
        will be cleared when it is destroyed.
        */
    class InUwpLayoutOperation_
    {
    public:
        InUwpLayoutOperation_(View* pView)
        {
            _pCore = cast<ChildViewCore>( pView->getViewCore() );

            if(_pCore!=nullptr)
                _pCore->setInUwpLayoutOperation(true);
        }

        ~InUwpLayoutOperation_()
        {
            if(_pCore!=nullptr)
                _pCore->setInUwpLayoutOperation(false);
        }

    private:
        P<ChildViewCore> _pCore;
    };
    friend class InUwpLayoutOperation;
    

   
    
    /** Sets the view's position and size, after adjusting the specified values
        to ones that are compatible with the underlying view implementation. The bounds are specified in DIP units
        and refer to the parent view's coordinate system.
        
        See adjustBounds() for more information about the adjustments that are made.
        
        Note that the adjustments are made with a "nearest valid" policy. I.e. the position and size are set
        to the closest valid value. This can mean that the view ends up being bigger or smaller than requested.
        If you need more control over which way the adjustments are made then you should pre-adjust the bounds
        with adjustBounds().

        The function returns the adjusted bounds that are actually used.
        */
    Rect adjustAndSetBounds(const Rect& requestedBounds)
    {        
        BDN_WINUWP_TO_STDEXC_BEGIN;

		// we can only control the position of a control indirectly. While there is the Arrange
		// method, it does not actually work outside of a UWP Layout cycle.
		
		// Positions can only be manually set for children of a Canvas container.
		// We have structured our views in such a way that all child views have a Canvas
		// container, so that is not a problem.

		// So we have to set the Canvas.left and Canvas.top custom properties
		// for this child view.

        // first adjust the bounds
        Rect adjustedBounds = adjustBounds( requestedBounds, RoundType::nearest, RoundType::nearest);

        if(adjustedBounds!=_currBounds || !_currBoundsInitialized)
        {
            _currBounds = adjustedBounds;
            _currBoundsInitialized = true;

            try
            {
                // note that UWP also uses DIPs as the fundamental UI unit. So no conversion necessary.
		        ::Windows::UI::Xaml::Controls::Canvas::SetLeft( _pFrameworkElement, adjustedBounds.x );
		        ::Windows::UI::Xaml::Controls::Canvas::SetTop( _pFrameworkElement, adjustedBounds.y );

                //XXX temp disabledif(_inUwpLayoutOperation)
                //{
                    // we are currently in a UWP layout operation. This happens when the adjustAndSetBounds
                    // call is called from inside a UWP UIElement::ArrangeOverride method (for example in a scrollview).

                    // In this case we can and should use the Arrange method to set our size and position.
                    ::Windows::Foundation::Rect winBounds = rectToUwpRect(adjustedBounds);

                    _pFrameworkElement->Arrange(winBounds);
                /*}
                else
                {
                    // The size is set by manipulating the Width and Height property.		
		            _pFrameworkElement->Width = doubleToUwpDimension( adjustedBounds.width );
		            _pFrameworkElement->Height = doubleToUwpDimension( adjustedBounds.height );            

                    // ensure that the changes are reflected as soon as possible.
                    // Note that almost everything works well if we omit this, but the test code has a hard time
                    // verifying the results of the operations. So for the time being we do it this way.
                    // If this turns out to be too slow and inefficient then we can try to find a better way
                    // at that point in time.

                    _pFrameworkElement->UpdateLayout();
                }*/
            }
            catch(::Platform::DisconnectedException^ e)
            {
                // view was already destroyed. Ignore this.
            }
        }

        return adjustedBounds;

        BDN_WINUWP_TO_STDEXC_END;
    }

    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const
    {
        // UWP also uses DIPs and floating point values, so it might seem as if no conversion were necessary.
        // However, UWP also implicitly rounds to full pixels (unless UseLayoutRounding is manually set to false).
        // We want to make this implicit process explicit and do the proper rounding in our code, with our parameters.

        double scaleFactor = UiProvider::get().getUiScaleFactor();

        // the scale factor indicates how many physical pixels there are per DIP. So we want to round to a multiple of that.
        Rect adjustedBounds = PixelAligner(scaleFactor).alignRect(requestedBounds, positionRoundType, sizeRoundType);

        return adjustedBounds;
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


	bool tryChangeParentView(View* pNewParent) override
	{
		_addToParent(pNewParent);
	
		return true;
	}


	void updateOrderAmongSiblings()
	{		
		// we do not care about ordering
	}


	
	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
		BDN_WINUWP_TO_STDEXC_BEGIN;

        // Most views will clip the size returned by Measure to never
		// exceed the specified max width or height (even though Measure is actually
		// documented to return a bigger size if the view cannot be made small enough
		// to fit).

        Size measureAvailSize( availableSpace );

        try
        {
            Size preferredSizeHint( Size::none() );

            P<View> pOuter = getOuterViewIfStillAttached();
            if(pOuter!=nullptr)
            {
                // preferredSizeMaximum is a hard limit, which is exactly how most UWP controls
                // interpret the available size. So we incorporate it.
                measureAvailSize.applyMaximum( pOuter->preferredSizeMaximum() );

                preferredSizeHint = pOuter->preferredSizeHint();
            }

            float measureAvailWidthFloat;
		    float measureAvailHeightFloat;
            		
		    if( !std::isfinite(measureAvailSize.width) || !canAdjustWidthToAvailableSpace() )
			    measureAvailWidthFloat = std::numeric_limits<float>::infinity();
		    else
			    measureAvailWidthFloat = doubleToUwpDimension( measureAvailSize.width );

		    if( !std::isfinite(measureAvailSize.height) || !canAdjustHeightToAvailableSpace() )
			    measureAvailHeightFloat = std::numeric_limits<float>::infinity();
		    else
			    measureAvailHeightFloat = doubleToUwpDimension( measureAvailSize.height );

            if(measureAvailWidthFloat<0)
			    measureAvailWidthFloat = 0;
		    if(measureAvailHeightFloat<0)
			    measureAvailHeightFloat = 0;
            

		    // tell the element that it has a huge available size.
		    // The docs say that one can pass Double::PositiveInfinity here as well, but apparently that constant
		    // is not available in C++. And std::numeric_limits<float>::infinity() does not seem to work.

       
		    ::Windows::UI::Xaml::Visibility oldVisibility = _pFrameworkElement->Visibility;
		    if(oldVisibility != ::Windows::UI::Xaml::Visibility::Visible)
		    {
			    // invisible elements all report a zero size. So we must make the element temporarily visible
			    _pFrameworkElement->Visibility = ::Windows::UI::Xaml::Visibility::Visible;			
		    }

		    
		    // the Width and Height properties indicate to the layout process how big we want to be.
		    // If they are set then they are incorporated into the DesiredSize measurements.
            // So it sounds like they are analogous to our preferredSizeHint.
            // However, for many views the "hint size" is seen as an exact size that Measure
            // will simply return unchanged. I.e. we will never get anything below this size if
            // we set it. So we cannot generally use this as the hint.
            // In the default case we do not pass the hint on to the control. The specific subclasses
            // of view need to implement the hinting for their specific case with the knowledge what
            // the specific control will do with this information.
            
            // XXX disabled to test out layout stuff
            //_pFrameworkElement->Width = std::numeric_limits<double>::quiet_NaN();
            //_pFrameworkElement->Height = std::numeric_limits<double>::quiet_NaN();
            
            //_pFrameworkElement->InvalidateMeasure();

		    _pFrameworkElement->Measure( ::Windows::Foundation::Size( measureAvailWidthFloat, measureAvailHeightFloat ) );

		    ::Windows::Foundation::Size desiredSize = _pFrameworkElement->DesiredSize;
		
		    Size size = uwpSizeToSize(desiredSize);

		    if(oldVisibility != ::Windows::UI::Xaml::Visibility::Visible)
			    _pFrameworkElement->Visibility = oldVisibility;

            if(pOuter!=nullptr)
            {
                size.applyMinimum( pOuter->preferredSizeMinimum() );

                // clip to the maximum again. We never want that to be exceeded, even
                // if the content does not fit.
                size.applyMaximum( pOuter->preferredSizeMaximum() );
            }

            return size;
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // view was already destroyed. Ignore this and return zero size
            return Size();
        }
        
        BDN_WINUWP_TO_STDEXC_END;
	}


	/** Returns the XAML FrameworkElement object for this view.*/
	::Windows::UI::Xaml::FrameworkElement^ getFrameworkElement() override
	{
		return _pFrameworkElement;
	}


	/** Returns a pointer to the outer view object that is associated with this core.
        Can return null if the core has been disposed (i.e. if it is not connected
        to an outer view anymore).*/
	P<View> getOuterViewIfStillAttached() const
	{
		return _outerViewWeak.toStrong();
	}

protected:

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
	
	ViewCoreEventForwarder^ getViewCoreEventForwarder()
	{
		return _pEventForwarder;
	}

	virtual void _uwpSizeChanged()
	{
		// do not layout here. We do that in _uwpLayoutUpdated.		
	}

	virtual void _uwpLayoutUpdated()
	{
        P<View> pOuterView = getOuterViewIfStillAttached();
        
        if(_uwpLayoutTriggersOurLayout)
        {
		    // Xaml has done a layout cycle. At this point all the controls should know their
		    // desired sizes. So this is when we schedule our layout updated            
            if(pOuterView!=nullptr)
                pOuterView->needLayout();
        }

        if(_updateSizingInfoAfterNextUwpLayout)
		{
			_updateSizingInfoAfterNextUwpLayout = false;

            if(pOuterView!=nullptr)
			    pOuterView->needSizingInfoUpdate();
		}
	}


    double getEmSizeDips() const
    {
        if(_emSizeDipsIfInitialized==-1)
        {
            // FrameworkElement does not have a FontSize property. Only Control and TextBlock
            // objects have a font size.

            ::Windows::UI::Xaml::Controls::Control^ pControl = dynamic_cast<::Windows::UI::Xaml::Controls::Control^>(_pFrameworkElement);
            if(pControl!=nullptr)
                _emSizeDipsIfInitialized = pControl->FontSize;
            else
            {
                ::Windows::UI::Xaml::Controls::TextBlock^ pTextBlock = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>(_pFrameworkElement);
                if(pTextBlock!=nullptr)
                    _emSizeDipsIfInitialized = pTextBlock->FontSize;
                else
                {
                    // use the default font size (which is documented as being 11 DIPs).
                    _emSizeDipsIfInitialized = 11;
                }
            }
        }

        return _emSizeDipsIfInitialized;
    }

    double getSemSizeDips() const
    {
        if(_semSizeDipsIfInitialized==-1)
            _semSizeDipsIfInitialized = UiProvider::get().getSemSizeDips();

        return _semSizeDipsIfInitialized;
    }


protected:

    void scheduleSizingInfoUpdateAfterNextUwpLayout()
    {
        _updateSizingInfoAfterNextUwpLayout = true;
    }

    /** Sets an internal parameter that controls whether or not a layout
        of the view is automatically scheduled after a UWP layout operation
        has finished for the view.
        
        The default is true.
        */
    void setUwpLayoutTriggersOurLayout(bool autoTrigger)
    {
        _uwpLayoutTriggersOurLayout = autoTrigger;
    }
    
		
private:
	void _addToParent(View* pParentView)
	{
		if(pParentView==nullptr)
		{
			// classes derived from ViewCore MUST have a parent. Top level windows do not
			// derive from ViewCore.
			throw ProgrammingError("bdn::winuwp::ViewCore constructed for a view that does not have a parent.");
		}

		P<IViewCore> pParentCore = pParentView->getViewCore();
		if(pParentCore==nullptr)
		{
			// this should not happen. The parent MUST have a core - otherwise we cannot
			// initialize ourselves.
			throw ProgrammingError("bdn::winuwp::ViewCore constructed for a view whose parent does not have a core.");
		}

		cast<IViewCoreParent>( pParentCore )->addChildUiElement( _pFrameworkElement );
	}


    void setInUwpLayoutOperation(bool inLayoutOp)
    {
        _inUwpLayoutOperation = inLayoutOp;
    }



    ref class UwpContainerView sealed : public ::Windows::UI::Xaml::Controls::Canvas
    {
    public:
        UwpContainerView()
        {
        }

    protected:
        ::Windows::Foundation::Size MeasureOverride(::Windows::Foundation::Size availableSize) override
        {
            if(_pCore!=nullptr)
            {
                // forward to the core
                return _pCore->uwpMeasureOverride(availableSize);
            }
            else
                return ::Windows::UI::Xaml::Controls::Canvas::MeasureOverride(availableSize);
        }


        ::Windows::Foundation::Size ArrangeOverride(::Windows::Foundation::Size finalSize) override
        {
            if(_pCore!=nullptr)
                return _pCore->uwpArrangeOverride(finalSize);
            else
                return ::Windows::UI::Xaml::Controls::Canvas::ArrangeOverride(finalSize);
        }


    private:
        void setCore(ContainerViewCore* pCore)
        {
            _pCore = pCore;
        }

        // weak by design
        ContainerViewCore* _pCore = nullptr;

        friend class ContainerViewCore;
    };
    friend ref class UwpContainerView;

	::Windows::UI::Xaml::FrameworkElement^ _pFrameworkElement;

	WeakP<View> 			_outerViewWeak;	// weak by design

	ViewCoreEventForwarder^ _pEventForwarder;

    mutable double _emSizeDipsIfInitialized = -1;
    mutable double _semSizeDipsIfInitialized = -1;

    bool _currBoundsInitialized = false;
    Rect _currBounds;
    
    bool _uwpLayoutTriggersOurLayout = true;
    bool _updateSizingInfoAfterNextUwpLayout = true;

    bool _inUwpLayoutOperation = false;
};


}
}

#endif

