#ifndef BDN_IAsyncNotifier_H_
#define BDN_IAsyncNotifier_H_

#include <bdn/INotifierBase.h>

namespace bdn
{


/** Base interface for objects that manages notifications for arbitrary "events".

	You can subscribe functions and methods to the notifier. When the corresponding event
	happens (i.e. when someone calls the postNotification() method), then all subscribed functions
	and methods are called shortly afterwards.
		
	IAsyncNotifier implementations call the subscribed functions in an asynchronous way. I.e. the
	calls are posted to the application's main even queue and are executed during normal
	event processing from the main thread. This is especially useful for events that come from
	background threads: with asynchronous execution the subscriber does not have to deal with multithreading and
	knows that he is only called from the main thread.

	See ISyncNotifier for an alternative that calls subscribed functions in an synchronous way (immediately).

	It is possible to pass arguments to postNotification(), which it will in turn pass on to each of
	the functions it calls. The number and types of these arguments are defined by the
	template parameters of the INotifier class.

    Notifier objects work well with weak method references (see weakMethod()). If the method's
    associated object is deleted then this will be detected and the subscribtion will be removed
    the next time a notification happens.

	All IAsyncNotifier implementations must be thread safe. In particular, notifications can be triggered from any thread.
    However, the subscribed functions will ONLY be called from the main thread. So subscribers
    usually do not need to be concerned with threading issues.
    
    More detailed information about the implementation of the Notifier class and the design decisions
    can be found here: \ref notifier_internals.md

	Example:

	\code

	// this function should be called when the event happens
	void myFunc(String x, int y)
	{
		... handle notification
	}

	// The IAsyncNotifier instance manages the notifications of certain events. These are usually
    // returned by methods that are called onXYZ.

	// Subscribe myFunc to the notifier returned by an imaginary function onSomeEvent().
	someObj.onSomeEvent() += myFunc;

	// when the event occurs then the owner of the notifier object (in our example someObj)
    // calls the "postNotification" method of the notifier and passes the correct parameters that describe the event.
    // This will cause a notification to be posted to the main thread's event queue,
    // which will cause the subscribed functions to be called.
	someObj.onSomeEvent().postNotification("Some text", 42);

    // note that the notification calls happen delayed, from the main thread's event
    // queue. So they are not necessarily finished here.

    \endcode


    If you want to explicitly unsubscribe at some point then you need to use the subscribe()
    function instead of the += operator to subscribe:

    \code

    // The returned INotifierSubscription object
    // can optionally be stored for explicitly unsubscribing later.
    // However, it is often simpler to use weak methods, since those
    // do not need to be unsubscribed (see below)
	P<INotifierSubscription> pSub = someObj.onSomeEvent().subscribe( myFunc );

    ...
    
	// When you do not want to receive notifications anymore then you can unsubscribe.
    someObj.onSomeEvent().unsubscribe(pSub);

	// This will NOT call myFunc anymore, since unsubscribe was called and the
	// subscription has been destroyed.
	someObj.onSomEvent().postNotification("Other text", 17);
    
	\endcode	

    Notifier objects can also use weak methods to avoid having to unsubscribe explicitly
    when an object is destroyed.

    \code

    class MyClass
    {
    public:

        void myCallbackMethod(String param)
        {
            ...
        }
    };

    P<MyClass> pObject = newObj<MyClass>();

    // subscribe myCallbackMethod to the event
    // as a weak method.
    someObj.onSomeEvent() += weakMethod(pObject, &MyClass::myCallbackMethod);

    // weak methods do not keep the method's object alive. So the object
    // can be destroyed at any time.

    // This will destroy the object that pObject points to.
    pObject = nullptr;

    // What happens now when postNotification is called? Some might expect a crash here,
    // since it still has a method of a destroyed object in its internal list.
    // However, nothing bad will happen when we call the notifier.
    // It is safe to call weak methods even after their object was destroyed.
    // All that happens is that a DanglingFunctionError exception is generated.
    // And the notifier will automatically handle that and simply remove the method
    // from its list. So calling postNotification has no bad effects here and will simply
    // cause the expired method to be removed from the notifier.
    someObj.onSomeEvent().postNotification("hello");

    \endcode

*/
template<class... ArgTypes>
class IAsyncNotifier : BDN_IMPLEMENTS INotifierBase<ArgTypes...>
{
public:

    /** Schedules a call to all subscribed functions with the specified arguments.
        The call does not happen immediately. It is posted to the main thread and happens asynchronously.

        The arguments passed to postNotification are copied, even if they are passed by reference.
        This is important: if you pass a reference to an object then a copy is made and the subscribed functions
        will get a reference to the copied object.

        If you want to pass a specific object to the subscribed functions then you need to pass a pointer to it.
        The objects referred to by pointers are not copied. However, it is the responsibility of the caller to
        postNotification to ensure that the object is still valid at the time when the notification calls are actually made
        from the main thread. A good way to do that is to pass a bdn::P object instead of a plain pointer.

        Note that the notification calls will use the subscriber set at the time when the
        calls are actually made, not the subscriber set at the time when postNotification() was called.
    */
    virtual void postNotification(ArgTypes... args)=0;

};
    
}


#endif


