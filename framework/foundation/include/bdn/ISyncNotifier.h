#ifndef BDN_ISyncNotifier_H_
#define BDN_ISyncNotifier_H_

#include <bdn/INotifierBase.h>

namespace bdn
{

    /** Interface for objects that manages notifications for arbitrary "events".

        You can subscribe functions and methods to the notifier. When the
       corresponding event happens (i.e. when someone calls the
       postNotification() method), then all subscribed functions and methods are
       called.

        ISyncNotifier implementations call the subscribed functions in a
       synchronous way. I.e. the calls happen immediately when the event
       happens, from the same thread that triggered the event.

        See IAsyncNotifier for an alternative that calls subscribed functions in
       an asynchronous way from the main thread.

        It is possible to pass arguments to notify(), which it will in turn pass
       on to each of the functions it calls. The number and types of these
       arguments are defined by the template parameters of the INotifier class.

        Notifier objects work well with weak method references (see
       weakMethod()). If the method's associated object is deleted then this
       will be detected and the subscribtion will be removed the next time a
       notification happens.

        More detailed information about the implementation of the Notifier class
       and the design decisions can be found here: \ref notifier_internals.md

        Example:

        \code

        // this function should be called when the event happens
        void myFunc(String x, int y)
        {
            ... handle notification
        }

        // The ISyncNotifier instance manages the notifications of certain
       events. These are usually
        // returned by methods that are called onXYZ.

        // Subscribe myFunc to the notifier returned by an imaginary function
       onSomeEvent(). someObj.onSomeEvent() += myFunc;

        // when the event occurs then the owner of the notifier object (in our
       example someObj)
        // calls the "notify" method of the notifier and passes the correct
       parameters that describe the event.
        // This will cause a notification to be posted to the main thread's
       event queue,
        // which will cause the subscribed functions to be called.
        someObj.onSomeEvent().notify("Some text", 42);

        // Since the notifier is synchronous, the subscribed functions have all
       been called at
        // this point.

        \endcode


        If you want to explicitly unsubscribe at some point then you need to use
       the subscribe() function instead of the += operator to subscribe:

        \code

        // The returned INotifierSubscription object
        // can optionally be stored for explicitly unsubscribing later.
        // However, it is often simpler to use weak methods, since those
        // do not need to be unsubscribed (see below)
        P<INotifierSubscription> sub = someObj.onSomeEvent().subscribe( myFunc
       );

        ...

        // When you do not want to receive notifications anymore then you can
       unsubscribe. someObj.onSomeEvent().unsubscribe(sub);

        // This will NOT call myFunc anymore, since unsubscribe was called and
       the
        // subscription has been destroyed.
        someObj.onSomEvent().postNotification("Other text", 17);

        \endcode

        Notifier objects can also use weak methods to avoid having to
       unsubscribe explicitly when an object is destroyed.

        \code

        class MyClass
        {
        public:

            void myCallbackMethod(String param)
            {
                ...
            }
        };

        P<MyClass> object = newObj<MyClass>();

        // subscribe myCallbackMethod to the event
        // as a weak method.
        someObj.onSomeEvent() += weakMethod(object,
       &MyClass::myCallbackMethod);

        // weak methods do not keep the method's object alive. So the object
        // can be destroyed at any time.

        // This will destroy the object that object points to.
        object = nullptr;

        // What happens now when notify is called? Some might expect a crash
       here,
        // since it still has a method of a destroyed object in its internal
       list.
        // However, nothing bad will happen when we call the notifier.
        // It is safe to call weak methods even after their object was
       destroyed.
        // All that happens is that a DanglingFunctionError exception is
       generated.
        // And the notifier will automatically handle that and simply remove the
       method
        // from its list. So calling notify has no bad effects here and will
       simply
        // cause the expired method to be removed from the notifier.
        someObj.onSomeEvent().notify("hello");

        \endcode

    */
    template <class... ARG_TYPES> class ISyncNotifier : BDN_IMPLEMENTS INotifierBase<ARG_TYPES...>
    {
      public:
        /** Calls all subscribed functions immediately with the specified
           arguments. All subscribed functions are called before notify returns.
        */
        virtual void notify(ARG_TYPES... args) = 0;
    };
}

#endif
