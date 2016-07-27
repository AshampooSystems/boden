#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/Thread.h>

namespace bdn
{

void _uiAppInit(AppControllerBase* pAppController)
{
    Thread::_setMainId( Thread::getCurrentId() );

    AppControllerBase::_set(pAppController);

    AppLaunchInfo launchInfo;

    try
    {
        pAppController->beginLaunch( launchInfo );
        pAppController->finishLaunch( launchInfo );
    }
    catch(std::exception& e)
    {
        // we will exit abnormally. Still call onTerminate.
        pAppController->onTerminate();
        
        // let error through.
        throw;
    }
}

}






