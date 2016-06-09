#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/test.h>
#include <bdn/Thread.h>

#if BDN_PLATFORM_WEB
#include <bdn/Uri.h>
#endif

#include <iostream>

namespace bdn
{
    
	
int _commandLineAppMain(	std::function< int(const AppLaunchInfo& launchInfo) > appFunc,
							AppControllerBase* pAppController,
							int argCount,
							char* argv[] )
{
#ifdef BDN_PLATFORM_WINDOWS_CLASSIC
	// we are DPI aware
	::SetProcessDPIAware();
#endif

	Thread::_setMainId( Thread::getCurrentId() );

    AppControllerBase::_set(pAppController);

	int result=0;
        
    try
    {
		AppLaunchInfo launchInfo;

		std::vector<String> args;

#if BDN_PLATFORM_WEB
		// arguments are URL-escaped
		for(int i=0; i<argCount; i++)
			args.push_back( Uri::unescape( String(argv[i]) ) );
#else
		for(int i=0; i<argCount; i++)
			args.push_back( String::fromLocaleEncoding(argv[i]) );
#endif
		if(argCount==0)
			args.push_back("");	// always add the first entry.

		launchInfo.setArguments(args);
				
        pAppController->beginLaunch(launchInfo);
        pAppController->finishLaunch(launchInfo);			

		result = appFunc(launchInfo);
    }
    catch(std::exception& e)
    {
		std::cerr << e.what() << std::endl;

        // we will exit abnormally. Still call onTerminate.
        pAppController->onTerminate();
        
        // let error through.
        throw;
    }
    
    pAppController->onTerminate();
    
    return result;
}


int _commandLineTestAppFunc( const AppLaunchInfo& launchInfo )
{
	std::vector<const char*> args;

	for(const String& s: launchInfo.getArguments() )
		args.push_back( s.asUtf8Ptr() );

	return bdn::runTestSession( static_cast<int>( args.size() ), &args[0] );
}


}



