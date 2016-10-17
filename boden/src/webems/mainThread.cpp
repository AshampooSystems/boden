#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/log.h>

#include <emscripten.h>

namespace bdn
{


namespace webems
{


/** Handles main thread calls and ensures that they are executed in the correct order.

	Note that we cannot rely on the execution order of events that were scheduled with 
	setTimeout(0). Different JS engines behave in different ways and sometimmes (it seems)
	even a little bit random. So to avoid these issues we implement our own queuing.
	*/
class MainThreadRunner_ : public Base
{
public:

	static MainThreadRunner_& get();


	void addIdle(ISimpleCallable* pCallable)
	{
		// There is no standard way to schedule something "when idle".
		// At the time of this writing, requestIdleCallback is non-standard and only supported
		// by a handful of browsers (Chrome, Opera, Android). So we cannot really use that,
		// otherwise we risk to have vastly different timing dependending on the browser.

		// It is also difficult to define what "idle" means for a web browser.
		// DOM changes take effect immediately, so there is are no queued events for that.
		// The whole rendering is done in a way that is mostly opaque to the app, so we can
		// ignore rendering events.
		// So what might be queued up are basically only user input events.

		// If we use emscripten_async_call then any events that are already in the queue will
		// be processed before our idle handler is called. So the only difference for external
		// events are user input events that come in between now and the time emscripten_async_call
		// calls our function. In a perfect world those SHOULD be handled before our idle callback.
		// But we could argue that those events come at "random" times anyway and the behaviour of the
		// whole system does not depend on exact timing for them.
		// In other words: we could argue that we can safely treat newly enqueued user input events as if they
		// happened just after our idle handler was called, instead of the real occurrence time (between now
		// and the handler call).

		// So as far as external events are concerned, it should be "safe" (although slightly incorrect) to
		// behave the same way as asyncCallFromMainThread here, as long as we ensure the ordering of our OWN scheduled
		// calls.

		_idleList.push_back(pCallable);


		// we immediately schedule the callbacks for all pending events.
		// That might yield better performance than chaining them
		// (i.e. scheduling the next event after the first has been executed ).
		emscripten_async_call(&MainThreadRunner_::callback, static_cast<void*>(this), 0);
	}

	void addNormal(ISimpleCallable* pCallable)
	{
		_normalList.push_back(pCallable);

		// we immediately schedule the callbacks for all pending events.
		// That might yield better performance than chaining them
		// (i.e. scheduling the next event after the first has been executed ).

		emscripten_async_call(&MainThreadRunner_::callback, static_cast<void*>(this), 0);
	}

private:
	void handler()
	{
		// execute a single event to ensure that we do not block for too long.
		P<ISimpleCallable> pCallable;
		if(!_normalList.empty())
		{
			pCallable = _normalList.front();
			_normalList.pop_front();
		}
		else if(!_idleList.empty())
		{
			pCallable = _idleList.front();
			_idleList.pop_front();
		}

		try
		{
			pCallable->call();
		}
		catch(std::exception& e)
		{
			// log and ignore
			logError(e, "Exception while performing idle call in main thread. Ignoring.");
		}
		catch(...)
		{
			// log and ignore
			logError("Exception (not derived from std::exception) while performing idle call in main thread. Ignoring.");
		}

	}

	static void callback(void* pArg)
	{
		((MainThreadRunner_*)pArg)->handler();
	}


	std::list< P<ISimpleCallable> > _idleList;
	std::list< P<ISimpleCallable> > _normalList;
};

BDN_SAFE_STATIC_IMPL( MainThreadRunner_, MainThreadRunner_::get );

}
	
static void CallFromMainThread_timedCallback(void* pArg)
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
		logError(e, "Exception while performing timed async call in main thread. Ignoring.");
	}
	catch(...)
	{
		// log and ignore
		logError("Exception (not derived from std::exception) while performing timed async call in main thread. Ignoring.");
	}
}


void CallFromMainThreadBase_::dispatchCall()
{
	bdn::webems::MainThreadRunner_::get().addNormal(this);
}


void CallFromMainThreadBase_::dispatchCallWithDelaySeconds(double seconds)
{
	int64_t millis = (int64_t)(seconds*1000);

	if(millis<=0)
		dispatchCall();
	else
	{
		// keep ourselves alive during this
		addRef();

		emscripten_async_call(CallFromMainThread_timedCallback, static_cast<void*>(this), (int)millis);
	}
}


void CallFromMainThreadBase_::dispatchCallWhenIdle()
{
	bdn::webems::MainThreadRunner_::get().addIdle(this);	
}




}

