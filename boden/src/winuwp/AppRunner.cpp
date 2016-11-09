#include <bdn/init.h>
#include <bdn/winuwp/AppRunner.h>



#include <bdn/win32/ThreadLocalStorageManager.h>

namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( ThreadLocalStorageManager, ThreadLocalStorageManager::get );

}
}



namespace bdn
{
namespace winuwp
{    


    
ref class App sealed :	public ::Windows::UI::Xaml::Application,
						public ::Windows::UI::Xaml::Markup::IXamlMetadataProvider
{
internal:
	App( AppRunner* pAppRunner )
	{	
        _pAppRunner = pAppRunner;

        InitializeComponent();

		Suspending +=
			ref new Windows::UI::Xaml::SuspendingEventHandler(this, &App::suspending);

		Resuming +=
			ref new Windows::Foundation::EventHandler<Object^>(this, &App::resuming);
	}

public:

	
	virtual ::Windows::UI::Xaml::Markup::IXamlType^ GetXamlType(::Windows::UI::Xaml::Interop::TypeName type)
	{	
        BDN_ROOT_BEGIN;

		int x=0;
		return nullptr;//return getXamlTypeInfoProvider()->GetXamlTypeByType(type);

        BDN_ROOT_END;
	}

	virtual ::Windows::UI::Xaml::Markup::IXamlType^ GetXamlType(::Platform::String^ fullName)
	{
        BDN_ROOT_BEGIN;

		int x=0;
		return nullptr;//return getXamlTypeInfoProvider()->GetXamlTypeByName(fullName);

        BDN_ROOT_END;
	}

	virtual ::Platform::Array<::Windows::UI::Xaml::Markup::XmlnsDefinition>^ GetXmlnsDefinitions()
	{
        BDN_ROOT_BEGIN;

		int x=0;
		return nullptr;//return ref new ::Platform::Array<::Windows::UI::Xaml::Markup::XmlnsDefinition>(0);

        BDN_ROOT_END;
	}


protected:

	virtual void OnLaunched(  Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ pArgs ) override
	{
        BDN_ROOT_BEGIN;

        _pAppRunner->launch();

        BDN_ROOT_END_EXCEPTIONS_ARE_FATAL;
	}

	void unhandledUwpException(::Platform::Object^ pSender, ::Windows::UI::Xaml::UnhandledExceptionEventArgs^ pArgs)
	{
        BDN_ROOT_BEGIN

        try
        {
            throw pArgs->ExceptionObject;
        }
        catch(...)
        {
            unhandledException()
        }

		String errorMessage( pArgs->Message->Data() );

		logError("Unhandled top level exception: "+errorMessage);

        BDN_ROOT_END_EXCEPTIONS_ARE_FATAL;
	}
	
	void InitializeComponent()
	{
        BDN_WINUWP_TO_PLATFORMEXC_BEGIN

		UnhandledException += ref new ::Windows::UI::Xaml::UnhandledExceptionEventHandler( this, &App::unhandledUwpException );

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
	P<AppRunnerBase> _pAppRunner;
};


    
AppLaunchInfo AppRunner::makeAppLaunchInfo(Platform::Array<Platform::String^>^ args)
{
    AppLaunchInfo launchInfo;

    // there is no way to pass commandline arguments to a universal windows
    // app. The args parameter does not contain commandline arguments.

    // So we return an empty launchInfo object.

    return launchInfo;
}

AppRunner::AppRunner( std::function< P<AppControllerBase>() > appControllerCreator, Platform::Array<Platform::String^>^ args )
    : AppRunnerBase( appControllerCreator, makeAppLaunchInfo(args) )
{
}

void AppRunner::prepareLaunch()
{
    AppRunnerBase::prepareLaunch();

    _pMainDispatcher = newObj< bdn::winuwp::Dispatcher >( Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher );
}

void AppRunner::entry()
{
    try
    {
        P<AppRunner> pThis = this;

        Windows::UI::Xaml::Application::Start(
		    ref new Windows::UI::Xaml::ApplicationInitializationCallback(
			    [pThis](Windows::UI::Xaml::ApplicationInitializationCallbackParams^ pParams)
			    {
				    ref new App(pThis);		
			    } ) );

    }
    catch(...)
    {
        unhandledException(false);
        // just let the exception through. It will terminate the program.
        throw;
    }

    return 0;
}    

void AppRunner::disposeMainDispatcher()
{    
    _pMainDispatcher->dispose();
}
    
}
}



