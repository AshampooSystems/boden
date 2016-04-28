#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/sysError.h>

#include <windows.h>


namespace bdn
{


int _uiAppMain(AppControllerBase* pAppController, int showCommand)
{
	AppControllerBase::_set(pAppController);

	try
    {
		// initialize app parameters

		const wchar_t* commandLine = ::GetCommandLineW();

		int argCount=0;
		wchar_t** argPtrs = ::CommandLineToArgvW(commandLine, &argCount);
		if( argPtrs==NULL )
		{
			BDN_throwLastSysError( ErrorFields().add("func", "CommandLineToArgvW")
												.add("context", "commandline parsing at app start") );
		}

		std::vector<String> args;
		for(int i=0; i<argCount; i++)
			args.push_back( argPtrs[i] );
		if(argCount==0)
			args.push_back("");	// always add the first entry.

		::LocalFree(argPtrs);

		pAppController->setArguments(args);

		// initialize launch info
		std::map<String,String> launchInfo;	
		launchInfo["windows.showCommand"] = std::to_string(showCommand);

		pAppController->setLaunchInfo(launchInfo);

		
        pAppController->beginLaunch();
        pAppController->finishLaunch();
    
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



