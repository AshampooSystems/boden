#ifndef BDN_INotifier_H_
#define BDN_INotifier_H_

#include <bdn/init.h>

#include <bdn/INotifierSubControl.h>

#include <functional>

namespace bdn
{


/** Interface for objects that manages notifications for arbitrary "events".

    You can subscribe functions and methods to the notifier. When the corresponding event
	happens (i.e. when someone calls the postNotification() method), then all subscribed functions
	and methods are called.

	It is possible to pass arguments to postNotification(), which it will in turn pass on to each of
	the functions it calls. The number and types of these arguments are defined by the
	template parameters of the INotifier class.

    Notifier objects work well with weak method references (see weakMethod()). If the method's
    associated object is deleted then this will be detected and the subscribtion will be removed
    the next time a notification happens.

	All INotifier implementations must be thread safe. In particular, notifications can be triggered from any thread.
    However, the subscribed functions will ONLY be called from the main thread. So subscribers
    usually do not need to be concerned with threading issues.
    
    More detailed information about the implementation of the Notifier class and the design decisions
    can be found here: \ref notifier_internals

	Example:

	\code

	// this function should be called when the event happens
	void myFunc(String x, int y)
	{
		... handle notification
	}

	// The INotifier instance manages the notifications of certain events. These are usually
    // returned by methods that are called onXYZ.

	// Subscribe myFunc to the notifier returned by an imaginary function onSomeEvent().
    // The returned INotifierSubControl object
    // can optionally be stored for explicitly unsubscribing later.
    // However, it is often simpler to use weak methods, since those
    // do not need to be unsubscribed (see below)
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

    // The returned INotifierSubControl object
    // can optionally be stored for explicitly unsubscribing later.
    // However, it is often simpler to use weak methods, since those
    // do not need to be unsubscribed (see below)
	P<INotifierSubControl> pSubControl = someObj.onSomeEvent().subscribe( myFunc );

    ...
    
	// When you do not want to receive notifications anymore then you can unsubscribe.
    pSubControl->unsubscribe();

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
class INotifier : BDN_IMPLEMENTS IBase
{
public:

    /** Subscribes a function to the notifier. While subscribed, the function will be called
		whenever the notifier's postNotification() function is called.	

		The parameter list of \c func must match the template parameters of the notifier object.
		Note that for convenience there is also a variant called subscribeParamless() that subscribes a function
		without parameters. You can use that if your notification function does not care about the
		event parameters.

        The returned INotifierSubControl object can optionally be used to remove the subscription later. However, in many
        cases this is not necessary and the control object is not needed.

        For example, you can subscribe a weak method (see weakMethod()). If the object that the method belongs to is deleted
        then the subscription will automatically and silently be deleted the next time a notification happens.
        This is a fully supported use case and completely safe.

        If you subscribe a strong method (see strongMethod()) then the object that is associated with the method
        is kept alive by the Notifier. So the method and its object will always be valid.

        For other types of functions you might need to explicitly unsubscribe if the function is not needed anymore
        or the resources it accesses become invalid.
        
        Instead of unsubscribing via the INotifierSubControl object you could also implement your callback function
        in a way so that it throws a DanglingFunctionError exception if you want it to be unsubscribed.
        In that case the subscription will be removed automatically by the notifier. This is actually the mechanism
        that causes weak methods to be unsubscribed automatically after their object has been deleted.
        
        Note that it is perfectly save to use the returned INotifierSubControl object even after the Notifier object that returned it
        has already been deleted. In that case calling INotifierSubControl::unsubscribe() will have
        no effect, since the subscription does not exist anymore. This safety feature allows
        one to subscribe to a notifier and call INotifierSubControl::unsubscrbe at some arbitrary point in time later, without having to keep a
        pointer to the Notifier object or ensuring that the Notifier even still exists.                
		*/
    virtual P<INotifierSubControl> subscribe(const std::function<void(ArgTypes...)>& func)=0;    
   

    /** Same as subscribe(). Returns a reference to the notifier object.*/
    virtual INotifier& operator+=(const std::function<void(ArgTypes...)>& func)=0;


	/** Convenience function. Similar to subscribe(), except that this
		version takes a function without parameters and subscribes it to the event. You can use this
		if your callback function is not interested in the event parameters and only cares about when the event
		itself happens.
		*/
    virtual P<INotifierSubControl> subscribeParamless(const std::function<void()>& func)=0;
    
    

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


    /** Unsubscribes all currently subscribed functions.

        If this is called from the main thread then none of the unsubscribed functions
        will be called after unsubscribeAll returns.

        If this is called from some other thread and a notification is already in
        progress then it can happen that one of the unsubscribed functions is called
        even after unsubscribeAll has returned.
    */
    virtual void unsubscribeAll()=0;
};
    
}


#endif


