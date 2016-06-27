#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/sysError.h>
#include <bdn/Thread.h>

#include <windows.h>


namespace bdn
{


int _uiAppMain(AppControllerBase* pAppController, int showCommand)
{
	// we are DPI aware
	::SetProcessDPIAware();

	Thread::_setMainId( Thread::getCurrentId() );

	AppControllerBase::_set(pAppController);

	try
    {
		AppLaunchInfo launchInfo;

		// commandline arguments

		std::vector<String> args = parseWin32CommandLine( ::GetCommandLineW() );

		if(args.empty())
			args.push_back("");	// always add the first entry.
		
		launchInfo.setArguments(args);


		// windows show command
		launchInfo.setWindowsShowCommand(showCommand);

				
        pAppController->beginLaunch(launchInfo);
        pAppController->finishLaunch(launchInfo);
    
        MSG msg;
        while(true)
        {
            int result = ::GetMessage(&msg, NULL, 0, 0);
            if(result==0)   // WM_QUIT received
                break;
            
            if(result==-1)
            {
                // error. Just exit.
                break;
            }
            
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }

    }
    catch(std::exception&)
    {
        // we will exit abnormally. Still call onTerminate.
        pAppController->onTerminate();
        
        // let error through.
        throw;
    }
    
    pAppController->onTerminate();
    
    return 0;
}
    




}



