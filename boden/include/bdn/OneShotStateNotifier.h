#ifndef BDN_OneShotStateNotifier_H_
#define BDN_OneShotStateNotifier_H_

#include <bdn/Notifier.h>

namespace bdn
{


/** A special kind of notifier that has the following properties:

    - the notify function is called at most once. If it is called again then a programmingError()
      is raised.

    - Subscribed functions are removed after the single notify call, so their
      resources are released.

    - Any functions that are subscribed after notify will be called immediately, with the parameters
      of the original notify call. These parameters are copied, unless they are pointers (see below for details).

    This kind of notifier is well suited for "finished" notifications. I.e. notifications
    that indicate that some process is done.
    
    The fact that subscribed functions that are added after notify was already called will
    also be executed is a convenient property for "finished" notifications of background
    processes. It means that there are no race conditions for these events.
    The subscribed functions are all guaranteed to be called, even if the background
    process finishes very quickly, before the subscription is created.

    The name OneShotStateNotifier comes from the two special properties mentioned above.
    The notifier notifies only once ("one shot"). The "state" name component refers to
    the fact that late subscribers are called with the original notify parameters. Calling functions
    later with the original parameters is only correct if the notification applies to a "state"
    that has been reached by some kind of process or object and that state remains until the notifier
    object is deleted or no more subscribers can be added.

    Pointer and reference parameters
    --------------------------------

    The argument types of the notifier (the template parameters) need some special consideration.
    In most cases, the parameters passed to notify are copied so that they can be passed on 
    to late subscribers (subscribers that are added after the initial notify() call).
    This is also true for reference parameters - their value is also copied and the late subscriber
    will get passed a reference to the copied object.
    If you want late subscribers and initial subscribers to get the exact same object instance as a parameter
    then you have to use a pointer as the Notifier template parameter (and as such, the subscribed functions will
    get such a pointer as their parameter).
    
*/
template<class... ArgTypes>
class OneShotStateNotifier : public Notifier<ArgTypes...>
{
public:
    OneShotStateNotifier()
    {            
    }

    P<INotifierSubControl> subscribe( const std::function<void(ArgTypes...)>& func) override
    {
        MutexLock lock(getMutex());

        // call immediately if we are already done.
        if(_notified)
        {
            _caller(func);

            // return a dummy control object.
            return newObj<DummySubControl>();
        }
        else
            return Notifier::subscribe(func);

    }

    void notify(ArgTypes... args) override
    {
        MutexLock lock(getMutex());

        if(_notified)
        {
            // should not happen
            programmingError("OneShotStateNotifier::notify was called multiple times. It should only be called once.");
        }

        _notified = true;

        // bind the arguments to our static function callFunc, so that we can call newly
        // added functions when they subscribe
        _caller = std::bind(&OneShotStateNotifier::callFunc, std::placeholders::_1, args... );

        Notifier::notify(args...);

        // unsubscribe all. Our notifications only happen once, so we can release the notification functions.
        unsubscribeAll();
    }


    /** Returns true if notify() has already been called.*/
    bool didNotify()
    {
        MutexLock lock(getMutex());

        return _notified;
    }

private:
    class DummySubControl : public Base, BDN_IMPLEMENTS INotifierSubControl
    {
    public:
        
        void unsubscribe() override
        {
            // do nothing
        }
    };


    static void callFunc( const std::function<void(ArgTypes...)>& func, ArgTypes... args )
    {
        func(args...);
    }

    bool        _notified = false;
    std::function<void( const std::function<void(ArgTypes...)>& ) > _caller;
};


}


#endif


