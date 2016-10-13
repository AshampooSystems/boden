#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/log.h>

#include <gtk/gtk.h>

namespace bdn
{


static gboolean _callFromMainThreadBase_doCall(gpointer data)
{
    ISimpleCallable* pCallable = static_cast<ISimpleCallable*>(data);

    try
    {
        pCallable->call();
    }
    catch(std::exception& e)
    {
        // log and ignore exceptions
        logError(e, "Exception thrown by CallFromMainThreadBase_::call. Ignoring.");
    }

    try
    {
        pCallable->releaseRef();
    }
    catch(std::exception& e)
    {
        // log and ignore exceptions
        logError(e, "Exception thrown by CallFromMainThreadBase_ destructor. Ignoring.");
    }
    
    // returning FALSE is important here. If we are called from a timer
    // then returning FALSE will delete the timer. Otherwise we would be called
    // periodically, which is not what we want.

    return FALSE;
}


void CallFromMainThreadBase_::dispatchCall()
{
    // inc refcount so that object is kept alive until the call happens.
    addRef();

    gdk_threads_add_idle_full(  G_PRIORITY_DEFAULT,
                                _callFromMainThreadBase_doCall,
                                static_cast<ISimpleCallable*>(this),
                                NULL );
}


void CallFromMainThreadBase_::dispatchCallWhenIdle()
{
    XXX
}

void CallFromMainThreadBase_::dispatchCallWithDelaySeconds(double seconds)
{
    int64_t millis = (int64_t)(seconds*1000);
    
    if(millis<1)
    {
        dispatchCall();
        return;
    }
    else
    {    
        // inc refcount so that object is kept alive until the call happens.
        addRef();

        gdk_threads_add_timeout_full(   G_PRIORITY_DEFAULT,
                                        millis,
                                        _callFromMainThreadBase_doCall,
                                        static_cast<ISimpleCallable*>(this),
                                        NULL );
    }
}


}

