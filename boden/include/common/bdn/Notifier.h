#ifndef BDN_EventSource_H_
#define BDN_EventSource_H_

#include <bdn/init.h>

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

	// Subscribe myFunc to the notifier
	auto pSub = eventNotifier.subscribe(myFunc);

	// when the event occurs then someone needs to call the "notify" method of
	// the notifier and pass the correct parameters that describe the event.
	eventNotifier.notify("Some text", 42);

	// When the pSub object is destroyed then the corresponding
	// function is automatically removed.
	pSub = nullptr;

	// So this will NOT call myFunc anymore, since pSub was set to null and the
	// subscription has been destroyed.
	eventNotifier.notify("Other text", 17);


	\endcode	
*/
template<class... ArgTypes>
class Notifier : public Base
{
public:
    Notifier()
    {
    }
    
    ~Notifier()
    {
        MutexLock lock(_mutex);
        
        for(auto pSub: _subList)
            pSub->detachFromParent();
    }
    

	/** The interface of subscription objects. Note that they currently do not define
		any methods.*/
	class ISub : BDN_IMPLEMENTS IBase
    {	    
    };
    
    /** Subscribes a function to the notifier. While subscribed, the function will be called
		whenever the notifier's notify() function	is called.	

		The function parameter list must match the template parameters of the notifier object.
		Note that for convenience there is also a version of subscribe() that subscribes a function
		without parameters. You can use that if your notification function does not care about the
		event parameters.

		The function returns a pointer to a subscription object that controls when the function is unsubscribed.
		The subscription remains in place as long as the returned subscription object is alive. In other words:
		you should store the returned pointer in some place and destroy it or set it to null when you want the subscription
		to go away.

		Example:

		\code

		Notifier<String, int> eventNotifier;

		// this function should be called when the notifier fires
		void myFunc(String x, int y)
		{
			... handle notification
		}

		// Subscribe myFunc to the notifier and store the returned
		// subscription pointer
		P<IBase> pSub = eventNotifier.subscribe(myFunc);

		// myFunc will now be called whenever the notifier is fired.

		// Now destroy the subscription object
		pSub = nullptr;

		// From this point on the subscription is deleted and myFunc will
		// not be called anymore.

		\endcode
		*/
    P<ISub> subscribe( const std::function<void(ArgTypes...)>& func)
    {
        MutexLock lock(_mutex);
        
        P<Sub_> pSub = newObj<Sub_>(this, func);

        _subList.push_back( pSub );
        
        return pSub;
    }
    

	/** Convenience function. Similar to the other version of subscribe(), except that this
		one takes a function without parameters and subscribes it to the event. You can use this
		if your function is not interested in the event parameters and only cares about when the event
		itself happens.		.
		*/
    P<ISub> subscribeVoid( const std::function<void()>& func)
    {
        return subscribe( VoidFunctionAdapter(func) );
    }
    
    
    template<class OwnerType>
    P<ISub> subscribeMember( OwnerType* pOwner, const std::function<void(OwnerType*,ArgTypes...)>& func)
    {
		return subscribe(	[pOwner, func](ArgTypes... args)
							{
								func(pOwner, args...);
							} );
    }
    
    template<class OwnerType>
    P<ISub> subscribeVoidMember( OwnerType* pOwner, const std::function<void(OwnerType*)>& func)
    {
        return subscribe( VoidFunctionAdapter( std::bind(func, pOwner) ) );
    }
    
    
    void notify(ArgTypes... args)
    {
        MutexLock lock(_mutex);
        
        // make a note of the subscribers that we still need to call.
        // We must make this copy of the list, so that we can handle cases
        // when a subscriber unsubscribes in response to the call.
        _remainingSubListForCall = _subList;
        
        while(!_remainingSubListForCall.empty())
        {
            auto pSub = _remainingSubListForCall.front();
            _remainingSubListForCall.pop_front();
            
            pSub->call(args...);
        }
    }
    
protected:

	class Sub_ : public Base, BDN_IMPLEMENTS ISub
    {
    public:		
        Sub_(Notifier* pParent, const std::function<void(ArgTypes...)>& func)
        {
            _pParent = pParent;
            _func = func;
        }

        ~Sub_()
        {
			MutexLock lock(_parentMutex);
            
            if(_pParent!=nullptr)
                _pParent->unsubscribe(this);
        }
        
        
        void detachFromParent()
        {
            MutexLock lock(_parentMutex);
            
            _pParent = nullptr;
        }
        
        void call(ArgTypes... args)
        {
            _func(args...);
        }
        
        Mutex							 _parentMutex;
        Notifier*						 _pParent;
        std::function<void(ArgTypes...)> _func;
        
        friend class Notifier;
    };
    friend class Sub_;
    
    void unsubscribe(Sub_* pSub)
    {
        MutexLock lock(_mutex);
        
        auto it = std::find(_subList.begin(), _subList.end(), pSub);
        if(it!=_subList.end())
            _subList.erase( it );
        
        // note that a notification cannot currently be in progress in
        // ANOTHER thread (since we lock the mutex).
        // But the a notification might be in progress in our thread. This can
        // happen if the unsubscribe happens in response to the notification call.
        // We MUST ensure that we do not call this subscriber after unsubscribe
        // returns (since it might be deleted immediately afterwards).
        // So we simply also remove it from the _remainingSubListForCall.
        
        auto remIt = std::find(_remainingSubListForCall.begin(), _remainingSubListForCall.end(), pSub);
        if(remIt!=_remainingSubListForCall.end())
            _remainingSubListForCall.erase( remIt );
        
    }
    
    class VoidFunctionAdapter
    {
    public:
        VoidFunctionAdapter( std::function<void()> func)
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
    
    Mutex               _mutex;
    
    // weak by design. We do not keep subscriptions alive
    std::list< Sub_* >   _subList;
    std::list< Sub_* >   _remainingSubListForCall;
};
    
}


#endif


