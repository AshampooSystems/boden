#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/log.h>

#include <emscripten.h>

namespace bdn
{


namespace web
{

class IdleRunner_ : public Base
{
public:

	static IdleRunner_& get();

	void normalPriorityCallQueued()
	{
		_normalPriorityCallsPending++;
	}

	void normalPriorityCallUnqueued()
	{
		_normalPriorityCallsPending--;
	}

	void addCall(ISimpleCallable* pCallable)
	{
		_callList.push_back(pCallable);

		ensureCallbackPending();
	}

private:

	void ensureCallbackPending()
	{
		if(!_callbackPending)
		{
			// schedule a new callback
			emscripten_async_call(&IdleRunner_::callback, static_cast<void*>(this), 0);
			_callbackPending = true;
		}
	}

	void handler()
	{
		_callbackPending = false;

		if(_normalPriorityCallsPending>0)
		{
			// there are normal piority tasks pending. Re-schedule again to wait until they are done.
			ensureCallbackPending();
		}
		else
		{
			// no more normal priority tasks in the queue. We can execute the idle tasks.

			// Make a copy to ensure that tasks that are added after this are not executed.
			std::list< P<ISimpleCallable> > toCall = _callList;
			_callList.clear();

			// we want the callable objects to be released right after they are
			// executed. So we remove them from the list in each iteration
			while( !toCall.empty() )
			{
				P<ISimpleCallable>& pCallable = toCall.front();

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

				toCall.pop_front();
			}

		}
	}

	static void callback(void* pArg)
	{
		((IdleRunner_*)pArg)->handler();
	}


	std::list< P<ISimpleCallable> > _callList;

	bool _callbackPending = false;

	int64_t _normalPriorityCallsPending = 0;
};

BDN_SAFE_STATIC_IMPL( IdleRunner_, IdleRunner_::get );

}
	
static void CallFromMainThread_callbackNormalPriority(void* pArg)
{
	web::IdleRunner_::get().normalPriorityCallUnqueued();

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


void CallFromMainThreadBase_::dispatchCall()
{
	// keep ourselves alive during this
	addRef();

	web::IdleRunner_::get().normalPriorityCallQueued();

	emscripten_async_call(CallFromMainThread_callbackNormalPriority, static_cast<void*>(this), 0);
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

		web::IdleRunner_::get().normalPriorityCallQueued();

		emscripten_async_call(CallFromMainThread_callbackNormalPriority, static_cast<void*>(this), (int)millis);
	}
}


void CallFromMainThreadBase_::dispatchCallWhenIdle()
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
	// behave the same way as asyncCallFromMainThread here.

	// Where it is not safe is for our own internal events. Here the ordering is well defined and
	// we must ensure that newly enqueued async tasks are handled before our idle tasks.

	// So for this we use the IdleRunner helper object.

	web::IdleRunner_::get().addCall( this );
}




}

