#ifndef BDN_mainThread_H_
#define BDN_mainThread_H_

#include <bdn/ISimpleCallable.h>
#include <bdn/Thread.h>
#include <bdn/IDispatcher.h>

#include <future>

namespace bdn
{

class CallFromMainThreadBase_ : public Base, BDN_IMPLEMENTS ISimpleCallable
{
public:
	void dispatchCall()
    {
        getMainDispatcher()->enqueue( Caller(this) );
    }

    void dispatchCallWithDelaySeconds(double seconds)
    {
        getMainDispatcher()->enqueueInSeconds( seconds, Caller(this) );
    }

    void dispatchCallWhenIdle()
    {
        getMainDispatcher()->enqueue( Caller(this), IDispatcher::Priority::idle );
    }

private:
    class Caller
    {
    public:
        Caller(CallFromMainThreadBase_* pCallable)
        {
            _pCallable = pCallable;
        }

        void operator()()
        {
            _pCallable->call();
        }

    private:
        P<CallFromMainThreadBase_> _pCallable;
    };
};

template <class FuncType, class... Args>
class CallFromMainThread_ : public CallFromMainThreadBase_
{
public:
	CallFromMainThread_(FuncType&& func, Args&&... args)
		: _packagedTask( std::bind(std::forward<FuncType>(func), std::forward<Args>(args)...) )
	{
	}

	std::future<typename std::result_of<FuncType(Args...)>::type> getFuture()
	{
		return _packagedTask.get_future();
	}

	void call() override
	{
        try
        {
		    _packagedTask();
        }
        catch(DanglingFunctionError&)
        {
            // we ignore this, as required for dispatcher.
            // This exception means that the function is a weak method
            // and the corresponding object has already been deleted.
        }        
	}

protected:
	std::packaged_task< typename std::result_of<FuncType(Args...)>::type() > _packagedTask;
};


/** Causes the specified function to be called from the main thread. The main thread is the
	thread that runs the user interface and the event loop.

	If callFromMainThread is called from the main thread then the function \c func is executed immediately (synchronously).
	If you do not want that consider using asyncCallFromMainThread() instead.

	If callFromMainThread is called from another (non-main) thread then the actual call happens
	asynchronously. If you want to wait until the function finishes in the main thread then you should store the returned
	std::future object and call std::future::get or one of its wait functions.
	If you do not want to wait until the function finishes then you can simply not store the returned
	future object or ignore it.

	If \c func throws an exception then std::future::get will throw that exception when you call it.
*/
template <class FuncType, class... Args>
std::future<typename std::result_of<FuncType(Args...)>::type> callFromMainThread(FuncType&& func, Args&&... args)
{
	P< CallFromMainThread_<FuncType, Args...> > pCall = newObj< CallFromMainThread_<FuncType, Args...> >(std::forward<FuncType>(func), std::forward<Args>(args)... );

	// we return a future object that will block until the function finishes.
	// So if we are called from the main thread there is a potential for a deadlock here, since
	// no events will be processed while we wait blocking.
	// So if we are on the main thread then we must execute the function immediately.
	if(Thread::isCurrentMain())
		pCall->call();
	else
		pCall->dispatchCall();

	return pCall->getFuture();
}


/** Schedules the specified function to be called from the main thread asynchronously.

	The main thread is the thread that runs the user interface and the event loop.

	The function \c func is ALWAYS called asynchronously, even if asyncCallFromMainThread
	is already called from the main thread. \c func will be run during the normal event
	processing loop on the main thread.

	It is not possible to access the return value of the function \c func. If you need access to
	that, consider using callFromMainThread() instead.
*/
template <class FuncType, class... Args>
void asyncCallFromMainThread(FuncType&& func, Args&&... args)
{
	// always dispatch to the event loop.

	P< CallFromMainThread_<FuncType, Args...> > pCall = newObj< CallFromMainThread_<FuncType, Args...> >(std::forward<FuncType>(func), std::forward<Args>(args)... );

	pCall->dispatchCall();
}



/** Schedules the specified function to be called from the main thread asynchronously
    after all pending UI events and UI work has finished.
    
    If new UI events are enqueued after the idle call was scheduled then those are also
    executed BEFORE the idle call is executed. I.e. the idle call happens when the UI
    work/event queue is empty.

	The main thread is the thread that runs the user interface and the event loop.

	On some platforms it may not be possible to define or detect a true "idle" state for the
	app. On those platforms the framework will provide a "best effort" implementation
	that makes it likely (but not guaranteed) that all pending work happens before the idle call.	
	However, all implementations WILL ensure that events and calls that were scheduled
	from inside the Boden framework (for example with asyncCallFromMainThread) are executed
	in the correct priority order. The "best effort" aspect only
	applies to system events and "under the hood" processing that may not be visible to the app
	or that the app may not have control over.

	Platform note:

	At the time of this writing the only platform with an imperfect "best effort" implementation is the webems target.
	Here it may be that idle calls are executed even though user input events or similar are
	in the event queue.

*/
template <class FuncType, class... Args>
void asyncCallFromMainThreadWhenIdle(FuncType&& func, Args&&... args)
{
	P< CallFromMainThread_<FuncType, Args...> > pCall = newObj< CallFromMainThread_<FuncType, Args...> >(std::forward<FuncType>(func), std::forward<Args>(args)... );

	pCall->dispatchCallWhenIdle();
}


/** Schedules the specified function to be called from the main thread asynchronously
    after a delay of the specified number of seconds. The seconds parameter is a floating
    point number, so you can specify non-integer amounts of seconds (e.g.
    a value 0.2 would cause a delay of 200 milliseconds).

	The main thread is the thread that runs the user interface and the event loop.
*/
template <class FuncType, class... Args>
void asyncCallFromMainThreadAfterSeconds(double seconds, FuncType&& func, Args&&... args)
{
	P< CallFromMainThread_<FuncType, Args...> > pCall = newObj< CallFromMainThread_<FuncType, Args...> >(std::forward<FuncType>(func), std::forward<Args>(args)... );

	pCall->dispatchCallWithDelaySeconds(seconds);
}

/** Wraps a function (called the "inner function") into a wrapper function. When the returned wrapper function
	is called it causes the inner function to be executed in the main thread (as if by calling callFromMainThread() ).
    Note that there is a variation of this function that is often simpler to use. See wrapAsyncCallFromMainThread().

 	The generated wrapper function takes the parameters specified in the template parameters Args and passes
	those on to the inner function when the inner function is called from the main thread.

    The last template is the function type. You can usually leave that out - it is normally deduced automatically.

	If the wrapper is called from the main thread then the wrapped function is executed immediately. The returned
    future object is then immediately in finished state and its value can be accessed directly.

    If the wrapper is called from another thread then it schedules the inner function to be called from the main
    thread. It does NOT automatically wait for the inner function to complete in the main thread.
    It merely schedules it to be called and then returns a std::future object that
	can be used to wait for the execution to end, if desired. See the documentation of callFromMainThread() for
	an explanation of how the future object is used.

    If the inner function throws an exception then std::future::get will throw that exception if you call it.

	Example:

	\code

	int myFunc()
	{
		... do something.
	}

	auto wrapped = wrapCallFromMainThread(myFunc);

	// Just execute myFunc in the main thread. Do not wait for it to finish.
	wrapped();

	// myFunc will now execute at some point in the main thread.

	// --- Alternative way to call ---

	// If we want to instead wait for myFunc to finish in the main thread
	// then we need to store the returned future object.
	std::future<int> resultFuture = wrapped();

	// myFunc will now execute at some point in the main thread.

	// We could wait for it with resultFuture.wait or resultFuture.wait_for, ot resultFuture.wait_until.

	// We can also wait for myFunc to finish and access the result by calling resultFuture.get.
	// Note that if myFunc throws and exception then get() will throw that exception when we call it.
	int result = resultFuture.get();

	... do something with the result.

	\endcode


	Example with an inner function that takes parameters:

	\code

	int myFunc(String a, double b)
	{
		... do something.
	}

	auto wrapped = wrapCallFromMainThread<String, double>(myFunc);

	// Just execute myFunc in the main thread. Do not wait for it to finish.
	wrapped("hello", 42.5 );

	// myFunc will now execute at some point in the main thread.

	// --- Alternative way to call ---

	// If we want to instead wait for myFunc to finish in the main thread
	// then we need to store the returned future object.
	std::future<int> resultFuture = wrapped("World", 1.5);

	// myFunc will now execute at some point in the main thread.

	// We could wait for it with resultFuture.wait or resultFuture.wait_for, ot resultFuture.wait_until.

	// We can also wait for myFunc to finish and access the result by calling resultFuture.get.
	// Note that if myFunc throws and exception then get() will throw that exception when we call it.
	int result = resultFuture.get();

	... do something with the result.

	\endcode
	*/
template <class... Args, class InnerFuncType>
std::function< std::future< typename std::result_of<InnerFuncType(Args...)>::type> (Args...) > wrapCallFromMainThread(InnerFuncType&& innerFunc)
{
    return [innerFunc](Args... args)
    {
        return callFromMainThread(innerFunc, args...);
    };
}


/** Wraps a function (called the "inner function") into a wrapper function. When the returned wrapper function
	is called it causes the inner function to be executed asynchronously in the main thread (see also asyncCallFromMainThread() ).

	The generated wrapper function takes the parameters specified in the template parameters Args and passes
	those on to the inner function when the inner function is called from the main thread.

    The last template is the function type. You can usually leave that out - it is normally deduced automatically.

	The wrapper never waits for the inner function to complete and it does not return the result of the function.
	The return type of the wrapper function is void.

    Even if the wrapper function is called from the main thread, it still schedules the inner function to be executed
    asynchronously at a later time (during the normal event processing). If you do not want this then take a look
    at wrapCallFromMainThread().


	Example:

	\code

	int myFunc()
	{
		... do something.
	}

	auto wrapped = wrapAsyncCallFromMainThread(myFunc);

	// The following will schedule myFunc to be executed in the main thread. We will not wait for
	// it to finish and we cannot access its return value.
	wrapped("hello", 42.5 );

	// myFunc will now execute at some point in the main thread.

	\endcode


	Example with an inner function that takes parameters:

	\code

	int myFunc(String a, double b)
	{
		... do something.
	}

	auto wrapped = wrapAsyncCallFromMainThread<String, double>(myFunc);

	// The following will schedule myFunc to be executed in the main thread. We will not wait for
	// it to finish and we cannot access its return value.
	wrapped("hello", 42.5 );

	// myFunc will now execute at some point in the main thread.

	\endcode

	*/
template <class... Args, class InnerFuncType>
std::function< void(Args...) > wrapAsyncCallFromMainThread(InnerFuncType&& innerFunc)
{
	return	[innerFunc](Args... args)
			{
				asyncCallFromMainThread( innerFunc, args... );
			};
}





}


#endif
