#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/Thread.h>

namespace bdn
{

static P<AppLaunchInfo> createLaunchInfo()
{
    // there are no start parameters for an android app.
    P<AppLaunchInfo> pLaunchInfo = newObj<AppLaunchInfo>();

    std::vector<String> args;

    // add a dummy arg for the program name
    args.push_back( String() );

    pLaunchInfo->setArguments(args);

    return pLaunchInfo;
}


void _uiAppInit(AppControllerBase* pAppController)
{
    Thread::_setMainId( Thread::getCurrentId() );

    AppControllerBase::_set(pAppController);

    P<AppLaunchInfo> pLaunchInfo = createLaunchInfo();

    try
    {
        pAppController->beginLaunch( *pLaunchInfo );
        pAppController->finishLaunch( *pLaunchInfo );
    }
    catch(std::exception& e)
    {
        // we will exit abnormally. Still call onTerminate.
        pAppController->onTerminate();
        
        // let error through.
        throw;
    }
}


class CommandLineAppRunnable : public Base, BDN_IMPLEMENTS IThreadRunnable
{
public:
    CommandLineAppRunnable(int (*appFunc)(const AppLaunchInfo& launchInfo), const AppLaunchInfo& launchInfo)
    {
        _appFunc = appFunc;
        _launchInfo = launchInfo;
    }

    void run()
    {
        // just call the app func.
        _appFunc(_launchInfo);

        // when the app func is done then we have nothing else to do.
        // just end the thread.
        int x=0;
        x++;
    }

    void signalStop()
    {
        // tell the app controller to stop. If the user has implemented graceful stopping
        // in a custom app controller then this will call it.
        AppControllerBase::get()->closeAtNextOpportunityIfPossible(0);
    }

private:
    int (*_appFunc)(const AppLaunchInfo &launchInfo);

    AppLaunchInfo _launchInfo;
};


void _commandLineAppInit(int (*appFunc)(const AppLaunchInfo& launchInfo), AppControllerBase* pAppController)
{
    // on android there is no actual way for an app to take over the real main event loop.
    // So we emulate the effect by starting a new thread that runs the appFunc.
    // Note that a pure commandline app has no UI, so there can be no problems with the fact that
    // we run everything in a new thread.

    _uiAppInit(pAppController);

    P<Thread> pThread = newObj<Thread>( newObj<CommandLineAppRunnable>(appFunc, *createLaunchInfo() ) );
    pThread->detach();
}

}






