#include <bdn/init.h>
#include <bdn/AppRunnerBase.h>

#include <bdn/log.h>
#include <bdn/Thread.h>


namespace bdn
{

void AppRunnerBase::launch()
{
	// ensure that the global mutex for safeinit is allocated. It is important
	// that this happens before multiple threads are created.
	SafeInitBase::_ensureReady();

	// mark the current thread as the main thread
	Thread::_setMainId( Thread::getCurrentId() );	

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
		onTerminate();
        
		// let error through.
		throw;
	}    
}

void AppRunnerBase::runMainLoop()
{
	try
	{
		mainLoopImpl();
	}
	catch( std::exception& e )
	{
		logError(e, "Top level exception encountered in main loop. Letting through.");

		terminating();

		throw;
	}
	catch(...)
	{
		logError("Top level exception of unknown type encountered in main loop. Letting through.");

		terminating();

		throw;
	}

	terminating();
}


void AppRunnerBase::terminating()
{
	try
	{
		AppControllerBase::get()->onTerminate();
	}
	catch( std::exception& e )
	{
		logError(e, "AppController onTerminating threw exception. Letting through.");
			
		throw;
	}
	catch(...)
	{
		logError("AppController onTerminating threw exception of unknown type. Letting through.");
			
		throw;
	}
}

}