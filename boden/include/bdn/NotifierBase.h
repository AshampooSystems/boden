#ifndef BDN_NotifierBase_H_
#define BDN_NotifierBase_H_


#include <bdn/IAsyncNotifier.h>
#include <bdn/ISyncNotifier.h>
#include <bdn/DanglingFunctionError.h>
#include <bdn/RequireNewAlloc.h>

#include <bdn/Map.h>

#include <functional>

namespace bdn
{


/** Base class for notifier implementations.

    NotifierBase objects MUST be allocated with newObj / new.

	The MUTEX_TYPE template parameter indicates the type of the mutex object that the Notifier
	uses. Pass bdn::Mutex to use a normal mutex. You can also pass bdn::DummyMutex to 
	use a fake mutex that does nothing (thus removing multithread support).
*/
template<class MUTEX_TYPE, class... ARG_TYPES>
class NotifierBase : public RequireNewAlloc<Base, NotifierBase<MUTEX_TYPE, ARG_TYPES...> >
    , BDN_IMPLEMENTS INotifierBase<ARG_TYPES...>
{
public:
    NotifierBase()
    {
    }

    ~NotifierBase()
    {
    }
        
    P<INotifierSubControl> subscribe(const std::function<void(ARG_TYPES...)>& func) override
    {
        int64_t subId = doSubscribe(func);

        return newObj<SubControl_>(this, subId);        
    }

     
   

    INotifierBase<ARG_TYPES...>& operator+=(const std::function<void(ARG_TYPES...)>& func) override
    {
        doSubscribe(func);

        return *this;
    }



    P<INotifierSubControl> subscribeParamless(const std::function<void()>& func) override
    {
        return subscribe( ParamlessFunctionAdapter(func) );
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
	
	/** Perform a notification call.*/
	virtual void doNotify(ARG_TYPES... args)
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
                deactivateNotificationState(&state);
                throw;
            }

            deactivateNotificationState(&state);
        }
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
    

    void unsubscribe(int64_t subId)
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



    class SubControl_ : public Base, BDN_IMPLEMENTS INotifierSubControl
    {
    public:		
        SubControl_(NotifierBase* pParent, int64_t subId)
            : _pParentWeak( pParent )
            , _subId(subId)
        {
        }
        
        void unsubscribe() override
        {              
            P<NotifierBase> pParent = _pParentWeak.toStrong();
            if(pParent!=nullptr)
                pParent->unsubscribe(_subId);
        }


    private:
        WeakP<NotifierBase> 	_pParentWeak;
        int64_t                 _subId;
        
        friend class NotifierBase;
    };
    friend class SubControl_;



    
    MUTEX_TYPE                          _mutex;
    int64_t                             _nextSubId = 1;
    Map<int64_t,  Sub_>					_subMap;
    NotificationState*                  _pFirstNotificationState = nullptr;
};
    
}


#endif


