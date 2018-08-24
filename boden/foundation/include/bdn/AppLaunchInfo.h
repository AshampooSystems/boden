#ifndef BDN_AppLaunchInfo_H_
#define BDN_AppLaunchInfo_H_

#include <bdn/Array.h>

namespace bdn
{


/** Contains information for the app launch, like commandline arguments
	etc.*/
class AppLaunchInfo : public Base
{
public:
	AppLaunchInfo()
	{
#if BDN_PLATFORM_WIN32
		_windowsShowCommand = 1; // SW_SHOWNORMAL
#endif
	}


	/** Returns the app's commandline arguments.

		The first item in the argument list is a representation of the program name and/or path.
		You should not depend on this first entry, though. It is highly dependent on the platform
		and on how the app was called. Sometimes the first entry can be a full path. Sometimes it
		is a relative path or just a file name. On some platforms it can also simply be an empty string.

		The remaining items are the parameters passed to the app by the caller.*/
	const Array<String>& getArguments() const
	{
		return _arguments;
	}


	/** Sets the commandline arguments - see getArguments().*/
	void setArguments(const Array<String>& args)
	{
		_arguments = args;
	}



#if BDN_PLATFORM_WIN32

	/** Returns the windows show command.

		This function is only available if the platform is Windows Classic.

		The show command is a hint from the caller as to how the app should start.
		For example, desktop shortcuts that start apps have a parameter to run the
		app maximmized or minimized. This results in the show command to be the corresponing value.
				
		The possible values for the show command are those that can be passed to
		the Windows function ShowWindow. The default value is SW_SHOWNORMAL. You will
		get SW_SHOWMAXIMIZED or SW_SHOWMINIMIZED when the caller wants the app to run
		in maximized / minimized mode.

		The show command is rarely used and many programs ignore it.
	*/
	int getWindowsShowCommand() const
	{
		return _windowsShowCommand;
	}		


	/** Sets the windows show command - see getWindowsShowCommand().
	
		This function is only available if the platform is Windows Classic.
	*/
	void setWindowsShowCommand(int command)
	{
		_windowsShowCommand = command;
	}
#endif
	

protected:
	Array<String> _arguments;

#if BDN_PLATFORM_WIN32
	int _windowsShowCommand;
#endif

};


}



#endif