#ifndef BDN_GenericCommandLineAppRunner_H_
#define BDN_GenericCommandLineAppRunner_H_

#include <bdn/AppRunnerBase.h>

namespace bdn
{
    
/** Generic AppRunner implementation for commandline apps. Uses standard C++ commandline
	constructs only.
	*/
class GenericCommandLineAppRunner : public AppRunnerBase
{
private:
	AppLaunchInfo _makeLaunchInfo(int argCount, const char** args)
	{
		AppLaunchInfo launchInfo;

		std::vector<String> argStrings;
		for(int i=0; i<argCount; i++)
			argStrings.push_back( String::fromLocaleEncoding(args[i]) );
		launchInfo.setArguments(argStrings);

		return launchInfo;
	}

public:
	GenericCommandLineAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, const char** args)
		: GenericCommandLineAppRunner( appControllerCreator, _makeLaunchInfo(argCount, args) )
	{
	}

	GenericCommandLineAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, const AppLaunchInfo& launchInfo)
		: GenericCommandLineAppRunner( appControllerCreator, launchInfo )
	{
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

protected:

	
	virtual bool shouldExit() const
	{
		MutexLock lock(_exitMutex);
		return _exitRequested;
	}

	void mainLoopImpl() override
	{
		// commandline apps have no user interface events to handle. So our main loop
		// only needs to handle our own scheduled events.

		
		P<AppControllerBase> pAppController = AppControllerBase::get();

		if(pAppController->usesMainLoop())
		{
			// just run the app controller iterations until we need to close.
			while( !shouldExit() )
			{
				_pDispatcher->
				pAppController->mainLoopIteration();
		}
		else
		{
			// a commandline controller without a main loop. The app probably
			// does everything it needs to do in beginLaunch and finishLaunch.
			// So do nothing here.
		}
	}

	

	mutable Mutex _exitMutex;

	bool _exitRequested = false;
	int	 _exitCode = 0;
};
  		

    
    
}

#endif

