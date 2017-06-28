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
        // XXX
        OutputDebugString( (String(typeid(*this).name())+".needSizingInfoUpdate()\n" ).asWidePtr() );

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


    void childSizingInfoChanged(View* pChild) override
    {
        // we do not do anything here. Windows takes care of propagating the sizing info changes
        // to the parent views.
    }

    void needLayout() override
    {
        // XXX
        OutputDebugString( (String(typeid(*this).name())+".needLayout()\n" ).asWidePtr() );

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
    

    Rect adjustAndSetBounds(const Rect& requestedBounds)
    {        
        BDN_WINUWP_TO_STDEXC_BEGIN;

		// we can only control the position of a control indirectly. There is the Arrange
		// method, but it does not actually work outside of a UWP Layout cycle.

        // first adjust the bounds
        Rect adjustedBounds = adjustBounds( requestedBounds, RoundType::nearest, RoundType::nearest);

        if(adjustedBounds!=_currBounds || !_currBoundsInitialized)
        {
            _currBounds = adjustedBounds;
            _currBoundsInitialized = true;            
        }

        // we assume that we are called during a parent layout cycle and that Arrange actually works.
        if(_pFrameworkElement!=nullptr)
        {
            ::Windows::Foundation::Rect winRect = rectToUwpRect(_currBounds);

            _pFrameworkElement->Arrange( winRect );
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
        // XXX
        OutputDebugString( (String(typeid(*this).name())+".calcPreferredSize("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+"\n" ).asWidePtr() );

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
                // XXX measureAvailSize.applyMaximum( pOuter->preferredSizeMaximum() );

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
            
            // XXX
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
            

            // XXX
            OutputDebugString( ("/"+String(typeid(*this).name())+".calcPreferredSize()\n" ).asWidePtr() );

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
        // nothing to do here. Windows takes care of updating our layout.
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



	::Windows::UI::Xaml::FrameworkElement^ _pFrameworkElement;

	WeakP<View> 			_outerViewWeak;	// weak by design

	ViewCoreEventForwarder^ _pEventForwarder;

    mutable double _emSizeDipsIfInitialized = -1;
    mutable double _semSizeDipsIfInitialized = -1;

    bool _currBoundsInitialized = false;
    Rect _currBounds;
    
    bool _inUwpLayoutOperation = false;
};


}
}

#endif

