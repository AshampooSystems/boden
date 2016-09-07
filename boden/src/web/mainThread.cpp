#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/log.h>

#include <emscripten.h>

namespace bdn
{
	

static void CallFromMainThread_callback(void* pArg)
{
	P<CallFromMainThreadBase_> pObj;

	pObj.attachPtr( static_cast<CallFromMainThreadBase_*>(pArg) );

	try
	{
		pObj->call();
	}
	catch(std::exception& e)
	{
		// log and ignore
		logError(e, "Exception while performing async call in main thread. Ignoring.");
	}
	catch(...)
	{
		// log and ignore
		logError("Exception (not derived from std::exception) while performing async call in main thread. Ignoring.");
	}
}


void CallFromMainThreadBase_::dispatch()
{
	// keep ourselves alive during this
	addRef();

	emscripten_async_call(CallFromMainThread_callback, static_cast<void*>(this), 0);
}



void CallFromMainThreadBase_::dispatchWithDelaySeconds(double seconds)
{
	int64_t millis = (int64_t)(seconds*1000);

	if(millis<=0)
		dispatch();
	else
	{
		// keep ourselves alive during this
		addRef();

		emscripten_async_call(CallFromMainThread_callback, static_cast<void*>(this), (int)millis);
	}
}



}

