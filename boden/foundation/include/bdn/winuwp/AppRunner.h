#ifndef BDN_WINUWP_AppRunner_H_
#define BDN_WINUWP_AppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/winuwp/Dispatcher.h>

namespace bdn
{
namespace winuwp
{
    
    
/** IAppRunner implementation for winuwp.
	*/
class AppRunner : public AppRunnerBase
{
private:
    AppLaunchInfo makeAppLaunchInfo(Platform::Array<Platform::String^>^ args);

public:
	AppRunner( std::function< P<AppControllerBase>() > appControllerCreator, Platform::Array<Platform::String^>^ args );

    bool isCommandLineApp() const override;	

	/** Main entry function of the app runner.*/
    int entry();    

	P<IDispatcher> getMainDispatcher() override;

	void initiateExitIfPossible(int exitCode) override;


    void prepareLaunch() override;	


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
	
};
  		

    
}    
}

#endif

