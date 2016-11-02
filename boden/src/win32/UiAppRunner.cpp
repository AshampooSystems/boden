#include <bdn/init.h>
#include <bdn/win32/UiAppRunner.h>

#include <bdn/mainThread.h>

#include <bdn/win32/util.h>
#include <bdn/win32/GlobalMessageWindow.h>
#include <bdn/win32/MessageDispatcher.h>

#include <windows.h>

#include <ShellScalingApi.h>


namespace bdn
{
namespace win32
{


UiAppRunner::UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int showCommand)
		: AppRunnerBase(appControllerCreator, makeAppLaunchInfo(showCommand) )
{
    _pMainDispatcher = newObj<MessageDispatcher>();
}

void UiAppRunner::platformSpecificInit()
{
	// tell windows that we are DPI-aware
	::SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
}

int UiAppRunner::entry() 
{
	launch();
	runMainLoop();

	return _exitCode;
}

void UiAppRunner::initiateExitIfPossible(int exitCode) 
{
	callFromMainThread(
		[exitCode]()
		{
			::PostQuitMessage(exitCode);
		} );
}
	

void UiAppRunner::mainLoop()
{
    MSG msg;
    while(true)
    {

        XXX handle high priority items

        handle normal messages

        handle idle messages

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
		{
			if(msg.message==WM_QUIT)
				_exitCode = msg.wParam;
            break;
		}
            
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
    


}
}



