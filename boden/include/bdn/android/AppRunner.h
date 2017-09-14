#ifndef BDN_ANDROID_AppRunner_H_
#define BDN_ANDROID_AppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/android/Dispatcher.h>

namespace bdn
{
namespace android
{
    
    
/** IAppRunner implementation for android.
	*/
class AppRunner : public AppRunnerBase
{
private:
    AppLaunchInfo _makeLaunchInfo();

public:
	AppRunner( std::function< P<AppControllerBase>() > appControllerCreator );

	bool isCommandLineApp() const override;

	/** Main entry function of the app runner.*/
    void entry();


	P<IDispatcher> getMainDispatcher() override;


	void initiateExitIfPossible(int exitCode) override;



protected:
	void disposeMainDispatcher() override;

	P<Dispatcher> _pMainDispatcher;
};
  		

    
}    
}

#endif

