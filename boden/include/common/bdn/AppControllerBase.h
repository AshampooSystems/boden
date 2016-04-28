#ifndef BDN_AppControllerBase_H_
#define BDN_AppControllerBase_H_

#include <vector>
#include <map>

namespace bdn
{
    
    
/** Base class for app controllers.
 
    An app controller manages the lifecycle of the app. It performs initialization
    on startup, handles operating system requests to exit, performs app shutdown, etc.
 
    During the lifetime of the app, the app controllers lifecycle notification functions
    will be called at various points. AppControllerBase provides a default implementation
    for these that does noting.
 
    Usually custom app controller classes do not derive directly from AppControllerBase
	(although they can). Consider deriving from UiAppControllerBase for an app with a
	graphical user interface, or from CommandLineAppControllerBase for a commandline app.
	
	You control which app controller class your app uses by passing it to the app initialization macros
	#BDN_INIT_UI_APP(), #BDN_INIT_COMMANDLINE_APP(), ...
 
    */
class AppControllerBase : public Base
{
public:
    
	/** Sets the app's execution arguments. These are usually passed as commandline arguments when
		the app is executed.

		The first item in the argument list is a representation of the program name and/or path.
		You should not depend on this first entry, though. It is highly dependent on the platform
		and on how the app was called. Sometimes the first entry can be a full path. Sometimes it
		is a relative path or just a file name. On some platforms it can also simply be an empty string.

		The remaining items are the parameters passed to the app by the caller.

		The default implementation of this function does nothing. Derived classes can override
		this function if they need access to the arguments.
		*/
	virtual void setArguments(const std::vector<String>& args)
	{
		// do nothing by default
	}

	
	/** Sets the app's launch information. 

		This is called by the system before beginLaunch().

		The launch information is a system-dependent map with name-value entries.

		The controller implementation can use the launch information to optimize startup.
        For example, the launch information might contain an entry that indicates that the app
        is launched to handle a specific URL - in that case the controller might skip
		preparing its normal start screen because it knows that it will most likely not be shown.

		The possible value names and their contents depend on the platform.

		The default implementation does nothing and does not store the launch information. Derived
		classes can override this to handle the information..
		*/
	virtual void setLaunchInfo(const std::map<String,String>& launchInfo)
	{
		// do nothing by default
	}

			

    /** Called when the app launch has begun.
     
        beginLaunch should set up your user interface here and prepare the app to run.
     
        If the operating system supports automatically restoring the state of
        the app's UI (for example, on iOS) then the restoration has NOT yet happened
        yet when this function is called.

		Note that additional information about how and why the app is run might
		be available in the launch information. See setLaunchInfo().

		If you need to access the app's commandline arguments then you capture them
		by overriding setArguments().
        */
    virtual void beginLaunch()
    {
        // do nothing by default
    }
    
    
    /** Called when the app has finished launching, just before it becomes active.
     
        If the operating system supports automatically restoring the state of
        the app's UI (for example, on iOS) then the restoration has finished
        when this function is called.
     
        This function is intended for making final UI tweaks, just before the app
        becomes active and visible.
     
        Note that additional information about how and why the app is run might
		be available in the launch information. See setLaunchInfo().

		If you need to access the app's commandline arguments then you capture them
		by overriding setArguments().
     */
    virtual void finishLaunch()
    {
        // do nothing by default
    }
    
    
    
    /** The app has become active and ready for the user to interact with it.
     
        For desktop apps this means that the app has come into the foreground,
        usually one of the app's frame windows has been activated or will be activated
        shortly.
        
        For mobile apps this means that the app is the active app (visible in the foreground)
        or one of the active apps (if the system supports multiple apps on the screen at the same
        time).
        */
    virtual void onActivate()
    {
        // do nothing by default
    }
    
    
    /** The app will be deactivated or was just deactivated. The user does not interact with
        this app anymore and it is going into the background.
     
        For desktop app this means that the app is not the foreground application anymore
        and its frame windows are not active.
     
        For mobile apps this means that the app was either interrupted (for example by a phone call),
        or the user has switched to another app (or the home screen).
     
        Apps should reduce their activity to a minimum while they are inactive. Games might want to
        pause.
     
        Note that the app can continue to run while it is in deactivated state.
        But on some systems it will often enter background mode shortly afterwards
        (see onEnterBackgroundMode() )
     
        */
    virtual void onDeactivate()
    {
        // do nothing by default
    }
    
    
    
    /** The application is about to be suspended / frozen.
        
        An app should store state information for resume or restart here and
        release shared resources. The app should assume that its code execution
        is going to stop while it is suspended and it should not expect to get
        any processing resources during this time. An exception are registered
        background tasks - see below for more information.
     
        An app should also expect that it might terminate without notification
        while being suspended. So onSuspend should store any relevant state information
        that might be needed for the next launch and perform the necessary cleanup.
     
        On desktop systems onSuspend is typically called when the computer
        enters sleep mode (although that is not always guaranteed on all systems).

        On mobile systems onSuspend is typically called at some point after the app has
        been deactivated. The operating system suspends the app to save resources.
     
        onSuspend should return as quickly as possible. On many systems it has a limited
        execution time (typically 1-5 seconds) and the app might be terminated
        if this time is exceeded.
     
        Some systems offer the option to perform limited, registered background tasks
        while in suspended mode. These can be used to do longer cleanup tasks that would take too
        long for onSuspend. They can also be used to perform longer running low-level background
        operations during the suspension. See LimitedBackgroundTask for more
        information.
        */
    virtual void onSuspend()
    {
        // do nothing by default
    }
    
    
    
    /** Called when an app is about to exit suspended state
        (see onSuspend() ).
     
        The app will usually be activated shortly afterwards.
     
        */
    virtual void onResume()
    {
        // do nothing by default
    }
    

    
    /** The app is about to be terminated by the operating system (i.e. it will be
        fully unloaded and stop to run).
     
        \important Not all operating systems support the onTerminate notification.
        On some systems termination will happen without notification. You should
        usually use onSuspend() to store your application state and prepare for
        termination.
     
        onTerminate should return as quickly as possible. On many systems it has a limited
        execution time (typically 5 seconds) and the app might be killed without completing
        the onTerminate handler if this time is exceeded.
        */
    virtual void onTerminate()
    {
        // do nothing by default
    }
     
    

	/** Causes the app to close at the next opportunity, if that is possible.
	
		Note that some platforms do not allow apps to exit on their own. For example,
		iOS apps must never close themselves. They can only be closed by the user.

		Also, even if the platform supports exiting, it might be possible for
		some other component of the app to veto the exit request and prevent the exit.

		So you should always consider the possibility that the exit might not actually happen.
		If your app has nothing relevant to do anymore then you should consider
		simply displaying a message to the user to indicate that fact.
	*/
	virtual void closeAtNextOpportunityIfPossible(int exitCode)=0;
    
    
    /** Returns the global app controller instance.*/
    static P<AppControllerBase> get()
    {
        return _getGlobal();
    }
    
    
    /** Sets the global app controller instance.
     
        This is used internally by the framework. You should not call it.
     */
    static void _set(AppControllerBase* pAppController)
    {
        _getGlobal() = pAppController;
    }
    
    
private:
    static P<AppControllerBase>& _getGlobal()
    {
        static P<AppControllerBase> pController;
        
        return pController;
    }
    
};
    
    
}

#endif

