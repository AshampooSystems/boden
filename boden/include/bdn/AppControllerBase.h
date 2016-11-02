#ifndef BDN_AppControllerBase_H_
#define BDN_AppControllerBase_H_

#include <bdn/AppLaunchInfo.h>

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
  		

    /** Called when the app launch has begun.
     
        beginLaunch should set up your user interface here and prepare the app to run.
     
        If the operating system supports automatically restoring the state of
        the app's UI (for example, on iOS) then the restoration has NOT yet happened
        yet when this function is called.

		Note that additional information about how and why the app is run (including
		the commandline arguments) is available in the launchInfo parameter.
        */
    virtual void beginLaunch(const AppLaunchInfo& launchInfo)
    {
        // do nothing by default
    }
    
    
    /** Called when the app has finished launching, just before it becomes active.
     
        If the operating system supports automatically restoring the state of
        the app's UI (for example, on iOS) then the restoration has finished
        when this function is called.
     
        This function is intended for making final UI tweaks, just before the app
        becomes active and visible.
     
        Note that additional information about how and why the app is run (including
		the commandline arguments) is available in the launchInfo parameter.
     */
    virtual void finishLaunch(const AppLaunchInfo& launchInfo)
    {
        // do nothing by default
    }
    
    

	/** Must return true if the app needs a high performance loop at its core.
		See highPerformanceLoopIteration() for more information.
		
		Note that usingHighPerformanceLoop should not change the value it returns
		during the lifetime of the app. It may only be called once when the app starts,
		so changing the result during the lifetime may have no effect. In other words:
		the high performance loop cannot be activated at runtime, only during launch.

		The default implementation returns false.
		*/
	virtual bool usingHighPerformanceLoop() const
	{
		return false;
	}

	
	/** If the app has declared that it needs a high performance loop (see usingHighPerformanceLoop()) then
		this function will be called over and over again, by default as quickly as possible. Other pending work and
		user interface events will still be handled automatically in between calls.
		
		High performance loops are intended for certain games and other "realtime" apps that want to use every bit of available
		processing power in the main thread. While using all power is good for certain types of apps, it can also have negative effects,
		like high energy usage (i.e. battery draining), causing the computer to spin up its fans, etc. Because of this, high performance loops should
		only be used when they are really needed. Even in the case of games a normal timer is often a better choice than a high performance
		loop.

		The app controller can throttle down the call freqeuency of the high performance loop with the return value of highPerformanceLoopIteration().
		See below for more information.
		
		The way the loop function is integrated with other scheduled work depends on the implementation of the app scheduler.
		However, the scheduler guarantees that other pending work (including user interface events and work scheduled with the dispatcher)
		is still executed in a timely manner, so that the app remains responsive. Even work with priority "idle" will be executed.

		Some guidelines to using highPerformanceLoopIteration:

		- Each call to highPerformanceLoopIteration should only perform a small amount of work and return as quickly as possible, so that
		  user interface events and other scheduled work can be executed regularly.

		- Do not sleep or wait in highPerformanceLoopIteration. Instead, use the return value to cause the caller to wait before the next
		  iteration.

		- When the app does not need the highPerformanceLoopIteration for a while, use the return value to cause calls to happen less frequently.		  


		\return The return value can be used to throttle the call frequency of highPerformanceLoopIteration().
		
			If it <=0 then highPerformanceLoopIteration will be called as quickly as possible. Note that there are some platforms
			where "as quickly as possible" might be quite slow. For example, on the webems (web app) platform many browsers do not allow
			more than 100 or so iterations per second, even if the computer is very fast.
			
			If the return value is > 0 then the return value is the number of seconds that the next call of highPerformanceLoopIteration() should be delayed.
			Note that the return value a double, so you can return fractional values like 0.1 etc. to wait for less than a second.	
			As described above, this is intended to slow down in cases when the app does not need all the available speed - i.e. it is intended
			for temporary slow downs. Do not use this to emulate a timer with a fixed call frequency per second - use IDispatcher::createTimer
			for those cases.
		*/
	virtual double highPerformanceLoopIteration()
    {
        return 0;
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
     
    

    /** Returns the global app controller instance.*/
    static P<AppControllerBase> get()
    {
        return _getGlobalRef();
    }
    
    
    /** Sets the global app controller instance.
     
        This is used internally by the framework. You should not call it.
     */
    static void _set(AppControllerBase* pAppController)
    {
        _getGlobalRef() = pAppController;
    }
    
    
private:
    static P<AppControllerBase>& _getGlobalRef()
    {
        static P<AppControllerBase> pController;
        
        return pController;
    }
    
};
    
    
}

#endif

