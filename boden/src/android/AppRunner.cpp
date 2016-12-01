#include <bdn/init.h>
#include <bdn/android/AppRunner.h>


namespace bdn
{
namespace android
{


AppLaunchInfo AppRunner::_makeLaunchInfo()
{
    // there are no start parameters for an android app.
    AppLaunchInfo launchInfo;

    std::vector<String> args;

    // add a dummy arg for the program name
    args.push_back( String() );

    launchInfo.setArguments(args);

    return launchInfo;
}

AppRunner::AppRunner( std::function< P<AppControllerBase>() > appControllerCreator )
: AppRunnerBase(appControllerCreator, AppRunner::_makeLaunchInfo() )
{
    _pMainDispatcher = newObj<Dispatcher>( JLooper::getMainLooper() );
}


void AppRunner:entry()
{
    // entry is called from the JAVA main thread when the main activity is initialized
    launch();
}


P<IDispatcher> AppRunner::getMainDispatcher()
{
    return _pMainDispatcher;
}


void AppRunner::disposeMainDispatcher()
{
    _pMainDispatcher->dispose();
}

void AppRunner::initiateExitIfPossible(int exitCode)
{
    // android apps should never exit on their own. So, do nothing here.
    int x=0;
    x++;
}


void AppRunner::prepareLaunch() override
{

}


/** Used internally. Do not call.*/
void _storeParametersForUnhandledException(::Platform::Exception^ pUwpException, bool canKeepRunningAfterException);

/** Used internally. Do not call.*/
void _unhandledUwpException(::Platform::Object^ pSender, ::Windows::UI::Xaml::UnhandledExceptionEventArgs^ pArgs);


protected:

void disposeMainDispatcher() override;

private:
struct StoredUnhandledException : public Base
{
    bool				canKeepRunning;
    std::exception_ptr	exceptionPtr;
    HRESULT				hresult;
    ::Platform::String^	message;
};

BDN_SAFE_STATIC_THREAD_LOCAL( StoredUnhandledException, getThreadLocalStoredUnhandledException );

P<Dispatcher> _pMainDispatcher;


}
}
