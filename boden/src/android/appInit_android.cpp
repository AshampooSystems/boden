#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/Thread.h>
#include <bdn/android/JActivity.h>

namespace bdn
{
namespace android
{

void _uiAppInit(AppControllerBase* pAppController, jobject activityRef)
{
    Thread::_setMainId( Thread::getCurrentId() );

    AppControllerBase::_set(pAppController);

    JActivity       activity( bdn::java::LocalReference(activityRef) );
    JResources      resources = activity.getResources();
    JConfiguration  config = resources.getConfiguration();


    int densityDpi = config.densityDpi();

    // on android scale factor 1 is 160 dpi.
    // Note that smaller DPI values are possible, so the scale factor
    // can be <1 !
    double scaleFactor = densityDPI / 160.0;

    UIProvider::setCurrentScaleFactor

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
}






