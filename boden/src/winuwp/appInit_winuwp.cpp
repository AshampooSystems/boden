#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/Thread.h>
#include <bdn/log.h>

#include <bdn/win32/ThreadLocalStorageManager.h>
#include <bdn/RequireNewAlloc.h>

namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( ThreadLocalStorageManager, ThreadLocalStorageManager::get );

}
}



namespace bdn
{
	
#if false
ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
internal:
	App(AppControllerBase* pAppController, AppLaunchInfo* pLaunchInfo)
	{
		_pAppController = pAppController;
		_pLaunchInfo = pLaunchInfo;
	}

public:
	virtual ~App()
	{
	}


	// IFrameworkView methods
 
	// Called when the app is launched.
	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView)
	{
		_mainInit();

        AppControllerBase::_set(_pAppController);

		// Register event handlers for app lifecycle. This example includes Activated, so that we
		// can make the CoreWindow active and start rendering on the window.
		applicationView->Activated +=
			ref new Windows::Foundation::TypedEventHandler
				<Windows::ApplicationModel::Core::CoreApplicationView^,
				 Windows::ApplicationModel::Activation::IActivatedEventArgs^>
				(this, &App::activated);
 
		Windows::ApplicationModel::Core::CoreApplication::Suspending +=
			ref new Windows::Foundation::EventHandler< Windows::ApplicationModel::SuspendingEventArgs^ >(this, &App::suspending);
 
		Windows::ApplicationModel::Core::CoreApplication::Resuming +=
			ref new Windows::Foundation::EventHandler< Platform::Object^ >(this, &App::resuming);


		_pAppController->beginLaunch(*_pLaunchInfo);
		_pAppController->finishLaunch(*_pLaunchInfo);
	}

	/** This is called when the app's core window has changed. Initially this gets called before
		Run is called, but it may be called multiple times again later, if the window is recreated.
		
		The "core window" represents the main app window. It is provided by the system.
		*/
	virtual void SetWindow(Windows::UI::Core::CoreWindow^ window)
	{
		int x=0;
	}

	/** Loads or activates any external resources used by the app view before Run is called.*/
	virtual void Load(Platform::String^ entryPoint)
	{
		// do nothing
		int x=0;
	}


	/** Run the app event loop.*/
	virtual void Run()
	{
		// run the event loop.
		while(true)
			Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessOneAndAllPending);
	}


	/** Clean up any external resources created during Load.*/
	virtual void Uninitialize()
	{
	}
 
protected:	
	void activated(	Windows::ApplicationModel::Core::CoreApplicationView^ applicationView,
					Windows::ApplicationModel::Activation::IActivatedEventArgs^ args)
	{
		_pAppController->onActivate();
	}

	void suspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args)
	{
		_pAppController->onSuspend();
	}

	void resuming(Platform::Object^ sender, Platform::Object^ args)
	{
		_pAppController->onResume();
	}
 
	// Window event handlers.
	void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
	void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
	void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);
 
	// DisplayInformation event handlers.
	void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
	void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
	void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
 
private:
	P<AppControllerBase> _pAppController;
	P<AppLaunchInfo>	 _pLaunchInfo;
};


ref class AppFrameworkViewSource : Windows::ApplicationModel::Core::IFrameworkViewSource
{
internal:
	AppFrameworkViewSource(AppControllerBase* pAppController, AppLaunchInfo* pLaunchInfo)
	{
		_pAppController = pAppController;
		_pLaunchInfo = pLaunchInfo;
	}

public:

    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
    {
        return ref new App(_pAppController, _pLaunchInfo);
    }

private:
	P<AppControllerBase>	_pAppController;
	P<AppLaunchInfo>		_pLaunchInfo;
};

#endif

ref class App sealed :	public ::Windows::UI::Xaml::Application,
						public ::Windows::UI::Xaml::Markup::IXamlMetadataProvider
{
internal:
	App(AppControllerBase* pAppController, AppLaunchInfo* pLaunchInfo)
	{	
		_pAppController = pAppController;
		_pLaunchInfo = pLaunchInfo;

		_mainInit();

        AppControllerBase::_set(_pAppController);

		InitializeComponent();

		Suspending +=
			ref new Windows::UI::Xaml::SuspendingEventHandler(this, &App::suspending);

		Resuming +=
			ref new Windows::Foundation::EventHandler<Object^>(this, &App::resuming);
	}

public:

	
	virtual ::Windows::UI::Xaml::Markup::IXamlType^ GetXamlType(::Windows::UI::Xaml::Interop::TypeName type)
	{	
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		int x=0;
		return nullptr;//return getXamlTypeInfoProvider()->GetXamlTypeByType(type);

        BDN_WINUWP_TO_PLATFORMEXC_END
	}

	virtual ::Windows::UI::Xaml::Markup::IXamlType^ GetXamlType(::Platform::String^ fullName)
	{
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		int x=0;
		return nullptr;//return getXamlTypeInfoProvider()->GetXamlTypeByName(fullName);

        BDN_WINUWP_TO_PLATFORMEXC_END
	}

	virtual ::Platform::Array<::Windows::UI::Xaml::Markup::XmlnsDefinition>^ GetXmlnsDefinitions()
	{
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		int x=0;
		return nullptr;//return ref new ::Platform::Array<::Windows::UI::Xaml::Markup::XmlnsDefinition>(0);

        BDN_WINUWP_TO_PLATFORMEXC_END
	}


protected:

	virtual void OnLaunched(  Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ pArgs ) override
	{
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		try
		{            
            // ensure that the global dispatcher access object is initialized
            bdn::winuwp::DispatcherAccess::get();

			_pAppController->beginLaunch(*_pLaunchInfo);
			_pAppController->finishLaunch(*_pLaunchInfo);				
		}
		catch(std::exception& e)
		{
			std::cerr << e.what() << std::endl;

			// we will exit abnormally. Still call onTerminate.
			_pAppController->onTerminate();
        
			// let error through.
			throw;
		}

        BDN_WINUWP_TO_PLATFORMEXC_END
	}

	void unhandledException(::Platform::Object^ pSender, ::Windows::UI::Xaml::UnhandledExceptionEventArgs^ pArgs)
	{
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		String errorMessage( pArgs->Message->Data() );

		logError("Unhandled top level exception: "+errorMessage);

#ifdef BDN_DEBUG
		if (IsDebuggerPresent())
            __debugbreak();
#endif

        BDN_WINUWP_TO_PLATFORMEXC_END
	}
	
	void InitializeComponent()
	{
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		UnhandledException += ref new ::Windows::UI::Xaml::UnhandledExceptionEventHandler( this, &App::unhandledException );

        BDN_WINUWP_TO_PLATFORMEXC_END
	}

	void suspending(Platform::Object^ pSender, Windows::ApplicationModel::SuspendingEventArgs^ pArgs)
	{
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		_pAppController->onSuspend();

        BDN_WINUWP_TO_PLATFORMEXC_END
	}

	void resuming(Platform::Object^ pSender, Platform::Object^ pArgs)
	{
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		_pAppController->onResume();

        BDN_WINUWP_TO_PLATFORMEXC_END
	}



internal:
	P<AppControllerBase> _pAppController;
	P<AppLaunchInfo>	 _pLaunchInfo;
};


int _uiAppMain(AppControllerBase* pAppController, Platform::Array<Platform::String^>^ args)
{
    BDN_WINUWP_TO_PLATFORMEXC_BEGIN

    try
    {
	    P<AppLaunchInfo> pLaunchInfo = newObj<AppLaunchInfo>();

	    // note: apparently the args array is always empty (there does not seem to be a way
	    // to pass commandline arguments to a universal app from the outside).
	    // One can apparently add arguments via a XAML, if there is one. But since we do not
	    // have one (at least not at launch) it will probably always be empty. But we
	    // handle it nevertheless.
	    std::vector<String> argVector;
	    for(auto a: args)
		    argVector.push_back( a->Data() );

	    if(argVector.empty())
		    argVector.push_back("");

	    pLaunchInfo->setArguments(argVector);

	    Windows::UI::Xaml::Application::Start(
		    ref new Windows::UI::Xaml::ApplicationInitializationCallback(
			    [pLaunchInfo, pAppController](Windows::UI::Xaml::ApplicationInitializationCallbackParams^ pParams)
			    {
				    ref new App(pAppController, pLaunchInfo);		
			    } ) );

	    /*Windows::UI::Xaml::Application^ pApp = Windows::UI::Xaml::Application::Current;
	
	    auto frameworkViewSource = ref new AppFrameworkViewSource(pAppController, pLaunchInfo);

        ::Windows::ApplicationModel::Core::CoreApplication::Run(frameworkViewSource);
	    */

    }


    BDN_WINUWP_TO_PLATFORMEXC_END

    return 0;
}


}



