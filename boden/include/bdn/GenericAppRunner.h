#ifndef BDN_GenericAppRunner_H_
#define BDN_GenericAppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/GenericDispatcher.h>

namespace bdn
{
    
/** A generic AppRunner implementation. This can be used as-is for apps that
    do not have a platform specific event queue (like commandline apps). It can also
    be used as a base class for some custom app runners.
	*/
class GenericAppRunner : public AppRunnerBase
{
private:
	AppLaunchInfo _makeLaunchInfo(int argCount, char* args[] )
	{
		AppLaunchInfo launchInfo;

		std::vector<String> argStrings;
		for(int i=0; i<argCount; i++)
			argStrings.push_back( String::fromLocaleEncoding(args[i]) );
		launchInfo.setArguments(argStrings);

		return launchInfo;
	}

public:
	GenericAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[] )
		: AppRunnerBase( appControllerCreator, _makeLaunchInfo(argCount, args) )
	{
        _pDispatcher = newObj<GenericDispatcher>();
	}

	GenericAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, const AppLaunchInfo& launchInfo)
		: AppRunnerBase( appControllerCreator, launchInfo )
	{
        _pDispatcher = newObj<GenericDispatcher>();
	}

	void initiateExitIfPossible(int exitCode) override
	{
		MutexLock lock(_exitMutex);

		_exitRequested = true;
		_exitCode = exitCode;
	}

	int entry()
	{
		launch();

		runMainLoop();

		{
			MutexLock lock(_exitMutex);

			return _exitCode;
		}
	}


    P<IDispatcher> getMainDispatcher() override
    {
        return _pDispatcher;
    }

protected:

	
	virtual bool shouldExit() const
	{
		MutexLock lock(_exitMutex);
		return _exitRequested;
	}

	void mainLoop() override
	{
		// commandline apps have no user interface events to handle. So our main loop
		// only needs to handle our own scheduled events.
        
		// just run the app controller iterations until we need to close.
            
		while( !shouldExit() )
		{
            if(!_pDispatcher->executeNext())
            {
                // just wait for the next work item.
                _pDispatcher->waitForNext( 10 );
            }
        }
	}

	

	mutable Mutex _exitMutex;

    P<GenericDispatcher> _pDispatcher;

	bool _exitRequested = false;
	int	 _exitCode = 0;
};
  		

    
    
}

#endif

