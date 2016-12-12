#ifndef BDN_entry_H_
#define BDN_entry_H_


#include <bdn/IAppRunner.h>


namespace bdn
{

	/** Convenience function that calls IAppRunner::unhandledException.

		If there is no app runner object yet then this function does nothing.	
		*/
	inline bool unhandledException(bool canKeepRunningAfterException)
	{
		P<IAppRunner> pAppRunner = getAppRunner();
		if(pAppRunner!=nullptr)
			return pAppRunner->unhandledException(canKeepRunningAfterException);
		else
			return false;
	}

#if BDN_PLATFORM_WINUWP
	namespace winuwp
	{
		void _storeParametersForUnhandledException(::Platform::Exception^ pUwpException, bool canKeepRunningAfterException);
	}
#endif

}



/** \def BDN_ENTRY_BEGIN

    Marks the beginning of a an entry function, i.e. a place where the operating system
    passes control to the App.

	IMPORTANT: This macro is used internally in the Boden framework and does not usually have to be used by
	the App code. The Boden framework normally provides all entry point functions, and automatically
	does everything that has to be done. Apps only have to concern themselves with this if they contain custom code that interacts directly
	with the operating system or the runtime.

    The end of an entry function must be marked with #BDN_ENTRY_END.

    Examples for entry functions are the global main() function and thread entry point functions.
    Event handlers that get called directly by the operating system or runtime
	library can also be entry functions.

    The entry macros will normally not let exceptions go across entry function boundaries. When
    an unhandled exception reaches a entry function then IAppRunner::unhandledException is called,
	which in turn calls IAppController::unhandledProblem. The app controller can then
    decide if the app should be terminated or keep running (whether or not it is possible
	to keep running depends on the context).

	Platform specific note: on Android the BDN_ENTRY_BEGIN macro has a parameter. You must
	pass it a pointer to the JNIEnv object that the JNI function received from the Java side.

    Example:

    \code

    void myCustomEntryFunction()
    {
        BDN_ENTRY_BEGIN;

        ... thread code


        // if unhandled exceptions reach this point then AppControllerBase::unhandledProblem
        // is called, which decides if the app terminates or not.
        BDN_ENTRY_END(true);
    }

    \endcode
*/


/** \def BDN_ENTRY_END( canKeepRunningAfterException )

    Marks the end of a root code block. See #BDN_ENTRY_BEGIN.

	canKeepRunningAfterException must be a boolean that indicates whether or not
	the app can continue to run if an unhandled exception is encountered here.
*/



#if BDN_PLATFORM_WINUDP

#include <bdn/winuwp/platformError.h>

#define BDN_ENTRY_BEGIN  try{

/* Sometimes our "entry" functions are executed by the runtime because of a
call we made to it. If the runtime knows that it was called from app code
then it will let the exception through to the app code, which will then
get another chance to handle the exception. That is preferable to just
assuming that our code was called by a "root" routine in the runtime and that there
is not more chance for normal app code to handle it.
So we want to let the exception through here. If we were called by root runtime routines
then that will trigger the app's unhandledException handler to be called, which in turn
will call the routine in our app runner. So that is the same end result that we would get
if we called our unhandledException handler here.
BUT if we were ultimately called by app code then the exception is just tunneled through
and may be handled normally. If not then the exception is propagated up until at some point
the root is reached and our unhandled exception handler is called again.

There is one drawback to letting the UWP runtime decide: we need to convert our C++ exceptions
to UWP exceptions. I.e. we lose some information there. BUT we can work around that by storing
the original C++ exception and using that instead of the UWP version when our handler is called.
*/
#define BDN_ENTRY_END(canKeepRunningAfterException) \
                        } \
						catch(::Platform::Exception^ pException) \
						{ \
							/* let these through. They will cause the UWP unhandled exception handler in our \
							app to be called, which in turn forwards the exception to our own unhandledException \
							handler.*/ \
							bdn::winuwp::_storeParametersForUnhandledException(pException, canKeepRunningAfterException); \
							throw; \
						} \
						catch(std::exception& e) \
						{ \
							/* convert C++ exceptions to UWP exceptions. Then we will get the normal \
						    UWP handling, which in turn will call our unhandled exception handler, as explained above. */ \
							::Platform::Exception^ pException = bdn::winuwp::exceptionToPlatformException(e); \
							bdn::winuwp::_storeParametersForUnhandledException(pException, canKeepRunningAfterException); \
							throw pException; \
						} \
                        catch(...) \
                        { \
							/* all other exceptions (not UWP and not std) are converted to a dummy UWP exception.*/ \
							::Platform::Exception^ pException = ::Platform::Exception::CreateException(E_FAIL, ref new ::Platform::String(L"Exception of unknown type encountered."); \
							bdn::winuwp::_storeParametersForUnhandledException(pException, canKeepRunningAfterException); \
							throw pException; \
                        }


#elif BDN_PLATFORM_ANDROID

#include <bdn/java/Env.h>

#define BDN_ENTRY_BEGIN(pJniEnv) \
        bdn::java::Env::get().jniBlockBegun(pEnv); \
        try \
        {


#define BDN_ENTRY_END() \
		} \
		catch(...) \
		{ \
			/* Just let exceptions through to the java side. We have a global uncaught exception handler */ \
			/* installed that will pass any exception that is not handled by the java code to our own unhandledException */ \
			/* function. */ \
			bdn::java::Env::get().setJavaSideException( std::current_exception() ); \
		}


#elif BDN_PLATFORM_IOS

    #include <bdn/fk/exceptionUtil.h>


    #define BDN_ENTRY_BEGIN  \
        try \
        {

    #define BDN_ENTRY_END(canKeepRunningAfterException) \
        } \
        catch(...) \
        { \
            /* For C++ exceptions we want to convert them to NSException objects, so that the objective C runtime can */ \
            /* deal with them as normal. Note that that is OK, because NSExceptions are intended */ \
            /* to be used for programming errors. And an exception reaching an entry point can be considered a programming error.*/ \
            bdn::fk::rethrowAsNSException(); \
        }


#elif BDN_PLATFORM_OSX

    #define BDN_ENTRY_BEGIN  \
        try \
        {

    #define BDN_ENTRY_END(canKeepRunningAfterException) \
        } \
        catch(...) \
        { \
            /* On macOS the global uncaught exception handler is NOT caught for exceptions that happen in the event loop.*/ \
            /* Those exceptions are simply logged and ignored. So we cannot simply convert our exceptions to NSException and */ \
            /* let them through like we do on iOS. Instead we have to manually call our own unhandled exception handler here */ \
            /* and manually terminate.*/ \
            if( ! bdn::unhandledException(canKeepRunningAfterException) ) \
                std::terminate(); \
        }


#else

#define BDN_ENTRY_BEGIN  try{

#define BDN_ENTRY_END(canKeepRunningAfterException) \
                        } \
                        catch(...) \
                        { \
                            if( ! bdn::unhandledException(canKeepRunningAfterException) ) \
                                std::terminate(); \
                        }

#endif





#endif


