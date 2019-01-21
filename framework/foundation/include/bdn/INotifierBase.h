#pragma once

#include <bdn/Base.h>

#include <bdn/INotifierSubscription.h>

#include <functional>

namespace bdn
{

    /** Base interface for objects that manages notifications for arbitrary
       "events".

        INotifierBase defines the necessary methods to subscribe to a notifier
       and to manage the subscription. See ISyncNotifier and IAsyncNotifier
       (both derived from INotifierBase) for interfaces that also define how to
       trigger the notification calls.

        You can subscribe functions and methods to the notifier. When the
       corresponding event happens, then all subscribed functions and methods
       are called.

        Notifier objects work well with weak method references (see
       weakMethod()). If the method's associated object is deleted then this
       will be detected and the subscribtion will be removed the next time a
       notification happens.



    */
    template <class... ArgTypes> class INotifierBase : public Base
    {
      public:
        /** Subscribes a function to the notifier. While subscribed, the
           function will be called whenever the notifier's postNotification()
           function is called.

            The parameter list of \c func must match the template parameters of
           the notifier object. Note that for convenience there is also a
           variant called subscribeParamless() that subscribes a function
            without parameters. You can use that if your notification function
           does not care about the event parameters.

            The returned INotifierSubscription object can optionally be used to
           remove the subscription later, by passing it to unsubscribe()..
           However, in many cases this is not necessary and the control object
           is not needed.

            For example, you can subscribe a weak method (see weakMethod()). If
           the object that the method belongs to is deleted then the
           subscription will automatically and silently be deleted the next time
           a notification happens. This is a fully supported use case and
           completely safe.

            If you subscribe a strong method (see strongMethod()) then the
           object that is associated with the method is kept alive by the
           Notifier. So the method and its object will always be valid.

            For other types of functions you might need to explicitly
           unsubscribe if the function is not needed anymore or the resources it
           accesses become invalid.

            */
        virtual std::shared_ptr<INotifierSubscription> subscribe(const std::function<void(ArgTypes...)> &func) = 0;

        /** Convenience function to subscribe functions that do not take any
           parameters to the notifier. Sometimes the notification parameters are
           irrelevant for an event listener and in that case it is often simpler
           to subscribe a simple void function or method without parameters,
           using subscribeParamless.

            Apart from the function parameters, subscribeParamless works exactly
           the same as subscribe().
            */
        virtual std::shared_ptr<INotifierSubscription> subscribeParamless(const std::function<void()> &func) = 0;

        /** Same as subscribe(). Returns a reference to the notifier object.*/
        virtual INotifierBase &operator+=(const std::function<void(ArgTypes...)> &func) = 0;

        /** Unsubscribes a subscribed function. The INotifierSubscription object
           is invalidated by this operation and should not be used again.

            Note that there is also another way to unsubscribe from a notifier.
           When the notifier fires and one of the subscribed functions throws a
           DanglingFunctionError exception then that function is automatically
           unsubscribed and the exception is otherwise ignored. This is the
           mechanism that causes weak methods to be unsubscribed automatically
           after their object has been deleted.

         */
        virtual void unsubscribe(std::shared_ptr<INotifierSubscription> sub) = 0;

        /** Unsubscribes all currently subscribed functions.

            If this is called from the main thread then none of the unsubscribed
           functions will be called after unsubscribeAll returns.

            If this is called from some other thread and a notification is
           already in progress then it can happen that one of the unsubscribed
           functions is called even after unsubscribeAll has returned.
        */
        virtual void unsubscribeAll() = 0;
    };
}
