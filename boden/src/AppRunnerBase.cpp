#include <bdn/init.h>
#include <bdn/AppRunnerBase.h>

#include <bdn/log.h>
#include <bdn/Thread.h>
#include <bdn/View.h>


namespace bdn
{

void AppRunnerBase::launch()
{
	// ensure that the global mutex for safeinit is allocated. It is important
	// that this happens before multiple threads are created.
	SafeInitBase::_ensureReady();

	// mark the current thread as the main thread
	Thread::_setMainId( Thread::getCurrentId() );	

    // the view's "hierarchy and core mutex" is accessed in View destructors.
    // That means that we should ensure that it is created very early, so
    // that it is destructed as one of the last objects.
    View::getHierarchyAndCoreMutex();

    
	try
	{
        // do additional platform-specific initialization (if needed)
        platformSpecificInit();

        // set the app controller as the global one
	    P<AppControllerBase> pAppController = _appControllerCreator();
	    AppControllerBase::_set( pAppController );


		beginningLaunch();

		_appControllerBeginLaunchCalled = true;
		pAppController->beginLaunch(_launchInfo);

		finishingLaunch();

		pAppController->finishLaunch(_launchInfo);

		launched();
	}
	catch(...)
	{
		// we will exit abnormally. Still call onTerminate.
		terminating();
        
		// let error through.
		throw;
	}    
}

void AppRunnerBase::runMainLoop()
{
    BDN_LOG_AND_IGNORE_EXCEPTION( mainLoop(), "Top level exception encountered in main loop. Ignoring.");

	terminating();
}

void AppRunnerBase::terminating()
{
    BDN_LOG_AND_IGNORE_EXCEPTION(
        if(_appControllerBeginLaunchCalled)
            AppControllerBase::get()->onTerminate()
        ,"AppController onTerminating threw exception. Ignoring." );

    // the main dispatcher may still contain some pending items. However, when we return
    // from terminating then the destruction of global variables may begin almost immediately.
    // Since the app runner/ holds a reference to the dispatcher, that means that any objects referenced in pending
    // work items will be destroyed with the app runner if we do nothing.
    // And since the app runner is created first, it will be destroyed last. Which means that if the
    // object's destructors access any global objects then those might already be destroyed and we
    // can get a crash.
    BDN_LOG_AND_IGNORE_EXCEPTION( disposeMainDispatcher(), "AppRunnerBase::disposeMainDispatcher threw exception. Ignoring." );


    BDN_LOG_AND_IGNORE_EXCEPTION( platformSpecificCleanup(), "AppRunnerBase::platformSpecificCleanup threw exception. Ignoring." );
}



}