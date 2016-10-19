#ifndef BDN_WINUWP_WindowCore_H_
#define BDN_WINUWP_WindowCore_H_

#include <bdn/Window.h>
#include <bdn/IWindowCore.h>
#include <bdn/log.h>
#include <bdn/NotImplementedError.h>

#include <bdn/winuwp/IViewCoreParent.h>
#include <bdn/winuwp/IFrameworkElementOwner.h>
#include <bdn/winuwp/util.h>
#include <bdn/winuwp/UiProvider.h>

#include <limits>
#include <atomic>

namespace bdn
{
namespace winuwp
{
	
class WindowCore : public Base,
                    BDN_IMPLEMENTS IWindowCore,
                    BDN_IMPLEMENTS IViewCoreParent,
                    BDN_IMPLEMENTS IFrameworkElementOwner
{
public:
	WindowCore(UiProvider* pUiProvider, Window* pOuterWindow)
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

		_pUiProvider = pUiProvider;
		_outerWindowWeak = pOuterWindow;

        // In UWP there is no normal "top level window" in the classical sense.
        // An UWP app has one or more "application views". While these views look
        // like a normal window to the user, they are actually something different.
        // Each application view has its own UI thread and runs independent of other application
        // views. So the problem with multiple UI threads alone means that we cannot simply
        // map application views directly to window objects.
        // Instead it is probably best to consider an application view as a kind of "screen"
        // instead and create our window objects as child panels of the application view.

        // For the time being, we also do not support multiple application views. This concept only
        // exists on Windows and other platforms do not have a similar construct. So it does not
        // make much sense to support it right now.

        // So we use one application view with a child panel for each of our window objects.

		_pAppView = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
        _appViewId = _pAppView->Id;			

        _pXamlWindow = ::Windows::UI::Xaml::Window::Current;			

        // create a sub-panel inside the xaml window. The sub-panel is what we actually
        // consider our "window". 
        
		_pEventForwarder = ref new EventForwarder(this);

		// we want full control over the placement of our "window" panel.
		// Unfortunately Xaml automatically arranges our content view of the top level window.
		// To work around that we add a canvas as the content view and then
		// add the real content view to the canvas.
		_pWindowPanelParent = (Windows::UI::Xaml::Controls::Canvas^)_pXamlWindow->Content;
        if(_pWindowPanelParent==nullptr)
        {
            _pWindowPanelParent = ref new ::Windows::UI::Xaml::Controls::Canvas();		    
		    _pWindowPanelParent->Visibility = ::Windows::UI::Xaml::Visibility::Visible;		
		    _pXamlWindow->Content = _pWindowPanelParent;
        }

        // now add the panel that should represent this "window". Note that we need
        // a separate panel for the window (as opposed to directly adding our content view
        // to the top level container) because we need a way to represent the window's own properties
        // (like visible/hidden) without modifying the content panel. So we add a panel for the window
        // to get this intermediate control.
        _pWindowPanel = ref new ::Windows::UI::Xaml::Controls::Canvas();		    
		_pWindowPanel->Visibility = ::Windows::UI::Xaml::Visibility::Visible;		
        _pWindowPanelParent->Children->Append(_pWindowPanel);

        // make sure that we are initialized with the current size
        _windowPanelParentLayoutUpdated();
        
        _pWindowPanelParent->LayoutUpdated += ref new Windows::Foundation::EventHandler<Platform::Object^>
				    ( _pEventForwarder, &EventForwarder::windowPanelParentLayoutUpdated );

        // update the position and size property of the outer window to reflect the current bounds
		_scheduleUpdateOuterPositionAndSizeProperty();				

        setVisible( pOuterWindow->visible() );

        BDN_WINUWP_TO_STDEXC_END;
	}

	~WindowCore()
	{
        BDN_DESTRUCT_BEGIN;

        BDN_WINUWP_TO_STDEXC_BEGIN;

        if(_pEventForwarder!=nullptr)
            _pEventForwarder->dispose();		

        if(_pWindowPanel!=nullptr)
        {
            // remove our window panel from the parent window
            if(_pWindowPanelParent!=nullptr)
            {
                try
                {
                    unsigned index=0;
                    if(_pWindowPanelParent->Children->IndexOf(_pWindowPanel, &index))
                        _pWindowPanelParent->Children->RemoveAt(index);
                }
                catch(::Platform::DisconnectedException^ e)
                {
                    // window is already destroyed. Ignore this.
                }
            }
            _pWindowPanel = nullptr;
        }

        _pWindowPanelParent = nullptr;

        BDN_WINUWP_TO_STDEXC_END;

        BDN_DESTRUCT_END(bdn::winuwp::WindowCore);

    }


    /** Returns the Xaml UI element that represents the window.
        Note that this is NOT a Windows.UI.Xaml.Window object. Instead it is a child
        container panel inside the actual Windows.UI.Xaml.Window.
    */
	::Windows::UI::Xaml::FrameworkElement^ getFrameworkElement() override
	{
		return _pWindowPanel;
	}
	
	void setTitle(const String& title) override
	{
		// apparently it is not possible to set the window title programmatically.
		// Windows seems to take this from the manifest.
		// So, just ignore this.
	}


	/** Returns the area where the Window's content window
		should be.*/
	Rect getContentArea() override
	{
		Size contentSize = _getContentSize();
		return Rect(0, 0, contentSize.width, contentSize.height );
	}


	Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
	{
		return contentSize + _getNonContentSize();
	}


	Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
	{
		return windowSize - _getNonContentSize();
	}


	Size getMinimumSize() const override
	{
		return _getNonContentSize();
	}


	Rect getScreenWorkArea() const override
	{
		return _pUiProvider->getScreenWorkArea();
	}


	void	setVisible(const bool& visible) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        _pWindowPanel->Visibility = visible ? ::Windows::UI::Xaml::Visibility::Visible : ::Windows::UI::Xaml::Visibility::Collapsed;
        
		if(visible)
			_pXamlWindow->Activate();            

        BDN_WINUWP_TO_STDEXC_END;

	}
	
	
	void setPadding(const Nullable<UiMargin>& padding) override
	{
		// we don't care. The outer Window object will handle the layout.
	}

	
	Rect adjustAndSetBounds(const Rect& bounds) override
	{
		// we cannot influence our bounds. The OS / the user has the only control over it.

        // So we ignore the call and return the current bounds as the "adjusted" value.
        return _getBounds();
	}

    Rect adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const
    {
        // we cannot influence our bounds. The OS / the user has the only control over it.

        // So we return the current bounds as the "adjusted" value.
        return _getBounds();
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
			throw InvalidArgumentError("Invalid UiLength unit passed to WindowCore::uiLengthToDips: "+std::to_string((int)uiLength.unit) );
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
		// the implementation for this must be provided by the outer Window object.
		throw NotImplementedError("WindowCore::calcPreferredSize");	
	}

	
	

	bool tryChangeParentView(View* pNewParent) override
	{
		// we don't have a parent. Report that we cannot do this.
		return false;
	}


	void addChildUiElement( ::Windows::UI::Xaml::UIElement^ pUiElement ) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        // we have only one child (our own content view).
        _pWindowPanel->Children->Clear();

		_pWindowPanel->Children->Append(pUiElement);

        BDN_WINUWP_TO_STDEXC_END;
	}
	

private:
    
	Size _getContentSize() const
	{
		// The Bounds rect actually does NOT include the title bar etc.
		// So the Bounds rect is actually the content rect.
		Rect bounds = _getBounds();

		return Size(bounds.width, bounds.height);
	}

	Rect _getBounds() const
	{
        BDN_WINUWP_TO_STDEXC_BEGIN

        Rect bounds;

        try
        {
            bounds = uwpRectToRect( _pXamlWindow->Bounds );
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // this means that the window is already destroyed.
            // Use an empty rect in that case.
            bounds = Rect();
        }

		if(bounds.width == std::numeric_limits<double>::max())
			bounds.width = 0;
		if(bounds.height == std::numeric_limits<double>::max())
			bounds.height = 0;

        // there is no "moved" event for Xaml windows. As such, we cannot find out when
        // the window position changes. So we cannot update the outer window's bounds property
        // with the position.
        // As a result it is better to always assume zero position. Otherwise we can get inconsistencies
        // with different bounds being reported at different times, even though there is no change event
        // in between
        bounds.x = 0;
        bounds.y = 0;

		return bounds;

        BDN_WINUWP_TO_STDEXC_END
	}

	Size _getNonContentSize() const
	{
		// Windows hides the sizes of the non-content area from us. So they are
		// 0 as far as we need to be concerned.
		return Size(0, 0);
	}

    double getEmSizeDips() const
    {
        if(_emSizeDipsIfInitialized==-1)
        {
            // the font size of a Window cannot be changed within the UWP system.
            // Only Controls have a font size attached to them.
            // The default font size for controls is documented as being 11 DIPs, so we use that
            // here as the em size.
            _emSizeDipsIfInitialized = 11;
        }

        return _emSizeDipsIfInitialized;
    }

    double getSemSizeDips() const
    {
        if(_semSizeDipsIfInitialized==-1)
            _semSizeDipsIfInitialized = UiProvider::get().getSemSizeDips();

        return _semSizeDipsIfInitialized;
    }


	void _scheduleUpdateOuterPositionAndSizeProperty()
	{
        if(_outerPositionAndSizeUpdateScheduled)
            return;

        _outerPositionAndSizeUpdateScheduled = true;

        // we want to update the View's position and size properties.
        // we do this asynchronously to ensure that there can be no strange
		// interactions with in-progress operations

		// keep ourselves alive during this
		P<WindowCore> pThis = this;

		asyncCallFromMainThread(
			[pThis]()			
            {
                pThis->_outerPositionAndSizeUpdateScheduled = false;

                P<View> pView = pThis->_outerWindowWeak.toStrong();
				if(pView!=nullptr)
                {
                    // call View::adjustAndSetBounds. That will automatically call
                    // OUR adjustAndSetBounds and then update the view properties
                    // according to the returned bounds rect.

                    pView->adjustAndSetBounds( pThis->_getBounds() );
                }
			});
	}

	ref class EventForwarder : public Platform::Object
	{
	internal:
		EventForwarder(WindowCore* pParent)
		{
			_pParentWeak = pParent;
		}

	public:
		void dispose()
		{
			_pParentWeak = nullptr;
		}
        
		void windowPanelParentLayoutUpdated( Platform::Object^ pSender, Platform::Object^ pArgs )
		{
            BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			if(_pParentWeak!=nullptr)
				_pParentWeak->_windowPanelParentLayoutUpdated();

            BDN_WINUWP_TO_PLATFORMEXC_END
		}

	private:
		WindowCore* _pParentWeak;
	};

    
	void _windowPanelParentLayoutUpdated()
	{
		// Xaml has done a layout cycle on the window panel parent.

        BDN_WINUWP_TO_STDEXC_BEGIN;

        try
        {        
            P<View> pOuterView = _outerWindowWeak.toStrong();
            if(pOuterView!=nullptr)
            {
                ::Windows::Foundation::Rect bounds = _pXamlWindow->Bounds;

                double width = bounds.Width;
                double height = bounds.Height;
                
                // Update our window panel to the same size as the outer window.
                if(_pWindowPanel->Width != width
                    || _pWindowPanel->Height != height)
                {                    
                    _pWindowPanel->Width = width;
                    _pWindowPanel->Height = height;

                    // we need to update the outer view's size property.
                    // We do that by calling View::adjustAndSetBounds. That will
                    // call our adjustAndSetBounds, which will do nothing and
                    // only return the current bounnds. And then the view will store
                    // that in its properties, which is what we want.

                    pOuterView->adjustAndSetBounds( _getBounds() );
                }
            }
        }
        catch(::Platform::DisconnectedException^ e)
        {
            // window is already destroyed. Ignore this.
        }

        BDN_WINUWP_TO_STDEXC_END;
	}


	P<UiProvider>	_pUiProvider;
	WeakP<Window>   _outerWindowWeak;

	// Windows::ApplicationModel::Core::CoreApplicationView^ _pCoreAppView;

	::Windows::UI::ViewManagement::ApplicationView^	_pAppView;
	int												_appViewId;

	::Windows::UI::Xaml::Window^			_pXamlWindow;
	::Windows::UI::Xaml::Controls::Canvas^	_pWindowPanelParent;

    ::Windows::UI::Xaml::Controls::Canvas^	_pWindowPanel;
        
	EventForwarder^ _pEventForwarder;

    bool _outerPositionAndSizeUpdateScheduled = false;

    mutable double _emSizeDipsIfInitialized = -1;
    mutable double _semSizeDipsIfInitialized = -1;
};


}
}

#endif