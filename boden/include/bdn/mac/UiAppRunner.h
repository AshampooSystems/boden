#ifndef BDN_MAC_UiAppRunner_H_
#define BDN_MAC_UiAppRunner_H_

#include <bdn/AppRunnerBase.h>

namespace bdn
{
namespace mac
{

class UiAppRunner : public AppRunnerBase
{
private:
    AppLaunchInfo _makeLaunchInfo(int argCount, char* args[] );

public:
    UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[]);


    int entry();
    
    
    void initiateExitIfPossible(int exitCode) override;
    
    
    P<IDispatcher> getMainDispatcher() override
    {
        return _pMainDispatcher;
    }
    
    
    void disposeMainDispatcher() override;
    
    
    /** Used internally. Do not call.*/
    void _applicationWillFinishLaunching();

    /** Used internally. Do not call.*/
    void _applicationDidFinishLaunching();
    
    /** Used internally. Do not call.*/
    void _applicationDidBecomeActive();
    
    /** Used internally. Do not call.*/
    void _applicationDidResignActive();
    
    /** Used internally. Do not call.*/
    void _applicationWillTerminate();
    
    
private:
    P<IDispatcher> _pMainDispatcher;
};


}
}


#endif
