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
}


void CallFromMainThreadBase_::dispatch()
{
	// keep ourselves alive during this
	addRef();

	emscripten_async_call(CallFromMainThread_callback, static_cast<void*>(this), 0);
}




}

