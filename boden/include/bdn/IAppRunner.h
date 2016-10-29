#ifndef BDN_IAppRunner_H_
#define BDN_IAppRunner_H_

#include <bdn/AppLaunchInfo.h>

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


	/** Returns true if the app is a commandline app.

		For most desktop platforms being a "commandline app" means that the app is either
		run in a text-mode commandline windo (Terminal), or is executed as a subprocess
		by another app.
		
		Some platforms do not have a concept of a commandline app (like iOS or android).
		In these cases the commandline environment is emulated by the app runner, so that the
		app itself can work the same as on other platforms.	

		*/
	virtual bool isCommandLineApp()=0;

	
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
	virtual const AppLaunchInfo& getLaunchInfo()=0;

};


/** Returns the app's runner object.*/
P<IAppRunner> getAppRunner(); 		


void _setAppRunner(IAppRunner* pAppRunner);

    
    
}

#endif

