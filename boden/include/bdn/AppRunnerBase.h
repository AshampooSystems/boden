#ifndef BDN_AppRunnerBase_H_
#define BDN_AppRunnerBase_H_

#include <bdn/AppLaunchInfo.h>
#include <bdn/IAppRunner.h>
#include <bdn/AppControllerBase.h>

#include <vector>
#include <functional>

namespace bdn
{
    
    
/** Base class for app runners. Implements a generic structure for
	more specific app runners.
	
	Platform specific implementations can override the virtual functions
	to implement custom steps.

	*/
class AppRunnerBase : public Base, BDN_IMPLEMENTS IAppRunner
{
public:
	AppRunnerBase( std::function< P<AppControllerBase>() > appControllerCreator, const AppLaunchInfo& launchInfo)
	{
		_appControllerCreator = appControllerCreator;
		_launchInfo = launchInfo;
	}

	virtual void launch();
    	
	
	/** Returns the app's launch information.*/
	const AppLaunchInfo& getLaunchInfo() const override
	{
		return _launchInfo;
	}

protected:

	/** Executes mainLoop(). Automatically handles exceptions and notifies the app controller
		when the loop ends.
		
		You usually do not need to override this if you want to provide a main loop implementation.
		Instead you should override mainLoop().
		*/
	virtual void runMainLoop();


	/** The core main loop implementation.

		Runs the apps main loop and does not return until the app should exit.

		It is ok for this to let exceptions through - they will be handled by the
		function that calls this.

		mainLoop also does NOT have to notify the app controller when the app exits
		- that is also done by the function that calls this.	
	*/
	virtual void mainLoop()=0;


	void setLaunchInfo(const AppLaunchInfo& launchInfo)
	{
		_launchInfo = launchInfo;
	}

	virtual void platformSpecificInit()
	{
		// do nothing by default
	}

	virtual void platformSpecificCleanup()
	{
		// do nothing by default
	}

    /** Stops the main dispatcher and prevents it from executing any more work.
        Any items currently in the queue must be released/destroyed.

        This is called when the app is about to terminate (after AppController::onTerminate was
        called).
    */
    virtual void disposeMainDispatcher()=0;

	virtual void beginningLaunch()
	{
		// do nothing by default
	}

	virtual void finishingLaunch()
	{
		// do nothing by default
	}

	virtual void launched()
	{
		// do nothing by default
	}

	virtual void terminating();


private:
	AppLaunchInfo								_launchInfo;
	std::function< P<AppControllerBase>()	>	_appControllerCreator;

	bool				_appControllerBeginLaunchCalled = false;
};
  		

    
    
}

#endif

