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

    return FALSE;
}


void CallFromMainThreadBase_::dispatch()
{
    // inc refcount so that object is kept alive until the call happens.
    addRef();

    gdk_threads_add_idle_full(  G_PRIORITY_DEFAULT,
                                _callFromMainThreadBase_doCall,
                                static_cast<ISimpleCallable*>(this),
                                NULL );
}


}

