#ifndef BDN_WINUWP_ChildViewCore_H_
#define BDN_WINUWP_ChildViewCore_H_

#include <bdn/View.h>

#include <bdn/winuwp/util.h>
#include <bdn/winuwp/IViewCoreParent.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/IFrameworkElementOwner.h>

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
				pViewCore->_sizeChanged();

            BDN_WINUWP_TO_PLATFORMEXC_END
		}

		void layoutUpdated( Platform::Object^ pSender, Platform::Object^ pArgs )
		{
            BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			ChildViewCore* pViewCore = getViewCoreIfAlive();
			if(pViewCore!=nullptr)
				pViewCore->_layoutUpdated();

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

		_pOuterViewWeak = pOuterView;
		_pFrameworkElement = pFrameworkElement;

		_pEventForwarder = pEventForwarder;

		// when windows updates the size of the content canvas then that
		// means that we have to update our layout.
		_pFrameworkElement->SizeChanged += ref new ::Windows::UI::Xaml::SizeChangedEventHandler( _pEventForwarder, &ViewCoreEventForwarder::sizeChanged );

		_pFrameworkElement->LayoutUpdated += ref new Windows::Foundation::EventHandler<Platform::Object^>
				( _pEventForwarder, &ViewCoreEventForwarder::layoutUpdated );

		setVisible( pOuterView->visible() );
				
		_addToParent( _pOuterViewWeak->getParentView() );

        BDN_WINUWP_TO_STDEXC_END;
	}

	~ChildViewCore()
	{
		_pEventForwarder->dispose();
	}

    void dispose() override
    {
        _pOuterViewWeak = nullptr;
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
	
	void setBounds(const Rect& bounds) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		// we can only control the position of a control indirectly. While there is the Arrange
		// method, it does not actually work outside of a Layout call.
		
		// Positions can only be manually set for children of a Canvas container.
		// We have structured our views in such a way that all child views have a Canvas
		// container, so that is not a problem.

		// For the position, we have to set the Canvas.left and Canvas.top custom properties
		// for this child view.

		double uiScaleFactor = UiProvider::get().getUiScaleFactor();

        try
        {
		    ::Windows::UI::Xaml::Controls::Canvas::SetLeft( _pFrameworkElement, bounds.x / uiScaleFactor );
		    ::Windows::UI::Xaml::Controls::Canvas::SetTop( _pFrameworkElement, bounds.y / uiScaleFactor );

		    // The size is set by manipulating the Width and Height property.
		
		    _pFrameworkElement->Width = intToUwpDimension( bounds.width, uiScaleFactor );
		    _pFrameworkElement->Height = intToUwpDimension( bounds.height, uiScaleFactor );
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // view was already destroyed. Ignore this.
        }

        BDN_WINUWP_TO_STDEXC_END;
	}


	int uiLengthToPixels(const UiLength& uiLength) const override
	{
		return UiProvider::get().uiLengthToPixels(uiLength);
	}

	Margin uiMarginToPixelMargin(const UiMargin& margin) const override
	{
		return UiProvider::get().uiMarginToPixelMargin(margin);
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


	
	Size calcPreferredSize() const
	{
		return _calcPreferredSize( std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() );
	}

	int calcPreferredHeightForWidth(int width) const
	{
		return _calcPreferredSize(
			intToUwpDimension(width, UiProvider::get().getUiScaleFactor() ),
			std::numeric_limits<float>::infinity() )
			.height;
	}

	int calcPreferredWidthForHeight(int height) const
	{
		return _calcPreferredSize(
			std::numeric_limits<float>::infinity(),
			intToUwpDimension(height, UiProvider::get().getUiScaleFactor() ) )
			.width;
	}


	/** Returns the XAML FrameworkElement object for this view.*/
	::Windows::UI::Xaml::FrameworkElement^ getFrameworkElement() override
	{
		return _pFrameworkElement;
	}


	/** Returns a pointer to the outer view object that is associated with this core.
        Can return null if the core has been disposed (i.e. if it is not connected
        to an outer view anymore).*/
	View* getOuterView()
	{
		return _pOuterViewWeak;
	}

protected:
	
	ViewCoreEventForwarder^ getViewCoreEventForwarder()
	{
		return _pEventForwarder;
	}

	virtual void _sizeChanged()
	{
		// do not layout here. We do that in _layoutUpdated.		
	}

	virtual void _layoutUpdated()
	{
		// Xaml has done a layout cycle. At this point all the controls should know their
		// desired sizes. So this is when we schedule our layout updated
        if(_pOuterViewWeak!=nullptr)
		    _pOuterViewWeak->needLayout();
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

	Size _calcPreferredSize(float availableWidth, float availableHeight) const
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		// tell the element that it has a huge available size.
		// The docs say that one can pass Double::PositiveInifinity here as well, but apparently that constant
		// is not available in C++. And std::numeric_limits<float>::infinity() does not seem to work.

        try
        {
		    ::Windows::UI::Xaml::Visibility oldVisibility = _pFrameworkElement->Visibility;
		    if(oldVisibility != ::Windows::UI::Xaml::Visibility::Visible)
		    {
			    // invisible elements all report a zero size. So we must make the element temporarily visible
			    _pFrameworkElement->Visibility = ::Windows::UI::Xaml::Visibility::Visible;			
		    }

		    if(availableWidth<0)
			    availableWidth = 0;
		    if(availableHeight<0)
			    availableHeight = 0;

		    // the Width and Height properties indicate to the layout process how big we want to be.
		    // If they are set then they are incorporated into the DesiredSize measurements.
		    // So we set them to "Auto" now, so that the size is only measured according to the content size.
		    _pFrameworkElement->Width = std::numeric_limits<double>::quiet_NaN();
		    _pFrameworkElement->Height = std::numeric_limits<double>::quiet_NaN();

		    _pFrameworkElement->Measure( ::Windows::Foundation::Size( availableWidth, availableHeight ) );

		    ::Windows::Foundation::Size desiredSize = _pFrameworkElement->DesiredSize;
		
		    double uiScaleFactor = UiProvider::get().getUiScaleFactor();

		    Size size = uwpSizeToSize(desiredSize, uiScaleFactor);

		    if(oldVisibility != ::Windows::UI::Xaml::Visibility::Visible)
			    _pFrameworkElement->Visibility = oldVisibility;

            return size;
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // view was already destroyed. Ignore this and return zero size
            return Size();
        }
        
        BDN_WINUWP_TO_STDEXC_END;
	}


	::Windows::UI::Xaml::FrameworkElement^ _pFrameworkElement;

	View*					_pOuterViewWeak;	// weak by design	

	ViewCoreEventForwarder^ _pEventForwarder;
};


}
}

#endif

