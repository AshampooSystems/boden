#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/win32/GlobalMessageWindow.h>

namespace bdn
{
	
    /*
void CallFromMainThreadBase_::dispatchCall()
{
	win32::GlobalMessageWindow::get().postCall(this);
}

void CallFromMainThreadBase_::dispatchCallWhenIdle()
{
    win32::GlobalMessageWindow::get().callOnceWhenIdle(this);
}

void CallFromMainThreadBase_::dispatchCallWithDelaySeconds(double seconds)
{
    int64_t millis = (int64_t)(seconds*1000);

    if(millis<=0)
        dispatchCall();
    else
    {
        // we cannot use SetTimer, because WM_TIMER message have a low priority.
        // I.e. they will only be posted when the message queue is empty.
        // That is not what we want for callFromMainThread.
        // So instead we start a thread, sleep and then dispatch the message from there.

        P<CallFromMainThreadBase_> pThis = this;

        Thread::exec(
            [pThis, millis]()
            {
                Thread::sleepMillis(millis);
                pThis->dispatchCall();            
            } );
    }    
}
*/
}

