#ifndef BDN_Dispatcher_H_
#define BDN_Dispatcher_H_

#include <bdn/ISimpleCallable.h>

#include <future>

namespace bdn
{

class CallFromMainThreadBase_ : public Base, BDN_IMPLEMENTS ISimpleCallable
{
public:
	void dispatch();

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

	virtual void call() override
	{
		_packagedTask();
	}

protected:
	std::packaged_task< typename std::result_of<FuncType(Args...)>::type() > _packagedTask;
};


/** Causes the specified function to be called from the main thread. The main thread is the
	thread that runs the user interface and the event loop.

	If callFromMainThread is called from the main thread then the function \c func is executed immediately.
	If you do not want that consider using asyncCallFromMainThread() instead.

	If callFromMainThread is called from another (non-main) thread then the actual call happens
	asynchronously. If you want to wait until the function finishes in the main thread, store the returned
	std::future object and call std::future::get or one of its wait functions.
	If you do not want to wait until the function finishes then you should simply not store the returned
	future object.

	When \c func throws an exception then std::future::get will throw that exception when you call it.		
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
		pCall->dispatch();

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

	pCall->dispatch();
}


/** Wraps a function (called the "inner function") into a wrapper function. When the returned wrapper function
	is called it causes the inner function to be executed in the main thread (see also callFromMainThread() ).

	The generated wrapper function takes the parameters specified in the template parameters Args and passes
	those on to the inner function when the inner function is called from the main thread.

	When the wrapper is called, it does NOT automatically wait for the inner function to
	complete in the main thread. It merely schedules it to be called and then returns a std::future object that
	can be used to wait for the execution to end, if desired. See the documentation of callFromMainThread() for
	an explanation of how the future object is used.

	Note that there are two variations of this function that are often simpler to use than wrapCallFromMainThread:
	wrapCallFromMainThreadAsync() and wrapCallFromMainThreadSync().

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

	auto wrapped = wrapCallFromMainThread<decltype(myFunc), String, double>(myFunc);

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
template <class InnerFuncType, class... Args>
std::function< std::future<typename std::result_of<InnerFuncType(Args...)>::type>(Args...) > wrapCallFromMainThread(InnerFuncType&& innerFunc)
{
	return	[innerFunc](Args... args)
			{
				return callFromMainThread( innerFunc, args... );	
			};
}


/** Wraps a function (called the "inner function") into a wrapper function. When the returned wrapper function
	is called it causes the inner function to be executed in the main thread (see also callFromMainThread() ).

	The generated wrapper function takes the parameters specified in the template parameters Args and passes
	those on to the inner function when the inner function is called from the main thread.

	The wrapper does not wait for the inner function to complete and it does not return the result of the function.
	The return type of the wrapper is void.

	Also see wrapCallFromMainThreadSync() and wrapCallFromMainThread().


	Example:

	\code

	int myFunc()
	{
		... do something.
	}

	auto wrapped = wrapCallFromMainThreadAsync(myFunc);

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

	auto wrapped = wrapCallFromMainThreadAsync<decltype(myFunc), String, double>(myFunc);

	// The following will schedule myFunc to be executed in the main thread. We will not wait for
	// it to finish and we cannot access its return value.
	wrapped("hello", 42.5 );

	// myFunc will now execute at some point in the main thread.

	\endcode
	
	*/
template <class InnerFuncType, class... Args>
std::function< void(Args...) > wrapCallFromMainThreadAsync(InnerFuncType&& innerFunc)
{
	return	[innerFunc](Args... args)
			{
				callFromMainThread( innerFunc, args... );	
			};
}



/** Wraps a function (called the "inner function") into a wrapper function. When the returned wrapper function
	is called it causes the inner function to be executed in the main thread (see also callFromMainThread() ).

	The generated wrapper function takes the parameters specified in the template parameters Args and passes
	those on to the inner function when the inner function is called from the main thread.

	When the wrapper is called, it schedules the inner function to be called from the main thread and waits
	for it to complete. The wrapper returns the return value of the inner function.

	Example:

	\code

	int myFunc()
	{
		... do something.
	}

	auto wrapped = wrapCallFromMainThreadSync(myFunc);

	// The following will cause myFunc to be executed in the main thread. The wrapper
	// automatically waits for it to return and returns its result.
	// If myFunc throws and exception then the wrapper will throw that exception
	// when we call it.
	int result = wrapped();

	\endcode



	Example with an inner function that takes parameters:

	\code

	int myFunc(String a, double b)
	{
		... do something.
	}

	auto wrapped = wrapCallFromMainThreadSync<decltype(myFunc), String, double>(myFunc);

	// The following will cause myFunc to be executed in the main thread. The wrapper
	// automatically waits for it to return and returns its result.
	// If myFunc throws and exception then the wrapper will throw that exception
	// when we call it.
	int result = wrapped("hello", 42.5 );

	\endcode
	*/
template <class InnerFuncType, class... Args>
std::function< typename std::result_of<InnerFuncType(Args...)>::type (Args...) > wrapCallFromMainThreadSync(InnerFuncType&& innerFunc)
{
	return	[innerFunc](Args... args)
			{
				return callFromMainThread( innerFunc, args... ).get();	
			};
}



    
}


#endif