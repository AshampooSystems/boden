#include <bdn/init.h>
#include <bdn/win32/util.h>

#include <bdn/win32/win32Error.h>

namespace bdn
{
namespace win32
{


/** Parses a wide character commandline string (as returned by the win32 API function
	GetCommandLineW.
	Returns an array (std::vector) with the individual arguments. The first element in the array
	will be the executable name that was included in the commandline.	
	*/
std::vector<String> parseWin32CommandLine(const String& commandLine)
{
	int argCount=0;
	wchar_t** argPtrs = ::CommandLineToArgvW(commandLine.asWidePtr(), &argCount);
	if( argPtrs==NULL )
	{
		BDN_WIN32_throwLastError( ErrorFields().add("func", "CommandLineToArgvW")
											.add("context", "parseWin32CommandLine") );
	}

	std::vector<String> args;
	for(int i=0; i<argCount; i++)
		args.push_back( argPtrs[i] );

	::LocalFree(argPtrs);

	return args;
}


AppLaunchInfo makeAppLaunchInfo(int showCommand)
{
	// we ignore the commandline arguments we get from the main function.
	// There are problems with the unicode-encoding with those (e.g. if the code page
	// cannot represent the actual parameters). So we always get the command line
	// from GetCommandLine, which handles Unicode properly
	std::vector<String> args = bdn::win32::parseWin32CommandLine( ::GetCommandLineW() );		
		
	if(args.empty())
		args.push_back("");	// always add the first entry.		

	AppLaunchInfo launchInfo;
	launchInfo.setArguments(args);

	launchInfo.setWindowsShowCommand(showCommand);

	return launchInfo;
}



}
}


