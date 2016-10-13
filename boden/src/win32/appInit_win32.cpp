#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/win32/win32Error.h>
#include <bdn/Thread.h>
#include <bdn/ISimpleCallable.h>

#include <bdn/win32/util.h>
#include <bdn/win32/GlobalMessageWindow.h>

#include <windows.h>

#include <ShellScalingApi.h>


namespace bdn
{


int _uiAppMain(AppControllerBase* pAppController, int showCommand)
{
	_mainInit();

	AppControllerBase::_set(pAppController);

	try
    {
		AppLaunchInfo launchInfo;

		// commandline arguments

		std::vector<String> args = bdn::win32::parseWin32CommandLine( ::GetCommandLineW() );

		if(args.empty())
			args.push_back("");	// always add the first entry.
		
		launchInfo.setArguments(args);


		// windows show command
		launchInfo.setWindowsShowCommand(showCommand);

				
        pAppController->beginLaunch(launchInfo);
        pAppController->finishLaunch(launchInfo);

        std::list< ISimpleCallable* > idleCallables;
    
        MSG msg;
        while(true)
        {
            // find out if there are any messages left in the queue.
            // If so then we call GetMessage as normal to process them.
            // If not then we execute our idle handlers.

            // Note that we use PM_NOREMOVE here. It might be tempting to
            // use PM_REMOVE and immediately process the message, but the documentation
            // of PeekMessage is not clear if that is exactly equivalent to calling
            // GetMessage. In fact it suggests that there are differences, at least for
            // WM_PAINT messages. So, to avoid hard-to-find problems we use PeekMessage
            // and then call GetMessage again for the same message. The additional overhead
            // is unlikely to be noticeable.
            if( ! ::PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE) )
            {
                // no messages pending. Execute pending idle handlers.
                bdn::win32::GlobalMessageWindow::get().notifyIdleBegun();
            }

            // whether or not there are messages in the queue, we always call GetMessage here.
            // This will block when there are no messages pending, which is exactly what we want.
            // Note that when new idle handlers are added to the IdleManager then it will post a dummy
            // message to unblock us here. That will cause a new idle phase to begin once the message
            // queue is emptied again.
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

        // execute any pending idle handlers, as they might hold references to
        // objects that should be destroyed before the teardown of static objects begins
        bdn::win32::GlobalMessageWindow::get().notifyIdleBegun();
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



