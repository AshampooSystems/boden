#ifndef BDN_MAC_AppRunner_HH_
#define BDN_MAC_AppRunner_HH_

#include <bdn/AppRunnerBase.h>

#import <Cocoa/Cocoa.h>

namespace bdn
{
namespace mac
{

class AppRunner : public AppRunnerBase
{
private:
    AppLaunchInfo _makeLaunchInfo(int argCount, char* args[] );

public:
    AppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[]);
    
    int entry();
    
    
    
    /** Used internally. Do not call.*/
    void _applicationWillFinishLaunching(NSNotification* notification);

    /** Used internally. Do not call.*/
    void _applicationDidFinishLaunching(NSNotification* notification);
    
    /** Used internally. Do not call.*/
    void _applicationDidBecomeActive(NSNotification* notification);
    
    /** Used internally. Do not call.*/
    void _applicationDidResignActive(NSNotification* notification);
    
    /** Used internally. Do not call.*/
    void _applicationWillTerminate(NSNotification* notification);

    
};

}
}


#endif
