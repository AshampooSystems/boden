#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/log.h>

#include <emscripten.h>

namespace bdn
{
	

static void CallFromMainThread_callback(void* pArg)
{
	CallFromMainThreadBase_* pObj = (CallFromMainThreadBase_*)pArg;

	try
	{
		pObj->call();
	}
	catch(std::exception& e)
	{
		// log and ignore
		logError(e, "Exception while performing async call in main thread. Ignoring.");
	}
}


void CallFromMainThreadBase_::dispatch()
{
	emscripten_async_call(CallFromMainThread_callback, this, 0);
}




}

