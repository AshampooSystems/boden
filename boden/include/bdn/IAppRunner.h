#ifndef BDN_IAppRunner_H_
#define BDN_IAppRunner_H_

#include <bdn/AppLaunchInfo.h>
#include <bdn/IDispatcher.h>

#include <vector>
#include <map>

namespace bdn
{
    
    
/** Defines the generic interface for the app runner.
	
	Each app has exactly one app runner object. The app runner's responsibility is
	to initialize the Boden framework and the app and to implement or manage
	the app's main loop and lifetime.
	*/
class IAppRunner : BDN_IMPLEMENTS IBase
{
public:
	
	/** Causes the app to close gracefully close at the next opportunity, if that is possible.
	
		Note that some platforms do not allow apps to initiate their own closing. For example,
		iOS apps must never close themselves - they can only be closed by the user. In such cases
		calling this function has no effect.
		
		So you should always be prepared for the case that the exit might not actually happen.
		If your app has nothing relevant to do anymore then you should consider
		displaying a message to the user to indicate that fact.

		This function can be called from any thread.
	*/
	virtual void initiateExitIfPossible(int exitCode)=0;


	
	/** Returns the app's launch information.*/
	virtual const AppLaunchInfo& getLaunchInfo() const=0;


    /** Returns true if the application is a commandline / terminal application
        (also sometimes called "console application").
    
        Commandline applications usualy use stdio (STDOUT / std::cout, etc.) to interact with the user.
        They are not supported on all platforms.

        On platforms that support both commandline and graphical apps, the compile
        definition BDN_COMPILING_COMMANDLINE_APP can be set when compiling the application
        to select which type of application one wants to build.

        */
    virtual bool isCommandLineApp() const=0;


    /** Returns the main dispatcher that is responsible handling app events
        and scheduling of work items.*/
    virtual P<IDispatcher> getMainDispatcher()=0;


    /** Notifies the app runner taht an unhandled exception was encountered.

        The exception must be the currently active one that is accessible with
        std::current_exception().

        The canKeepRunning parameter indicates whether or not the exception can be ignored.
                
        The unhandledException implementation must check if there is an app controller and if there is
        then it must call the app controller's AppControllerBase::unhandledProblem.

        \return true if the exception should be ignored and the app should continue (only allowed if
            canKeepRunning is true). False if the app should terminate.
        */
    virtual bool unhandledException(bool canKeepRunning)=0;

};


/** Returns the app's runner object.*/
P<IAppRunner> getAppRunner(); 		


void _setAppRunner(IAppRunner* pAppRunner);

   
    
}

#endif

