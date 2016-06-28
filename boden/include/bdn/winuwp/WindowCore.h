#ifndef BDN_WINUWP_WindowCore_H_
#define BDN_WINUWP_WindowCore_H_

#include <bdn/Window.h>
#include <bdn/IWindowCore.h>
#include <bdn/log.h>
#include <bdn/NotImplementedError.h>

namespace bdn
{
namespace winuwp
{
	
class WindowCore : public Base, BDN_IMPLEMENTS IWindowCore
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
	}

	~WindowCore()
	{
		
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
		Windows::Foundation::Rect bounds = _pWindow->Bounds;
		
		return Rect(
			std::lround(bounds.X),
			std::lround(bounds.Y),
			std::lround(bounds.Width),
			std::lround(bounds.Height) );
	}


	Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override
	{
		return contentSize;
	}


	Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override
	{
		return windowSize;
	}


	Size calcMinimumSize() const override
	{
		return Size(0, 0);
	}


	Rect getScreenWorkArea() const override
	{
		return _pUiProvider->getScreenWorkArea();
	}


	void	setVisible(const bool& visible) override
	{
		if(visible)
		{		 
			auto pShowOp = Windows::UI::ViewManagement::ApplicationViewSwitcher::TryShowAsStandaloneAsync(_appViewId);
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
				});		
		}
		else
		{
			// Apparently there is no real way to hide a window (not even a secondary one).
			// If another window is shown by "switching" then the previous one is automatically hidden.
			// So we do not do anything here.
		}
	}
	

	void setMargin(const UiMargin& margin) override
	{
		// we don't care. The outer Window object will handle the layout.
	}

	
	void setPadding(const UiMargin& padding) override
	{
		// we don't care. The outer Window object will handle the layout.
	}

	
	void setBounds(const Rect& bounds) override
	{
		// we cannot control our rect. The OS has the only control over it.
		throw std::runtime_error("Window bounds cannot be modified by the application.");
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

	

protected:
	P<UiProvider>	_pUiProvider;
	bool			_mainView;
	Window*			_pOuterWindowWeak;

	// Windows::ApplicationModel::Core::CoreApplicationView^ _pCoreAppView;

	Windows::UI::ViewManagement::ApplicationView^	_pAppView;
	int												_appViewId;

	Windows::UI::Xaml::Window^	_pWindow;
};


}
}

#endif