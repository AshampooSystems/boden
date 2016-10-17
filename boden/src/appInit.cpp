#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/test.h>
#include <bdn/Thread.h>

#if BDN_PLATFORM_WEBEMS
#include <bdn/Uri.h>
#endif

#if BDN_PLATFORM_WIN32
#include <bdn/win32/util.h>
#endif


#if BDN_PLATFORM_WINUWP
#include <bdn/winuwp/platformError.h>
#include <Shellapi.h>
#endif

#include <iostream>

#if BDN_PLATFORM_WIN32
#include <ShellScalingAPI.h>
#endif



namespace bdn
{


void _mainInit()
{
	// ensure that the global mutex for safeinit is allocated.
	SafeInitBase::_ensureReady();

	Thread::_setMainId( Thread::getCurrentId() );	

#if BDN_PLATFORM_WIN32
	// we are DPI aware
	::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	// the older function would be ::SetProcessDPIAware()
#endif


}

std::vector<String> _askForCommandlineParameters()
{
    std::vector<String> args;

    std::cout << "Please enter commandline parameters and press enter.\nLeave empty and press enter to run with no parameters." << std::endl;

	std::string input;
	std::getline(std::cin, input );

	String params = String::fromLocaleEncoding(input);

	// add an empty entry for the executable name first. We do not know it.
	args.push_back("");

	while(!params.isEmpty())
	{
		char32_t chr = params.front();
		
		if(chr==' ')
			params.erase(params.begin() );
		else if(chr=='\"')
		{
			params.erase(params.begin());
			args.push_back( params.splitOffToken("\"") );
		}
		else
			args.push_back( params.splitOffToken(" \t") );
	}
	
    return args;
}
    

#if BDN_PLATFORM_WINUWP

int _commandLineAppMain(	std::function< int(const AppLaunchInfo& launchInfo) > appFunc,
						AppControllerBase* pAppController,
						Platform::Array<Platform::String^>^ argsArray )
{
    BDN_WINUWP_TO_PLATFORMEXC_BEGIN

#else
int _commandLineAppMain(	std::function< int(const AppLaunchInfo& launchInfo) > appFunc,
							AppControllerBase* pAppController,
							int argCount,
							char* argv[] )
{

#endif

	_mainInit();

	AppControllerBase::_set(pAppController);

	int result=0;
        
    try
    {
		AppLaunchInfo launchInfo;

		std::vector<String> args;

#if BDN_PLATFORM_WINUWP
		for(auto s: argsArray)
			args.push_back(s->Data() );

		// WinRT apps do not support commandline arguments (at least not at the time of this writing).
		// So we let the user enter them programmatically.
		// Note that this is pretty hackish. We should probably produce "real" UI apps with an integrated
		// commandline instead. But for the moment this works.
		if(args.empty())
            args = _askForCommandlineParameters();
		
#elif BDN_PLATFORM_WIN32
		args = bdn::win32::parseWin32CommandLine( ::GetCommandLineW() );		

#elif BDN_PLATFORM_WEBEMS
		// arguments are URL-escaped
		for(int i=0; i<argCount; i++)
			args.push_back( Uri::unescape( String(argv[i]) ) );

#else
		for(int i=0; i<argCount; i++)
			args.push_back( String::fromLocaleEncoding(argv[i]) );
#endif

		if(args.empty())
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
			
#if BDN_PLATFORM_WINUWP

	// we must not exit. Otherwise we will get an error message, stating that the app did not start.
	// Also, even if we had a way to end without the error message: if we did that then the user would
	// not be able to see the final text output of the app (since it would disappear immediately).
	// So ending with a loop seems to be the right thing to do.
	std::cout << "\nProgram ended (exit code " << result << ")\n" << std::endl;
	while(true)
		Thread::sleepSeconds(1);

    return result;

    BDN_WINUWP_TO_PLATFORMEXC_END

#else
    
    return result;

#endif
}



int _commandLineTestAppFunc( const AppLaunchInfo& launchInfo )
{
	std::vector<const char*> args;

	for(const String& s: launchInfo.getArguments() )
		args.push_back( s.asUtf8Ptr() );

	return bdn::runTestSession( static_cast<int>( args.size() ), &args[0] );
}


}



