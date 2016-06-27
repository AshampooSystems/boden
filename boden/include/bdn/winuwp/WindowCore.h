#ifndef BDN_WINUWP_WindowCore_H_
#define BDN_WINUWP_WindowCore_H_

#include <bdn/IWindowCore.h>

namespace bdn
{
namespace winuwp
{
	
class WindowCore : public Base, BDN_IMPLEMENTS IWindowCore
{
public:
	WindowCore(UiProvider* pUiProvider, bool mainView)
	{
		_pUiProvider = pUiProvider;

		// the concept of a top level window in a UWP app is an "application view".
		// Application views are independent of one another.
		// IMPORTANT: each application view has its own UI thread!

		if(mainView)
		{
			// wrap the existing main view.
			_mainView = true;

			_pAppView = Window::ApplicationModel::Core::CoreApplication::MainView;
			_appViewId = _pAppView->Id;			
		}
		else
		{
			_mainView = false; 

			// create a new secondary application view

			CoreApplicationView^ pNewView = Window::ApplicationModel::Core::CoreApplication::CreateNewView();
		
			// initialize the new view. We do that in the new view's new UI thread.
			auto pInitOp = pNewView->Dispatcher->RunAsync(
				CoreDispatcherPriority.Normal,
				[this]()
				{
					_pWindow = Windows::UI::Xaml::Window::Current;

					Frame frame = new Frame();
					frame.Navigate(typeof(SecondaryPage), null);   
					Window.Current.Content = frame;
					// You have to activate the window in order to show it later.
					Window.Current.Activate();

					this->_pAppView = ApplicationView.GetForCurrentView();
					this->_appViewId = _pAppView->Id;
				});


			auto initTask = concurrency::create_task(pInitOp);

			// wait for the initialization to finish.
			// If there is an error then we will get an exception here.
			initTask.get();

			// now the _pAppView and _pAppViewId members are initialized.
		}
	}

	~WindowCore()
	{
		try
		{
			if(_mainView)
			{
				// do not close the view. The main view must remain.
			}
			else
			{
				auto pCloseOp = _pAppView->Dispatcher->RunAsync(
					CoreDispatcherPriority.Normal,
					[this]()
					{
						// close the app view and end its thread.
						_pAppView->Close();
					});


				auto closeTask = concurrency::create_task(pCloseOp);

				// wait for the initialization to finish.
				// If there is an error then we will get an exception here.
				closeTask.get();
			}			
		}
		catch(std::exception& e)
		{
			// log and ignore.
			logError(e, "Exception in WindowCore destructor. Ignoring.")
		}
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
		Windows::Foundation::Rect bounds = _pWindow->bounds;
		
		return Rect(bounds.x, bounds.y, bounds.with, bounds.height );
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
			auto pShowOp = Window::UI::ViewManagement::ApplicationViewSwitcher.TryShowAsStandaloneAsync(_appViewId);
			auto showTask = concurrency::create_task(pShowOp);

			// wait for the operation to finish.
			bool result = showTask.get();

			// it may well be that it is not possible to show
			// another application window. So result may be false.
			if(!result)
			{
				// Todo: we need to throw an exception here that causes the visible
				// property of the outer window to be set to false again.
			}
		}
		else
		{
			// Apparently there is no real way to hide a window (not even a secondary one).
			// If another window is shown by "switching" then the previous one is automatically hidden.
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
		// we cannot control our rect. Windows has control over it.
		throw RuntimeError();
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
	bool _mainView;

	Window::ApplicationModel::Core::CoreApplicationView^	_pAppView;
	int														_appViewId;

	Windows::UI::Xaml::Window^	pWindow;

	P<UiProvider>				_pUiProvider;
};


}
}

#endif