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



/** \def BDN_ROOT_BEGIN
    
    Marks the beginning of a code root, i.e. a place where the operating system
    passes control to the App.

    The end of a root code block must be marked with #BDN_ROOT_END or #BDN_ROOT_END_EXCEPTIONS_ARE_FATAL.

    Examples for roots are the global main() function and thread entry point functions.
    On some systems event handlers can also be roots.

    The boden framework will not let exceptions go across a root boundaries. When an
    unhandled exception reaches a root then IAppRunner::unhandledException is called,
    which in turn calls IAppController::unhandledProblem. The app controller can then
    decide if the app should be terminated, or if the exception should be ignored.

    Example:

    \code

    void myThreadFunction()
    {
        BDN_ROOT_BEGIN;

        ... thread code


        // if unhandled exceptions reach this point then AppControllerBase::unhandledProblem
        // is called, which decides if the app terminates or not.
        BDN_ROOT_END;
    }

    \endcode
*/


/** \def BDN_ROOT_END

    Marks the end of a root code block. See #BDN_ROOT_BEGIN
*/


/** \def BDN_ROOT_END_EXCEPTIONS_ARE_FATAL

    An alternative to #BDN_ROOT_END for cases where all exceptions that reach the root
    must be considered fatal and the app cannot continue to run.
*/

#if BDN_PLATFORM_WINUDP

#define BDN_ROOT_BEGIN  try{

#define BDN_ROOT_END \
                        } \
                        catch(...) \
                        { \
                            if(!bdn::getAppRunner()->unhandledException(true)) \
                                /* we want to throw a Platform::Exception here - Windows might show information about it in the crash messae*/ \
                                throw bdn::winuwp::currentExceptionToPlatformException(); \
                        }


#define BDN_ROOT_END_EXCEPTIONS_ARE_FATAL \
                        } \
                        catch(...) \
                        { \
                            bdn::getAppRunner()->unhandledException(false); \
                            /* we want to throw a Platform::Exception here - Windows might show information about it in the crash messae*/ \
                            throw bdn::winuwp::currentExceptionToPlatformException(); \
                        }

#else

#define BDN_ROOT_BEGIN  try{

#define BDN_ROOT_END \
                        } \
                        catch(...) \
                        { \
                            if(!bdn::getAppRunner()->unhandledException(true)) \
                                std::terminate(); \
                        }


#define BDN_ROOT_END_EXCEPTIONS_ARE_FATAL \
                        } \
                        catch(...) \
                        { \
                            bdn::getAppRunner()->unhandledException(false); \
                            std::terminate(); \
                        }

#endif

    
    
}

#endif

