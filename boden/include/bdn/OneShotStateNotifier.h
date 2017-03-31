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

    - Any functions that are subscribed after the single notify call happened will be called immediately, with the parameters
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
class OneShotStateNotifier : public RequireNewAlloc<Base, OneShotStateNotifier<ArgTypes...> >
    , BDN_IMPLEMENTS INotifier<ArgTypes...>
{
public:    
    OneShotStateNotifier()
    {
    }
        
    P<INotifierSubControl> subscribe(const std::function<void(ArgTypes...)>& func) override
    {
        int64_t subId = subscribeInternal(func);

        return newObj<SubControl_>(this, subId);        
    }
    

    INotifier<ArgTypes...>& operator+=(const std::function<void(ArgTypes...)>& func) override
    {
        subscribeInternal(func);

        return *this;
    }
    

    P<INotifierSubControl> subscribeParamless(const std::function<void()>& func) override
    {
        return subscribe( ParamlessFunctionAdapter(func) );
    }
    
        
    
    void postNotification(ArgTypes... args) override
    {
        MutexLock lock(_mutex);

        if(_postNotificationCalled)
        {
            // should not happen
            programmingError("OneShotStateNotifier::postNotification was called multiple times. It should only be called once.");
        }

        _postNotificationCalled = true;
                
        // bind the arguments to our static function callFuncWithParams, so that we can call newly
        // added functions when they subscribe
        _subscribedFuncCaller = std::bind(&OneShotStateNotifier::callFuncWithParams, std::placeholders::_1, args... );
        
        scheduleNotifyCall();
    }

   
    void unsubscribeAll() override
    {
        MutexLock lock(_mutex);

        _subMap.clear();

        if(_pNotificationState!=nullptr)
            _pNotificationState->nextItemIt = _subMap.end();
    }

    
private:
    static void callFuncWithParams( const std::function<void(ArgTypes...)>& func, ArgTypes... args )
    {
        func(args...);
    }

    int64_t subscribeInternal(const std::function<void(ArgTypes...)>& func)
    {
        MutexLock lock(_mutex);

        // always add the subscriber to the map. If postNotification has already been called
        // then it is still correct (see below)
        
        int64_t subId = _nextSubId;
        _nextSubId++;
            
        _subMap[subId] = Sub_(func);
                  
        if(_postNotificationCalled)
        {
            if(_notificationPending)
            {
                // note that if _postNotificationCalled is true and _notificationPending is true
                // then that means that the doNotificationFromMainThread has either not been called
                // yet or that it is currently still executing. In both cases, newly added items
                // in the map will still be picked up.
                // Note that the map key is an increasing integer and that map iterators always
                // iterate sorted in ascending order. So newly added map items are always added at
                // the end and will still be picked up by the loop if it is still running.

                // the only special case here is if the loop has already reached the last item
                // in the map. In that case the "next item" iterator will point to the end and
                // a newly added item will not be picked up automatically.
            
                // We handle this case here.

                if(_pNotificationState!=nullptr && _pNotificationState->nextItemIt==_subMap.end())
                {
                    // set the next item to the newly added one.
                    _pNotificationState->nextItemIt = _subMap.find(subId);
                }
            }
            else
            {
                // postNotification has been called once. So we are now in "auto-notify" mode
                // and want to notify new subscribers immediately. _notificationPending is false,
                // so no notification is currently pending.
                // That means that the last notification has been fully completed and the subMap
                // is now empty.
                // We now simply schedule a new notification and add the new subscriber to our
                // map afterwards. 

                scheduleNotifyCall();
            }
        }
        
        return subId;
    }

    void scheduleNotifyCall()
    {
        asyncCallFromMainThread( strongMethod(this, &OneShotStateNotifier::doNotifyFromMainThread) );
        _notificationPending = true;
    }
            
    struct Sub_
    {
        Sub_()
        {
        }

        Sub_(const std::function<void(ArgTypes...)>& func)
            : func(func)
        {
        }

        std::function<void(ArgTypes...)> func;
    };

    
    struct NotificationState
    {
        typename std::map<int64_t, Sub_ >::iterator   nextItemIt;        
    };
    

    void unsubscribe(int64_t subId)
    {
        MutexLock lock(_mutex);

        auto it = _subMap.find(subId);
        if(it!=_subMap.end())
        {
            // for us there can only be one notification running because we only do one notification at all.
            if(_pNotificationState!=nullptr && _pNotificationState->nextItemIt==it)
                _pNotificationState->nextItemIt++;

            _subMap.erase(it);
        }
    }

        
    void doNotifyFromMainThread()
    {
        MutexLock lock(_mutex);
        
        // We also need to ensure that subscribers that are removed while our notification call
        // is running are not called after they are removed (unless their specific call has already started).
        
        // There is usually only one notification call active at any given point in time, since all of them
        // are made from the main thread.
        // However, if the event loop does work somewhere in an inner function (e.g. by a modal dialog
        // created by some other framework) then a second notification call might be executed
        // before the first can finish. The mutex does not protect against this since both calls
        // would happen in the same thread (the main thread).

        NotificationState state;

        state.nextItemIt = _subMap.begin();

        _pNotificationState = &state;
                
        try
        {
            while(state.nextItemIt != _subMap.end())
            {
                std::pair< int64_t, Sub_ > item = *state.nextItemIt;

                // increase the iterator before we call the notification function. That is necessary
                // for cases when someone removes the next item in our map before we get to it.
                // Note that the unsubscribe function will update the iterator properly, to ensure that
                // it remains valid.
                state.nextItemIt++;

                // now we have to release the mutex. At this point in time the function might be unsubscribed
                // by another thread, but we cannot stop the call once we started it. We also cannot block
                // unsubscribes during this call, since that can open up the potential for deadlocks.
                // So the caller of unsubscribe has to deal with the fact that the function might be called
                // once more directly after unsubscribe finishes.

                try
                {                
                    MutexUnlock unlock(_mutex);

                    // note that _subscribedFuncCaller has the notification parameters bound to it.
                    // So we do not need to pass them here.
                    _subscribedFuncCaller( item.second.func );
                }
                catch(DanglingFunctionError&)
                {
                    // this is a perfectly normal case. It means that the target function
                    // was a weak reference and the target object has been destroyed.
                    // Just remove it from our list and ignore the exception.

                    unsubscribe(item.first);
                }
            }

            // remove all items in our map now
            _subMap.clear();

            // at this point items that are added to the map are not picked up anymore.
            // So we set _notificationPending to false.
            _notificationPending = false;
            _pNotificationState = nullptr;        

        }
        catch(...)
        {
            _pNotificationState = nullptr;

            _notificationPending = false;

            throw;
        }
    }

    class ParamlessFunctionAdapter
    {
    public:
        ParamlessFunctionAdapter( std::function<void()> func)
        {
            _func = func;
        }
        
        void operator()(ArgTypes... args)
        {
            _func();
        }
        
    protected:
        std::function<void()> _func;
    };


    class SubControl_ : public Base, BDN_IMPLEMENTS INotifierSubControl
    {
    public:		
        SubControl_(DefaultNotifier* pParent, int64_t subId)
            : _pParentWeak( pParent )
            , _subId(subId)
        {
        }
        
        void unsubscribe() override
        {              
            P<DefaultNotifier> pParent = _pParentWeak.toStrong();
            if(pParent!=nullptr)
                pParent->unsubscribe(_subId);
        }


    private:
        WeakP<DefaultNotifier> 	_pParentWeak;
        int64_t                 _subId;
        
        friend class DefaultNotifier;
    };
    friend class SubControl_;




    
    Mutex                               _mutex;
    int64_t                             _nextSubId = 1;
    std::map<int64_t,  Sub_>            _subMap;
    bool                                _notificationPending = false;
    NotificationState*                  _pFirstNotificationState = nullptr;
};



    OneShotStateNotifier()
    {            
    }

    P<INotifierSubControl> subscribe( const std::function<void(ArgTypes...)>& func) override
    {
        MutexLock lock( Notifier<ArgTypes...>::getMutex());

        // call immediately if we are already done.
        if(_notified)
        {
            _caller(func);

            // return a dummy control object.
            return newObj<DummySubControl>();
        }
        else
            return Notifier<ArgTypes...>::subscribe(func);

    }

    void postNotification(ArgTypes... args) override
    {
        MutexLock lock(Notifier<ArgTypes...>::getMutex());

        if(_notified)
        {
            // should not happen
            programmingError("OneShotStateNotifier::postNotification was called multiple times. It should only be called once.");
        }

        _notified = true;

        // bind the arguments to our static function callFunc, so that we can call newly
        // added functions when they subscribe
        _caller = std::bind(&OneShotStateNotifier::callFunc, std::placeholders::_1, args... );

        Notifier<ArgTypes...>::postNotification(args...);
        XXX cannot unsub here

        
    }


    /** Returns true if notify() has already been called.*/
    bool didNotify()
    {
        MutexLock lock(Notifier<ArgTypes...>::getMutex());

        return _notified;
    }

protected:
    void notificationFinished(ArgTypes... args) override
    {
        MutexLock lock(getMutex());

        // unsubscribe all. Our notifications only happen once, so we can release the notification functions.
        Notifier<ArgTypes...>::unsubscribeAll();


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


