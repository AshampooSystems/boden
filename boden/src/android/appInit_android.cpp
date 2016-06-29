#include <bdn/init.h>
#include <bdn/appInit.h>

#include <jni.h>


namespace bdn
{
    namespace android
    {
        // these functions are implemented by the appInit.h macros (e.g. BDN_INIT_UI_APP)
        bdn::P<bdn::AppControllerBase> _createAppController();
        std::function<int(const AppLaunchInfo&)> _getAppFunc();
    }
}


static bdn::P<bdn::AppControllerBase> _doAppInit()
{
    bdn::P<bdn::AppControllerBase> pAppController = bdn::android::_createAppController();

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

    return pAppController;
}


extern "C" JNIEXPORT void JNICALL Java_io_boden_boden_MainActivity_initApp( JNIEnv* env, jobject obj, jobject mainViewGroupObj)
{
    bdn::JavaConnector* pConnector = bdn::JavaConnector::get();

    pConnector->setEnv(env);
    pConnector->setMainActivityObject(obj);
    pConnector->setMainViewGroupObject(mainViewGroupObj);

    try
    {
        std::function<int(const AppLaunchInfo&)> appFunc = bdn::android::_getAppFunc();

		_mainInit();

        if(appFunc!=std::nullptr)
        {
            // we have a commandline app with a simple main-like app function.
            // We must run that in a secondary thread.
            // Note that since we have a commandline app it does not matter that
            // the app that runs the main function is not actually the original main thread.
            // Also, it does not matter that Java callbacks will come in a different thread
            // than the one that calls the appFunc (since there are almost no Java callbacks
            // that are relevant in a commandline app).
            // Also note that the callFromMainThread functions do not work in a commandline app
            // on ANY platform, so that is not a problem either.

            // Note that we cannot pretend that this thread is the main thread.
            // If we ever decide that we want to have some commandline-emulating UI then it is
            // important that the UI classes know which thread is ACTUALLY the main thread.
            // So we cannot fake this here.
            

            Thread::exec(
                [appFunc]()
                {
                    P<bdn::P<bdn::AppControllerBase> pAppController = _doAppInit();

                    try
                    {
                        appFunc();
                    }
                    catch(std::exception& e)
                    {
                        // we will exit abnormally. Still call onTerminate.
                        pAppController->onTerminate();
                        
                        // let error through.
                        throw;
                    }        

                    pAppController->onTerminate();
                } );
        }
        else
        {

            // normal UI app. Just init and return.
            _doAppInit();
        }        
    }
    catch(std::exception& e)
    {
        pConnector->convertToJavaException(e);
        return;
    }
}




