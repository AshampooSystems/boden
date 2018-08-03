#ifndef BDN_NotifierBase_H_
#define BDN_NotifierBase_H_


#include <bdn/IAsyncNotifier.h>
#include <bdn/ISyncNotifier.h>
#include <bdn/DanglingFunctionError.h>

#include <bdn/Map.h>

#include <functional>

namespace bdn
{


/** Base class for notifier implementations.

	The MUTEX_TYPE template parameter indicates the type of the mutex object that the Notifier
	uses. Pass bdn::Mutex to use a normal mutex. You can also pass bdn::DummyMutex to 
	use a fake mutex that does nothing (thus removing multithread support).
*/
template<class MUTEX_TYPE, class... ARG_TYPES>
class NotifierBase : public Base
    , BDN_IMPLEMENTS INotifierBase<ARG_TYPES...>
{
public:
    NotifierBase()
    {
    }

    ~NotifierBase()
    {
    }
        
    P<INotifierSubscription> subscribe(const std::function<void(ARG_TYPES...)>& func) override
    {
        int64_t subId = doSubscribe(func);

        return newObj<Subscription_>(subId);
    }

     
   

    INotifierBase<ARG_TYPES...>& operator+=(const std::function<void(ARG_TYPES...)>& func) override
    {
        doSubscribe(func);

        return *this;
    }



    P<INotifierSubscription> subscribeParamless(const std::function<void()>& func) override
    {
        return subscribe( ParamlessFunctionAdapter(func) );
    }
    
    
    void unsubscribe(INotifierSubscription* pSub) override
    {
        unsubscribeById( cast<Subscription_>(pSub)->subId() );
    }
	   
    void unsubscribeAll() override
    {
        typename MUTEX_TYPE::Lock lock(_mutex);

        _subMap.clear();

        NotificationState* pCurr = _pFirstNotificationState;            
        while(pCurr!=nullptr)
        {
            pCurr->nextItemIt = _subMap.end();
            pCurr = pCurr->pNext;
        }        
    }


protected:
    
    /** Perform a notification call. This is the internal implementation
        that performs the actual notification calls.
     
        doNotify calls this.
     
        Template parameters:
     
        - CALL_MAKER_TYPE the type of the call maker function (see callMaker parameter)
        - ADDITIONAL_CALL_MAKER_ARGS the types of additional arguments to pass to the call maker
            function (see additionalCallMakerArgs parameter).
     
        \param callMaker a helper function that performs the actual individual call
            for a single subscriber. As its first parameter this function gets the
            subscribed function object (of type std::function<void(ARG_TYPES...) ).
            It can optionally also get additional arguments, as specified by additionalCallMakerArgs.
        \param additionalCallMakerArgs an arbitrary number of additional arguments
            that are passed to the call maker function. The number and type of these are
            controlled by the function's template argument ADDITIONAL_CALL_MAKER_ARGS.
     */
    template< typename CALL_MAKER_TYPE, typename... ADDITIONAL_CALL_MAKER_ARGS >
    void notifyImpl( CALL_MAKER_TYPE callMaker, ADDITIONAL_CALL_MAKER_ARGS... additionalCallMakerArgs )
    {
        // we do not want to hold a mutex while we call each subscriber. That would create the potential
        // for deadlocks. However, we need to hold a mutex to access our internal structures, up to
        // the point in time when we actually make the call.
        
        {
            typename MUTEX_TYPE::Lock lock(_mutex);
            
            // We also need to ensure that subscribers that are removed while our notification call
            // is running are not called after they are removed (unless their specific call has already started).
            
            // When we have an async notifier then there is usually only one notification call active at any given point in time, since all of them
            // are made from the main thread.
            // However, if the event loop does work somewhere in an inner function (e.g. by a modal dialog
            // created by some other framework) then a second notification call might be executed
            // before the first can finish. The mutex does not protect against this since both calls
            // would happen in the same thread (the main thread).
            
            // When we are a synchronous notifier then there can also be additional notify calls happening while
            // we are inside the loop (if one of our subscribers causes another notification to happen).
            // So we also have to be re-entrant in that case.
            
            NotificationState state;
            
            state.nextItemIt = _subMap.begin();
            
            activateNotificationState(&state);
            
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
                        typename MUTEX_TYPE::Unlock unlock(_mutex);
                        
                        // note: we MUST NOT use std::forward here, since we may have to call multiple
                        // subscribers. std::forward might convert the temporary object to a move reference,
                        // this the additionalCallMakerArgs variable might otherwise be invalidated by the
                        // first subscriber call.
                        callMaker( item.second.func, additionalCallMakerArgs... );
                    }
                    catch(DanglingFunctionError&)
                    {
                        // this is a perfectly normal case. It means that the target function
                        // was a weak reference and the target object has been destroyed.
                        // Just remove it from our list and ignore the exception.
                        
                        unsubscribeById(item.first);
                    }
                }
            }
            catch(...)
            {
                deactivateNotificationState(&state);
                throw;
            }
            
            deactivateNotificationState(&state);
        }
    }
    
    /** A default call maker implementation that simply calls the subscribed function directly.
        This can be used with notifyImpl().*/
    static void defaultCallMaker( const std::function<void(ARG_TYPES...)>& func, ARG_TYPES... args )
    {
        func( args... );
    }
	
	/** Perform a notification call.*/
	virtual void doNotify(ARG_TYPES... args)
	{
        notifyImpl< decltype(&NotifierBase::defaultCallMaker), ARG_TYPES... >( &NotifierBase::defaultCallMaker, args... );
    }

	/** Subscribed the specified function \c func.
	
		Returns the ID of the created subscription.	
	*/	
    virtual int64_t doSubscribe(const std::function<void(ARG_TYPES...)>& func)
    {
        typename MUTEX_TYPE::Lock lock(_mutex);

        int64_t subId = _nextSubId;
        _nextSubId++;
        
        _subMap[subId] = Sub_(func);

        return subId;
    }
            

	/** Returns a reference to the mutex that the notifier uses.*/
	MUTEX_TYPE& getMutex()
	{
		return _mutex;
	}
	
    
private:

    struct Sub_
    {
        Sub_()
        {
        }

        Sub_(const std::function<void(ARG_TYPES...)>& func)
            : func(func)
        {
        }

        std::function<void(ARG_TYPES...)> func;
    };

    
    struct NotificationState
    {
        NotificationState* pNext = nullptr;

        typename Map<int64_t, Sub_ >::Iterator   nextItemIt;        
    };
    

    void unsubscribeById(int64_t subId)
    {
        typename MUTEX_TYPE::Lock lock(_mutex);

        auto it = _subMap.find(subId);
        if(it!=_subMap.end())
        {
            // notifications are only done in the main thread.
            // But there can still be multiple notifications running, if the event loop
            // is worked from an inner function (like a modal dialog that was created by
            // another framwork).
            NotificationState* pState = _pFirstNotificationState;
            while(pState!=nullptr)
            {
                if(pState->nextItemIt==it)
                    pState->nextItemIt++;

                pState = pState->pNext;
            }
            
            _subMap.erase(it);
        }
    }


    void activateNotificationState(NotificationState* pState)
    {
        pState->pNext = _pFirstNotificationState;
        _pFirstNotificationState = pState;
    }

    void deactivateNotificationState(NotificationState* pState)
    {
        // we usually have only one notification state active
        if(pState==_pFirstNotificationState)
            _pFirstNotificationState = pState->pNext;
        else
        {
            NotificationState* pPrev = _pFirstNotificationState;
            NotificationState* pCurr = _pFirstNotificationState->pNext;
            
            while(pCurr!=nullptr)
            {
                if(pCurr==pState)
                {
                    pPrev->pNext = pState->pNext;
                    break;
                }

                pState = pState->pNext;
            }
        }
    }
    

    class ParamlessFunctionAdapter
    {
    public:
        ParamlessFunctionAdapter( std::function<void()> func)
        {
            _func = func;
        }
        
        void operator()(ARG_TYPES... args)
        {
            _func();
        }
        
    protected:
        std::function<void()> _func;
    };



    class Subscription_ : public Base, BDN_IMPLEMENTS INotifierSubscription
    {
    public:		
        Subscription_(int64_t subId)
            : _subId(subId)
        {
        }
        
        int64_t subId() const
        {
            return _subId;
        }
        
    private:
        int64_t                 _subId;
    };


    
    MUTEX_TYPE                          _mutex;
    int64_t                             _nextSubId = 1;
    Map<int64_t,  Sub_>					_subMap;
    NotificationState*                  _pFirstNotificationState = nullptr;
};
    
}


#endif


