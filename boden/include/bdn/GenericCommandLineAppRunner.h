#ifndef BDN_GenericCommandLineAppRunner_H_
#define BDN_GenericCommandLineAppRunner_H_

#include <bdn/AppRunnerBase.h>
#include <bdn/GenericDispatcher.h>

namespace bdn
{
    
/** Generic AppRunner implementation for commandline apps. Uses standard C++ commandline
	constructs only.
	*/
class GenericCommandLineAppRunner : public AppRunnerBase
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
	GenericCommandLineAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[] )
		: AppRunnerBase( appControllerCreator, _makeLaunchInfo(argCount, args) )
	{
        _pDispatcher = newObj<GenericDispatcher>();
	}

	GenericCommandLineAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, const AppLaunchInfo& launchInfo)
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
        		
		P<AppControllerBase> pAppController = AppControllerBase::get();

		if( ! pAppController->usingHighPerformanceLoop() )
		{
            // if the high performance loop is not used then we treat this the
            // same as if it is suspended. We simply never schedule it to wake up
            _highPerformanceLoopSuspended = true;
        }

		// just run the app controller iterations until we need to close.
            
		while( !shouldExit() )
		{
            // We want to handle all pending dispatcher work items that are ready
            // at this point in time, but not any newly scheduled that
            // might be added during the processing of the ones that are ready.
            // Doing it this way is important because it ensures that the high performance
            // loop will not be starved if there is always something new added to the queue.

            // However, work item priorities make this a little difficult. We cannot
            // simply take the set of items that are currently ready and then execute them.
            // If a higher priority item is added during the execution of the initial batch
            // then that MUST be executed before the remaining items in the initial batch.
            // However, we to execute more work that is added during the initial batch
            // to ensure that the high performance loop is not starved.
            
            // We solve this by determining the NUMBER of items that are currently ready
            // to be executed. Then we execute exactly that number of items.
            // If a higher priority item is added during the operation then it will be executed
            // with the correct ordering and a lower priority item will be pushed out of the execution
            // set.

            int readyCount = _pDispatcher->getReadyCount();
            for(int i=0; i<readyCount; i++)
            {
                if(!_pDispatcher->executeNext())
                    break;
            }

            if(_highPerformanceLoopSuspended)
            {
                // just wait for the next work item. Note that the main loop
                // being unsuspended is also a work item, so we will automatically
                // wake up for that.
                if(readyCount==0)
                    _pDispatcher->waitForNext( 10 );
            }
            else
            {
				double suspendSeconds = pAppController->highPerformanceLoopIteration();
                if(suspendSeconds>0)
                {
                    // suspend the main loop. Then add an item to the dispatcher
                    // to unsuspend it.
                    _highPerformanceLoopSuspended = true;
                    _pDispatcher->enqueueInSeconds(
                        suspendSeconds,
                        [this]()
                        {
                            _highPerformanceLoopSuspended = false;
                        } );
                }
            }
        }
	}

	

	mutable Mutex _exitMutex;

    P<GenericDispatcher> _pDispatcher;

	bool _exitRequested = false;
	int	 _exitCode = 0;

    bool _highPerformanceLoopSuspended = false;
};
  		

    
    
}

#endif

