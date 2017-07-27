#ifndef BDN_WINUWP_WindowCore_H_
#define BDN_WINUWP_WindowCore_H_

#include <bdn/Window.h>
#include <bdn/IWindowCore.h>
#include <bdn/log.h>
#include <bdn/NotImplementedError.h>
#include <bdn/ContainerView.h>
#include <bdn/windowCoreUtil.h>

#include <bdn/winuwp/IViewCoreParent.h>
#include <bdn/winuwp/IUwpViewCore.h>
#include <bdn/winuwp/util.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/IUwpLayoutDelegate.h>
#include <bdn/winuwp/UwpViewWithLayoutDelegate.h>

#include <limits>
#include <atomic>

namespace bdn
{
namespace winuwp
{
	
class WindowCore : public Base,
                    BDN_IMPLEMENTS IWindowCore,
                    BDN_IMPLEMENTS IViewCoreParent,
                    BDN_IMPLEMENTS IUwpViewCore
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
		// Unfortunately Xaml automatically arranges the content view of the top level window.
		// To work around that we add a canvas as the content view and then
		// add the real content view to the canvas.
		_pSharedTopContainer = (UwpViewWithLayoutDelegate<>^)_pXamlWindow->Content;
        if(_pSharedTopContainer==nullptr)
        {
            _pSharedTopContainer = ref new UwpViewWithLayoutDelegate<>();
            
            _pSharedTopContainerLayoutDelegate = newObj<TopContainerLayoutDelegate>();
            _pSharedTopContainer->setLayoutDelegateWeak( _pSharedTopContainerLayoutDelegate );
            
		    _pSharedTopContainer->Visibility = ::Windows::UI::Xaml::Visibility::Visible;		
		    _pXamlWindow->Content = _pSharedTopContainer;
        }
        else
        {
            // we already created the top container. Fetch the existing layout delegate.
            _pSharedTopContainerLayoutDelegate = cast<TopContainerLayoutDelegate>( _pSharedTopContainer->getLayoutDelegate() );
        }

        // add the window to the delegate's list
        _pSharedTopContainerLayoutDelegate->addWindowWeak( pOuterWindow );

        // now add the panel that should represent this "window". Note that we need
        // a separate panel for the window (as opposed to directly adding our content view
        // to the top level container) because we need a way to represent the window's own properties
        // (like visible/hidden) without modifying the content panel. So we add a panel for the window
        // to get this intermediate control. Note that the top container is shared by all Window objects,
        // so we cannot use that to represent the window visibility.
        _pContentContainer = ref new UwpViewWithLayoutDelegate<>();
        _pContentContainerLayoutDelegate = newObj<ContentContainerLayoutDelegate>(pOuterWindow);
        _pContentContainer->setLayoutDelegateWeak( _pContentContainerLayoutDelegate );


		_pContentContainer->Visibility = ::Windows::UI::Xaml::Visibility::Visible;		
        _pSharedTopContainer->Children->Append(_pContentContainer);

        _pXamlWindow->SizeChanged += ref new ::Windows::UI::Xaml::WindowSizeChangedEventHandler
				    ( _pEventForwarder, &EventForwarder::windowSizeChanged );

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

        if(_pContentContainer!=nullptr)
        {
            // remove our content container from the top container
            if(_pSharedTopContainer!=nullptr)
            {
                try
                {
                    unsigned index=0;
                    if(_pSharedTopContainer->Children->IndexOf(_pContentContainer, &index))
                        _pSharedTopContainer->Children->RemoveAt(index);
                }
                catch(::Platform::DisconnectedException^ e)
                {
                    // window is already destroyed. Ignore this.
                }
            }
            _pContentContainer = nullptr;
        }

        _pSharedTopContainer = nullptr;

        BDN_WINUWP_TO_STDEXC_END;

        BDN_DESTRUCT_END(bdn::winuwp::WindowCore);

    }


    /** Returns the Xaml UI element that represents the window.
        Note that this is NOT a Windows.UI.Xaml.Window object. Instead it is a child
        container panel inside the actual Windows.UI.Xaml.Window.
    */
	::Windows::UI::Xaml::FrameworkElement^ getFrameworkElement() override
	{
		return _pContentContainer;
	}
	
	void setTitle(const String& title) override
	{
		// apparently it is not possible to set the window title programmatically.
		// Windows seems to take this from the manifest.
		// So, just ignore this.
	}



	void	setVisible(const bool& visible) override
	{
        BDN_WINUWP_TO_STDEXC_BEGIN;

        _pContentContainer->Visibility = visible ? ::Windows::UI::Xaml::Visibility::Visible : ::Windows::UI::Xaml::Visibility::Collapsed;
        
		if(visible)
			_pXamlWindow->Activate();            

        BDN_WINUWP_TO_STDEXC_END;

	}
	
	
	void setPadding(const Nullable<UiMargin>& padding) override
	{
		// we don't care. The outer Window object will handle the layout.
	}

    
    void setPreferredSizeHint(const Size& hint) override
    {
        // we do not use the hínt
    }

    
    void setHorizontalAlignment(const View::HorizontalAlignment& align) override
    {
        // don't care - it does not affect anything
    }

    void setVerticalAlignment(const View::VerticalAlignment& align) override
    {
        // don't care - it does not affect anything
    }
        
    void setPreferredSizeMinimum(const Size& limit) override
    {
        // our preferred size does not matter for the layout, since we cannot adapt the window size anyway
    }
    
    void setPreferredSizeMaximum(const Size& limit) override
    {
        // our preferred size does not matter for the layout, since we cannot adapt the window size anyway
    }
    
    
    void invalidateSizingInfo( View::InvalidateReason reason ) override
    {
        // See ChildViewCore::needLayout for an explanation on why we ignore standard property changes.
        if(reason!=View::InvalidateReason::standardPropertyChanged && reason!=View::InvalidateReason::standardChildPropertyChanged )
        {
            // XXX
            OutputDebugString( (String(typeid(*this).name())+".needSizingInfoUpdate()\n" ).asWidePtr() );

            // we leave the layout coordination up to windows. See doc_input/winuwp_layout.md for more information on why
            // this is.
            BDN_WINUWP_TO_STDEXC_BEGIN;

            try
            {
		        _pContentContainer->InvalidateMeasure();
            }
            catch(::Platform::DisconnectedException^ e)
            {
                // view was already destroyed. Ignore this.
            }

            BDN_WINUWP_TO_STDEXC_END;
        }
    }


    void needLayout( View::InvalidateReason reason ) override
    {
        // See ChildViewCore::needLayout for an explanation on why we ignore standard property changes.
        if(reason!=View::InvalidateReason::standardPropertyChanged && reason!=View::InvalidateReason::standardChildPropertyChanged )
        {
            // XXX
            OutputDebugString( (String(typeid(*this).name())+".needLayout()\n" ).asWidePtr() );

            // we leave the layout coordination up to windows. See doc_input/winuwp_layout.md for more information on why
            // this is.
            BDN_WINUWP_TO_STDEXC_BEGIN;

            try
            {
		        _pContentContainer->InvalidateArrange();
            }
            catch(::Platform::DisconnectedException^ e)
            {
                // view was already destroyed. Ignore this.
            }

            BDN_WINUWP_TO_STDEXC_END;
        }
    }


    void childSizingInfoInvalidated(View* pChild) override
    {
        // While windows takes care of propagating the "invalidateMeasure" request to parents automatically,
        // we still HAVE to propagate this event upwards to our parent, since the outer view objects cache sizing info
        // and these caches must be invalidated for all parents.
        P<View> pOuterView = _outerWindowWeak.toStrong();
        if(pOuterView!=nullptr)
        {
            pOuterView->invalidateSizingInfo( View::InvalidateReason::childSizingInfoInvalidated );
            pOuterView->needLayout( View::InvalidateReason::childSizingInfoInvalidated );
        }
    }


    void requestAutoSize() override
    {
        // it is not possible to control the size of a UWP window from inside the app. So we ignore this request.
    }

	void requestCenter() override
    {
        // it is not possible to control the position of a UWP window from inside the app. So we ignore this request.
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
    


	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override
	{
        P<Window> pWindow = _outerWindowWeak.toStrong();
        if(pWindow!=nullptr)
            return defaultWindowCalcPreferredSizeImpl( pWindow, availableSpace, Margin(), Size() );
        else
            return Size(0,0);
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
        _pContentContainer->Children->Clear();

		_pContentContainer->Children->Append(pUiElement);

        BDN_WINUWP_TO_STDEXC_END;
	}



    void layout()
    {
        // this is called from uwpMeasureFinalize. Just call the default layout for the window.
        
        OutputDebugString( (String(typeid(*this).name())+".layout()\n").asWidePtr() );
        
        P<Window> pOuter = _outerWindowWeak.toStrong();
        if(pOuter!=nullptr)
        {
            Size contentSize = _getContentSize();
            Rect contentArea(0, 0, contentSize.width, contentSize.height );

            defaultWindowLayoutImpl(pOuter, contentArea);
        }

        // XXX
        OutputDebugString( ("/"+String(typeid(*this).name())+".layout()\n").asWidePtr() );
    }
    
	

private:
        

	Rect _getScreenWorkArea() const
	{
		return _pUiProvider->getScreenWorkArea();
	}
    
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
        
		void windowSizeChanged( Platform::Object^ pSender, ::Windows::UI::Core::WindowSizeChangedEventArgs^ pArgs )
		{
            BDN_WINUWP_TO_PLATFORMEXC_BEGIN

			if(_pParentWeak!=nullptr)
				_pParentWeak->_windowSizeChanged();

            BDN_WINUWP_TO_PLATFORMEXC_END
		}


	private:
		WindowCore* _pParentWeak;
	};




    

    /** Layout delegate for the top container. Simply sizes the child to fill the entire space.*/
    class TopContainerLayoutDelegate : public Base, BDN_IMPLEMENTS IUwpLayoutDelegate
    {
    public:
        TopContainerLayoutDelegate()
        {
        }

        void addWindowWeak(Window* pWindow)
        {
            _windowWeakList.push_back(pWindow);
        }

        class AutoCleaningWindowIterator
        {
        public:
            AutoCleaningWindowIterator( std::list< WeakP<Window> >& weakList )
                : _weakList(weakList)
            {
                _nextElementIt = _weakList.begin();
            }

            bool next( P<Window>& pWindow )
            {
                while(_nextElementIt != _weakList.end() )
                {
                    pWindow = _nextElementIt->toStrong();
                    if(pWindow!=nullptr)
                    {
                        ++_nextElementIt;
                        return true;
                    }
                     
                    // window has been deleted. Remove it from the list
                    _nextElementIt = _weakList.erase( _nextElementIt );
                }

                return false;
            }

        protected:
            std::list< WeakP<Window> >&             _weakList;
            std::list< WeakP<Window> >::iterator    _nextElementIt;
        };

        Size uwpMeasureOverride(const Size& availableSpace ) override
        {
            // XXX
            OutputDebugString( String( "Window.TopContainerLayoutDelegate.measureOverride("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+")\n" ).asWidePtr() );

            AutoCleaningWindowIterator it( _windowWeakList );
            P<Window> pWindow;
            while( it.next(pWindow) )
            {
                P<WindowCore> pCore = tryCast<WindowCore>( pWindow->getViewCore() );
                if(pCore!=nullptr)
                {
                    if(pCore->_pContentContainer!=nullptr)
                        pCore->_pContentContainer->Measure( sizeToUwpSize(availableSpace) );                
                }
            }

            OutputDebugString( String( "/Window.TopContainerLayoutDelegate.measureOverride\n" ).asWidePtr() );

			// IMPORTANT:
			// In contrast to the documentation, UI elements cannot be made smaller than their DesiredSize
            // (see doc_input/winuwp_layout.md for more information). Their Arrange method will always
			// enlarge the size we pass to it and make the panel at least as big as the desired size.
			// Since we absolutely do not want that we always return a desired size of 0.
			// That does not interfere with our own layout, since the Window will always make this panel
            // the same size as itself.
            return Size(0,0);
        }

        
        void finalizeUwpMeasure(const Size& lastMeasureAvailableSpace) override
        {
            OutputDebugString( String( "Window.ContentContainerLayoutDelegate.finalizeUwpMeasure("+std::to_string(lastMeasureAvailableSpace.width)+", "+std::to_string(lastMeasureAvailableSpace.height)+")\n" ).asWidePtr() );

            AutoCleaningWindowIterator it( _windowWeakList );
            P<Window> pWindow;
            while( it.next(pWindow) )
                defaultFinalizeUwpMeasure(pWindow, lastMeasureAvailableSpace);                           
        }
        
        Size uwpArrangeOverride(const Size& finalSize ) override
        {
            // XXX
            OutputDebugString( String( "Window.TopContainerLayoutDelegate.arrangeOverride("+std::to_string(finalSize.width)+", "+std::to_string(finalSize.height)+"\n" ).asWidePtr() );

            Rect contentContainerRect( Point(0,0), finalSize);

            AutoCleaningWindowIterator it( _windowWeakList );
            P<Window> pWindow;
            while( it.next(pWindow) )
            {
                P<WindowCore> pCore = tryCast<WindowCore>( pWindow->getViewCore() );
                if(pCore!=nullptr)
                {
                    // arrange the content container
                    pCore->_pContentContainer->Arrange( rectToUwpRect(contentContainerRect) );
                }
            }

            // XXX
            OutputDebugString( String( "/Window.TopContainerLayoutDelegate.arrangeOverride()\n" ).asWidePtr() );

            return finalSize;
        }


    protected:
        std::list< WeakP<Window> > _windowWeakList;
    };
    friend class TopContainerLayoutDelegate;



    /** Layout delegate for the content container.*/
    class ContentContainerLayoutDelegate : public Base, BDN_IMPLEMENTS IUwpLayoutDelegate
    {
    public:
        ContentContainerLayoutDelegate(Window* pWindow)
            : _windowWeak(pWindow)
        {
        }

        Size uwpMeasureOverride(const Size& availableSpace ) override
        {
             // XXX
            OutputDebugString( String( "Window.ContentContainerLayoutDelegate.measureOverride("+std::to_string(availableSpace.width)+", "+std::to_string(availableSpace.height)+")\n" ).asWidePtr() );
            
            OutputDebugString( String( "/Window.ContentContainerLayoutDelegate.measureOverride\n" ).asWidePtr() );            

            return Size(0,0);
        }

        
	    void finalizeUwpMeasure(const Size& lastMeasureAvailableSpace) override
        {            
            OutputDebugString( String( "Window.ContentContainerLayoutDelegate.finalizeUwpMeasure("+std::to_string(lastMeasureAvailableSpace.width)+", "+std::to_string(lastMeasureAvailableSpace.height)+")\n" ).asWidePtr() );
            
            P<Window> pWindow = _windowWeak.toStrong();
            if(pWindow!=nullptr)
                defaultFinalizeUwpMeasure(pWindow, lastMeasureAvailableSpace);
                        
            OutputDebugString( String( "/Window.ContentContainerLayoutDelegate.finalizeUwpMeasure\n" ).asWidePtr() );            
        }
        
        Size uwpArrangeOverride(const Size& finalSize ) override
        {
            // XXX
            OutputDebugString( String( "Window.ContentContainerLayoutDelegate.arrangeOverride("+std::to_string(finalSize.width)+", "+std::to_string(finalSize.height)+"\n" ).asWidePtr() );

            P<Window> pWindow = _windowWeak.toStrong();

            if(pWindow!=nullptr)
            {
                // arrange the content view. The default implementation for uwpArrangeOverride does that.
                defaultArrangeOverride( pWindow, finalSize);
            }

            // XXX
            OutputDebugString( String( "/Window.ContentContainerLayoutDelegate()\n" ).asWidePtr() );

            return finalSize;
        }

                
        	

    protected:
        WeakP<Window> _windowWeak;
    };



    void _windowSizeChanged()
	{
        // the size of the xaml window has changed

        BDN_WINUWP_TO_STDEXC_BEGIN;
        
        try
        {        
            P<View> pOuterView = _outerWindowWeak.toStrong();
            if(pOuterView!=nullptr)
            {
                /*
                double width = _pXamlWindow->Bounds.Width;
                double height = _pXamlWindow->Bounds.Height;
                
                // Update our window panel to the same size as the outer window.
                if(_pSharedTopContainer->Width != width
                    || _pSharedTopContainer->Height != height)
                {                    
                    _pSharedTopContainer->Width = width;
                    _pSharedTopContainer->Height = height;
                }
                
                // Update our window panel to the same size as the outer window.
                if(_pContentContainer->Width != width
                    || _pContentContainer->Height != height)
                {                    
                    _pContentContainer->Width = width;
                    _pContentContainer->Height = height;
                }*/


                // we need to update the outer view's size property.
                // We do that by calling View::adjustAndSetBounds. That will
                // call our adjustAndSetBounds, which will do nothing and
                // only return the current bounnds. And then the view will store
                // that in its properties, which is what we want.

                pOuterView->adjustAndSetBounds( _getBounds() );
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
	UwpViewWithLayoutDelegate<>^	        _pSharedTopContainer;
    P<TopContainerLayoutDelegate>           _pSharedTopContainerLayoutDelegate;
    UwpViewWithLayoutDelegate<>^	        _pContentContainer;
    P<ContentContainerLayoutDelegate>       _pContentContainerLayoutDelegate;
        
	EventForwarder^ _pEventForwarder;

    bool _outerPositionAndSizeUpdateScheduled = false;

    mutable double _emSizeDipsIfInitialized = -1;
    mutable double _semSizeDipsIfInitialized = -1;
};


}
}

#endif