#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/Thread.h>

#include <gtk/gtk.h>

namespace bdn
{

int _uiAppExitCode = 0;


int _uiAppMain( AppControllerBase* pAppController,
                int argCount,
                char* argv[] )
{
    Thread::_setMainId( Thread::getCurrentId() );

    AppControllerBase::_set(pAppController);

    try
    {
        gtk_init(&argCount, &argv);

		AppLaunchInfo launchInfo;

		std::vector<String> args;

		for(int i=0; i<argCount; i++)
			args.push_back( String::fromLocaleEncoding(argv[i]) );
		if(argCount==0)
			args.push_back("");	// always add the first entry.

		launchInfo.setArguments(args);


        pAppController->beginLaunch( launchInfo );
        pAppController->finishLaunch( launchInfo );

        gtk_main();
    }
    catch(std::exception& e)
    {
        // we will exit abnormally. Still call onTerminate.
        pAppController->onTerminate();

        // let error through.
        throw;
    }

    pAppController->onTerminate();

    return _uiAppExitCode;
}


}



