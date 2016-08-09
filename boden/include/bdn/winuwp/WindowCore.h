#ifndef BDN_WINUWP_WindowCore_H_
#define BDN_WINUWP_WindowCore_H_

#include <bdn/Window.h>
#include <bdn/IWindowCore.h>
#include <bdn/log.h>
#include <bdn/NotImplementedError.h>

#include <bdn/winuwp/IParentViewCore.h>
#include <bdn/winuwp/util.h>

#include <limits>

namespace bdn
{
namespace winuwp
{
	
class WindowCore : public Base, BDN_IMPLEMENTS IWindowCore, BDN_IMPLEMENTS IParentViewCore
{
public:
	WindowCore(UiProvider* pUiProvider, Window* pOuterWindow)
	{
		_pUiProvider = pUiProvider;
		_pOuterWindowWeak = pOuterWindow;

		// the concept of a top level window in a UWP app is an "application view".
		// Application views are independent of one another.
		// IMPORTANT: each application view has its own UI thread!

		static bool firstWindowCore = true;

		if(firstWindowCore)
		{
			// wrap the existing main view.

			firstWindowCore = false;

			_mainView = true;

			_pAppView = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
			_appViewId = _pAppView->Id;			

			_pWindow = Windows::UI::Xaml::Window::Current;			
		}
		else
		{
			_mainView = false; 

			// create a new secondary application view

			// This is currently untested. Throw an exception for the time being
			throw NotImplementedError("winuwp::WindowCore not implemented for secondary Window objects.");

			/*_pCoreAppView = Windows::ApplicationModel::Core::CoreApplication::CreateNewView();
		
			// initialize the new view. We do that in the new view's new UI thread.
			auto pInitOp = _pCoreAppView->Dispatcher->RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				ref new Windows::UI::Core::DispatchedHandler( [this]()
				{
					_pWindow = Windows::UI::Xaml::Window::Current;

					// Frame frame = new Frame();
					// frame.Navigate(typeof(SecondaryPage), null);   
					// Window.Current.Content = frame;
					// You have to activate the window in order to show it later.
					Windows::UI::Xaml::Window::Current->Activate();

					this->_pAppView =  Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
					this->_appViewId = _pAppView->Id;
				}) );

			auto initTask = concurrency::create_task(pInitOp);


			// wait for the initialization to finish.
			// If there is an error then we will get an exception here.
			initTask.get();

			// now the _pAppView and _pAppViewId members are initialized.
			*/
		}


		_pEventForwarder = ref new EventForwarder(this);
		_pWindow->SizeChanged += ref new ::Windows::UI::Xaml::WindowSizeChangedEventHandler( _pEventForwarder, &EventForwarder::sizeChanged );

		// we want full control over the placement of our content window.
		// Unfortunately Xaml automatically arranges our content view.
		// To work around that we add a canvas as the content view and then
		// add the real content view to the canvas.

		_pContentCanvas = ref new ::Windows::UI::Xaml::Controls::Canvas();


		// when windows updates the size of the content canvas then that
		// means that we have to update our layout.
		_pContentCanvas->SizeChanged += ref new ::Windows::UI::Xaml::SizeChangedEventHandler( _pEventForwarder, &EventForwarder::contentSizeChanged );

		_pContentCanvas->LayoutUpdated += ref new Windows::Foundation::EventHandler<Platform::Object^>
				( _pEventForwarder, &EventForwarder::layoutUpdated );

		_pContentCanvas->Visibility = ::Windows::UI::Xaml::Visibility::Visible;

		
		_pWindow->Content = _pContentCanvas;

		// update the bounds property of the outer window to reflect the current bounds
		_scheduleUpdateOuterBoundsProperty();				
	}

	~WindowCore()
	{
		_pEventForwarder->dispose();		
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


	Size calcMinimumSize() const override
	{
		return _getNonContentSize();
	}


	Rect getScreenWorkArea() const override
	{
		return _pUiProvider->getScreenWorkArea();
	}


	void	setVisible(const bool& visible) override
	{
		if(visible)
		{		 
			_pWindow->Activate();

			/*auto pShowOp = Windows::UI::ViewManagement::ApplicationViewSwitcher::TryShowAsStandaloneAsync(_appViewId);
			auto showTask = concurrency::create_task(pShowOp);

			// keep ourselves alive while we do this.
			P<WindowCore> pThis = this;

			showTask.then(
				[pThis](concurrency::task<bool> previousTask)
				{
					try
					{
						// it may well be that it is not possible to show
						// another application window. So result may be false.
						if(!previousTask.get())
						{
							// if we cannot show this as a standalone window then we switch to it.
							auto pSwitchOp = Windows::UI::ViewManagement::ApplicationViewSwitcher::SwitchAsync( pThis->_appViewId );
							auto switchTask = concurrency::create_task(pSwitchOp);

							switchTask.then(
								[pThis](concurrency::task<void> previousTask)
								{
									try
									{
										previousTask.get();
									}
									catch(std::exception& e)
									{
										logError(e, "Error showing WindowCore (ApplicationViewSwitcher::SwitchAsync resulted in exception). Ignoring.");

										// since we were unable to show the window we have to correct the value of the visible property.
										pThis->_pOuterWindowWeak->visible() = false;
									}
								} );
						}
					}
					catch(std::exception& e)
					{
						logError(e, "Error showing WindowCore (ApplicationViewSwitcher.TryShowAsStandaloneAsync resulted in exception). Ignoring.");

						// since we were unable to show the window we have to correct the value of the visible property.
						pThis->_pOuterWindowWeak->visible() = false;
					}				
				});		*/
		}
		else
		{
			// Apparently there is no real way to hide a window (not even a secondary one).
			// If another window is shown by "switching" then the previous one is automatically hidden.
			// So we do not do anything here.
		}
	}
	
	
	void setPadding(const UiMargin& padding) override
	{
		// we don't care. The outer Window object will handle the layout.
	}

	
	void setBounds(const Rect& bounds) override
	{
		// we cannot control our rect. The OS has the only control over it.
		// So, just reset the bounds property back to what it really is.
		_scheduleUpdateOuterBoundsProperty();
	}


	
	int uiLengthToPixels(const UiLength& uiLength) const override
	{
		return _pUiProvider->uiLengthToPixels(uiLength);
	}

	Margin uiMarginToPixelMargin(const UiMargin& margin) const override
	{
		return _pUiProvider->uiMarginToPixelMargin(margin);
	}


	Size calcPreferredSize() const override
	{
		// the implementation for this must be provided by the outer Window object.
		throw NotImplementedError("WindowCore::calcPreferredWidthForHeight");	
	}

	
	int calcPreferredHeightForWidth(int width) const override
	{
		// the implementation for this must be provided by the outer Window object.
		throw NotImplementedError("WindowCore::calcPreferredWidthForHeight");	
	}


	int calcPreferredWidthForHeight(int height) const override
	{
		// the implementation for this must be provided by the outer Window object.
		throw NotImplementedError("WindowCore::calcPreferredWidthForHeight");
	}


	

	bool tryChangeParentView(View* pNewParent) override
	{
		// we don't have a parent. Report that we cannot do this.
		return false;
	}


	void addChildUiElement( ::Windows::UI::Xaml::UIElement^ pUiElement ) override
	{
		_pContentCanvas->Children->Clear();
		_pContentCanvas->Children->Append(pUiElement);
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
		Rect bounds = uwpRectToRect( _pWindow->Bounds, UiProvider::get().getUiScaleFactor() );
		if(bounds.width == std::numeric_limits<int>::max())
			bounds.width = 0;
		if(bounds.height == std::numeric_limits<int>::max())
			bounds.height = 0;

		return bounds;
	}

	Size _getNonContentSize() const
	{
		// Windows hides the sizes of the non-content area from us. So they are
		// 0 as far as we need to be concerned.
		return Size(0, 0);
	}

	void _scheduleUpdateOuterBoundsProperty()
	{
		// keep ourselves alive during this
		P<WindowCore> pThis = this;

		// we do this asynchronously to ensure that there can be no strange
		// interactions with in-progress operations
		asyncCallFromMainThread(
			[pThis]()
			{
				if(pThis->_pOuterWindowWeak!=nullptr)
				{
					pThis->_pOuterWindowWeak->bounds() = pThis->_getBounds();
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

		void sizeChanged( Platform::Object^ pSender,  ::Windows::UI::Core::WindowSizeChangedEventArgs^ pArgs)
		{
			if(_pParentWeak!=nullptr)
				_pParentWeak->_sizeChanged();
		}

		void contentSizeChanged( Platform::Object^ pSender,  ::Windows::UI::Xaml::SizeChangedEventArgs^ pArgs)
		{
			if(_pParentWeak!=nullptr)
				_pParentWeak->_contentSizeChanged();
		}

		void layoutUpdated( Platform::Object^ pSender, Platform::Object^ pArgs )
		{
			if(_pParentWeak!=nullptr)
				_pParentWeak->_layoutUpdated();
		}

	private:
		WindowCore* _pParentWeak;
	};


	void _sizeChanged()
	{
		// do nothing
	}

	void _layoutUpdated()
	{
		// Xaml has done a layout cycle. At this point all the controls should know their
		// desired sizes. So this is when we schedule our layout updated
		_pOuterWindowWeak->needLayout();
	}

	void _contentSizeChanged()
	{
		// do nothing
	}

	P<UiProvider>	_pUiProvider;
	bool			_mainView;
	Window*			_pOuterWindowWeak;

	// Windows::ApplicationModel::Core::CoreApplicationView^ _pCoreAppView;

	::Windows::UI::ViewManagement::ApplicationView^	_pAppView;
	int												_appViewId;

	::Windows::UI::Xaml::Window^			_pWindow;
	::Windows::UI::Xaml::Controls::Canvas^	_pContentCanvas;

	EventForwarder^ _pEventForwarder;
};


}
}

#endif