#ifndef BDN_WINUWP_AppRunner_H_
#define BDN_WINUWP_AppRunner_H_

#include <bdn/AppRunnerBase.h>

namespace bdn
{
namespace winuwp
{
    
    
/** IAppRunner implementation for winuwp.
	*/
class AppRunner : public AppRunnerBase
{
private:
    AppLaunchInfo makeAppLaunchInfo(Platform::Array<Platform::String^>^ args)
    {
        AppLaunchInfo launchInfo;

        // there is no way to pass commandline arguments to a universal windows
        // app. The args parameter does not contain commandline arguments.

        // So we return an empty launchInfo object.

        return launchInfo;
    }
public:
	AppRunner( std::function< P<AppControllerBase>() > appControllerCreator, Platform::Array<Platform::String^>^ args )
        : AppRunnerBase( appControllerCreator, makeAppLaunchInfo(args) )
	{
	}


    void prepareLaunch() override;
    int entry();    	

protected:    
	void mainLoop() override;

	void disposeMainDispatcher() override;
};
  		

    
}    
}

#endif

