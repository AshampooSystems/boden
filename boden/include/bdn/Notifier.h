#ifndef BDN_Notifier_H_
#define BDN_Notifier_H_

#include <bdn/init.h>

#include <bdn/INotifierSubControl.h>
#include <bdn/DanglingFunctionError.h>

#include <functional>
#include <list>

namespace bdn
{


/** Manages notifications for arbitrary "events".

	You can subscribe functions and methods to the notifier. When the corresponding event
	happens (i.e. when someone calls the notify() method), then all subscribed functions
	and methods are called.

	It is possible to pass arguments to notify(), which it will in turn pass on to each of
	the functions it calls. The number and types of these arguments are defined by the
	template parameters of the Notifier class.

    Notifier objects work well with weak method references (see weakMethod()). If the method's
    associated object is deleted then this will be detected and the subscribtion will be removed
    the next time a notification happens.

	Notifier objects are thread safe. Notifications can be triggered from any thread.
	However, that also means that subscribed functions need to be aware that they might be called from different threads.
	Note that you can wrap your functions with wrapCallFromMainThread() before you pass them to
	subscribe if you want them to only be called from the main thread.

	Example:

	\code

	// this function should be called when the event happens
	void myFunc(String x, int y)
	{
		... handle notification
	}

	// The Notifier instance manages the notifications
	Notifier<String, int> eventNotifier;

	// Subscribe myFunc to the notifier. The returned INotifierSubControl object
    // can optionally be stored for explicitly unsubscribing later.
    // However, it is often simpler to use weak methods, since those
    // do not need to be unsubscribed (see below)
	P<INotifierSubControl> pControl = eventNotifier.subscribe(myFunc);
    // note that we could also subscribe with the += operator
    // like this: eventNotifier += myFunc

	// when the event occurs then someone needs to call the "notify" method of
	// the notifier and pass the correct parameters that describe the event.
	eventNotifier.notify("Some text", 42);

	// You can unsubscribe using the subscription control object, if you want.
    // Using weak methods may be an easier alternative (see below)
    pControl->unsubscribe();

	// So this will NOT call myFunc anymore, since pSub was set to null and the
	// subscription has been destroyed.
	eventNotifier.notify("Other text", 17);
    
	\endcode	

    Notifier objects can use weak methods to avoid having to unsubscribe explicitly
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

    Notifier<String> eventNotifier;

    // subscribe myCallbackMethod to the event
    // as a weak method.
    eventNotifier += weakMethod(pObject, &MyClass::myCallbackMethod);

    // weak methods do not keep the method's object alive. So the object
    // can be destroyed at any time.

    // This will destroy pObject.
    pObject = nullptr;

    // What happens now when eventNotifier is called? Some might expect a crash here,
    // since it still has a method of a destroyed object in its internal list.
    // However, nothing bad will happen when we call the notifier.
    // It is safe to call weak methods even after their object was destroyed.
    // All that happens is that a DanglingFunctionError exception is generated.
    // And the notifier will automatically handle that and simply remove the method
    // from its list. So calling notify has no bad effects here and will simply
    // cause the expired method to be removed from the notifier.
    eventNotifier.notify("hello");

    \endcode

*/
template<class... ArgTypes>
class Notifier : public Base
{
public:
    Notifier()
    {
        _pMutex = newObj<Mutex>();
    }

    ~Notifier()
    {
        MutexLock lock(_pMutex);

        for(auto item: _subMap)
        {
            SubControl_* pControl = item.second.pControlWeak;
            if(pControl!=nullptr)
                pControl->detachFromParent();
        }
    }      


    /** Subscribes a function to the notifier. While subscribed, the function will be called
		whenever the notifier's notify() function is called.	

		The parameter list of \c func must match the template parameters of the notifier object.
		Note that for convenience there is also a variant called subscribeParamless() that subscribes a function
		without parameters. You can use that if your notification function does not care about the
		event parameters.

        The returned INotifierSubControl object can be used to remove the subscription later. However, in many
        cases this is not necessary and the control object is not needed.

        There are two main reasons to unsubscribe:

        1) The subscribed function or method accesses resources that are being deleted.
        
           For this case you often do not need to unsubscribe explicitly, depending on what kind of function you
           subscribed.

           - If you subscribe a weak method (see weakMethod()) and the object that the method belongs to is deleted
             then the subscription will automatically and silently be deleted the next time a notification happens.
             This is a fully supported use case and completely safe.

           - If you subscribe a strong method (see strongMethod()) then the object that is associated with the method
             is kept alive by the Notifier. So the method will always be valid.

           - For other types of functions you might need to unsubscribe if the function accesses resources
             that become invalid. You could also implement your callback function in a way so that it throws
             DanglingFunctionError if the associated resources are invalid. In that case the subscription will be removed
             automatically, just as with weak methods (see above).

             
        2) You do not want the subscription to happen anymore because of internal application reasons,
           even if all required resources are still there. In this case you will always have to unsubscribe explicitly,
           using the returns INotifierSubControl object.


        Note that it is perfectly save to use the returned INotifierSubControl object even if the Notifier object that returned it
        has already been deleted. In that case calling INotifierSubControl::unsubscribe() will have
        no effect, since the subscription does not exist anymore. This safety feature allows
        one to subscribe to a notifier and call INotifierSubControl::unsubscrbe at some point in time later, without having to keep a
        pointer to the Notifier object or ensuring that the Notifier even still exists.                
		*/
    virtual P<INotifierSubControl> subscribe(const std::function<void(ArgTypes...)>& func)
    {
        MutexLock lock(_pMutex);

        int64_t subId = _nextSubId;
        _nextSubId++;

        P<SubControl_> pControl = newObj<SubControl_>(this, subId);        
        
        _subMap[subId] = Sub_(func, pControl);
        
        return pControl;
    }

     
   

    /** Same as subscribe(). Returns a reference to the notifier object.*/
    virtual Notifier& operator+=(const std::function<void(ArgTypes...)>& func)
    {
        subscribe(func);
        return *this;
    }



	/** Convenience function. Similar to subscribe(), except that this
		version takes a function without parameters and subscribes it to the event. You can use this
		if your callback function is not interested in the event parameters and only cares about when the event
		itself happens.
		*/
    P<INotifierSubControl> subscribeParamless(const std::function<void()>& func)
    {
        return subscribe( ParamlessFunctionAdapter(func) );
    }
    
        
    
    
    

    /** Schedules a call to all subscribed functions with the specified arguments.
        The call does not happen immediately. It is posted to the main thread and happens asynchronously.

        The arguments passed to postNotification are copied, even if they are passed by reference.
        This is important: if you pass a reference to an object then a copy is made and the subscribed functions
        will get a reference to the copied object.
        If you want to pass a specific object to the subscribed functions then you need to pass a pointer to it.
        The objects referred to by pointers are not copied. However, it is the responsibility of the caller to
        postNotification to ensure that the object is still valid at the time when the notification calls are actually made
        from the main thread.

        Note that the notification calls from the main thread will use the subscriber set at the time when the
        calls are actually made, not the subscriber set at the time when postNotification() was called.
    */
    virtual void postNotification(ArgTypes... args)
    {
        // We have three problems here:

        // postNotification can be called from any thread and in any context.
        // In particular, an unknown set of mutexes might be locked at the time of the call.
        // Subscribers to the notification have no good way of knowing what is locked and what isn't.
        // If we were to call subscribers immediately then deadlocks will almost certainly occur
        // in some places in a complex application, unless the subscriber is very careful what he locks.
        
        // Also, if subscribers are called from arbitrary threads then that makes programming with notifications
        // very difficult for novice programmers. It can create hard to find sporadic bugs that are based on timings
        // or event order and stuff like that.

        // A third problem is that we want to make sure that functions removed with unsubscribe are NOT called
        // after unsubscribe returns. That is important because lingering notifications that happen after
        // an unsubscribe can create complex situations and trigger rare sporadic bugs. It is something that can
        // be surprisingly hard to solve. So we do not want any notifications to the unsubscribed function after
        // unsubscribe returns. If a call is in progress then the unsubscribe call should wait until it is complete.
        // That is only possible by having the call thread and the unsubscribe thread hold a mutex. And that in turn makes
        // the deadlock problem described above even worse.

        // We solve all these issues by posting the notification to the main thread. At that point we are certain
        // that no mutexes are locked. And subscribers are also called from a well defined context of the main thread,
        // reducing the complexity in all parts of the program.

        callFromMainThread( std::bind<ArgTypes>( strongMethod(this, &Notifier::doNotifyFromMainThread) , std::forward<ArgTypes>(args)... );
    }

    /** Unsubscribes all currently subscribed functions.*/
    void unsubscribeAll()
    {
        MutexLock lock(_pMutex);
        
        // detach all control objects
        for(auto item: _subMap)
        {
            SubControl_* pControl = item.second.pControlWeak;
            if(pControl!=nullptr)
                pControl->detachFromParent();
        }

        // and then just clear the map
        _subMap.clear();
    }

protected:
    /** Returns the internal mutex of the notifier. This is locked
        when subscriptions are created or removed and when they fire.*/
    Mutex& getMutex() const
    {
        return *_pMutex;
    }

    
private:
    
    void doNotifyFromMainThread(ArgTypes... args)
    {
        // we keep a mutex locked here to ensure that subscribers that are removed with unsubscribe
        // do not have a call in progress when unsubscribe returns. See comments in postNotification.
        
        MutexLock lock(_pMutex);

        // there is usually only one notification call active at any given point in time.
        // However, if events are handled somewhere in an inner function (e.g. by a modal dialog
        // created by some other framework) then a second notification call might be executed
        // before the first can finish. The mutex does not protect against this since both calls
        // would happen in the same thread (the main thread).

        // We also need to ensure that subscribers that are removed while our notification call
        // is running are not called after they are removed.
        // Note that subscribers can ONLY be removed during a notification if a callback
        // function does that. Other threads cannot remove subscribers during this because we keep
        // our mutex locked.
        
        // So we keep a list for each running notification. If subscribers
        // are removed during a callback then unsubscribe() will update the notification structure accordingly.

        typename std::list< NotificationState >::iterator notificationStateSlotIt = _inProgressNotificationStates.emplace( _inProgressNotificationStates.end() );
        NotificationState&                       state = *notificationStateSlotIt;
                
        try
        {
            state.nextItemIt = _subMap.begin();

            while(state.nextItemIt != _subMap.end())
            {
                std::pair< int64_t, Sub_ > item = *state.nextItemIt;

                // increase the iterator before we call the notification function. That is necessary
                // for cases when the notification function removes the next item in our map.
                // The unsubscribe function will update the iterator properly, to ensure that
                // it remains valid.
                state.nextItemIt++;

                // note that we have to keep the mutex locked during the call to ensure that
                // an unsubscribe call in another thread will wait for the call to finish before it returns.
                
                // Having the mutex locked should not create the potential for deadlocks, since
                // we are called directly from the main loop and thus no other mutexes can be locked
                // by our thread. 
                            
                try
                {
                    item.second.func( args... );
                }
                catch(DanglingFunctionError&)
                {
                    // this is a perfectly normal case. It means that the target function
                    // was a weak reference and the target object has been destroyed.
                    // Just remove it from our list and ignore the exception.

                    unsubscribe(item.first);
                }
            }
        }
        catch(...)
        {
            _inProgressNotificationStates.erase( notificationStateSlotIt );
            throw;
        }

        _inProgressNotificationStates.erase( notificationStateSlotIt );
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
        SubControl_(Notifier* pParent, int64_t subId)
        {
            _pParent = pParent;
            _subId = subId;

            // we use the same mutex as our parent to avoid deadlocks
            _pMutex = pParent->_pMutex;
        }
        
        ~SubControl_()
        {
            MutexLock lock(_pMutex);
            
            if(_pParent!=nullptr)
                _pParent->subControlDeleted(_subId);
        }

        void detachFromParent()
        {
            MutexLock lock(_pMutex);

            _pParent = nullptr;
        }

        void unsubscribe()
        {
            MutexLock lock(_pMutex);
            
            if(_pParent!=nullptr)
            {
                _pParent->unsubscribe(_subId);
                _pParent = nullptr;
            }
        }


    private:
        P<Mutex>            _pMutex;
        Notifier*			_pParent;
        int64_t             _subId;
        
        friend class Notifier;
    };
    friend class SubControl_;


    /** Unsubscribed a function from the notifier.
        
        \param subId the subscription id that was returned by a previous call to subscrive().*/
    void unsubscribe(int64_t subId)
    {
        MutexLock lock(_pMutex);

        auto it = _subMap.find(subId);
        if(it!=_subMap.end())
        {
            Sub_& sub = it->second;

            if(sub.pControlWeak!=nullptr)
            {
                sub.pControlWeak->detachFromParent();
                sub.pControlWeak = nullptr;
            }                

            // note that a notification cannot currently be in progress in
            // ANOTHER thread (since we lock the mutex during notifications).
            // But the a notification might be in progress in our thread. This can
            // happen if the unsubscribe happens in response to the notification call.
            // We MUST ensure that we do not call this subscriber after unsubscribe
            // returns (since it might be deleted immediately afterwards).
            // The local states from each running notification are in _inProgressNotificationStates.
            // We update those accordingly and remove this subscriber if it is in there.
            for( NotificationState& state: _inProgressNotificationStates )
            {
                if(state.nextItemIt==it)
                    state.nextItemIt++;
            }           
            
            _subMap.erase(it);
        }
    }


    void subControlDeleted(int64_t subId)
    {
        MutexLock lock(_pMutex);

        auto subIt = _subMap.find(subId);
        if(subIt!=_subMap.end())
            subIt->second.pControlWeak = nullptr;
    }
    
    
    struct Sub_
    {
        Sub_()
            : pControlWeak(nullptr)
        {
        }

        Sub_(const std::function<void(ArgTypes...)>& func, SubControl_* pControl)
            : func(func)
            , pControlWeak(pControl)
        {
        }

        

        std::function<void(ArgTypes...)> func;

        // weak by design. The sub control objects notify us when they are deleted
        SubControl_*                     pControlWeak;
    };

    struct NotificationState
    {
        typename std::map<int64_t, Sub_ >::iterator   nextItemIt;
    };

    
    P<Mutex>                            _pMutex;
    int64_t                             _nextSubId = 1;
    std::map<int64_t,  Sub_>            _subMap;
    std::list< NotificationState >      _inProgressNotificationStates;
};
    
}


#endif


