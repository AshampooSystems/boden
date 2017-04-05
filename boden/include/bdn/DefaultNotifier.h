#ifndef BDN_Notifier_H_
#define BDN_Notifier_H_

#include <bdn/init.h>

#include <bdn/INotifier.h>
#include <bdn/DanglingFunctionError.h>
#include <bdn/RequireNewAlloc.h>
#include <bdn/mainThread.h>

#include <map>

namespace bdn
{


/** Default implementation for INotifier. See INotifier for more information.

    DefaultNotifier objects MUST be allocated with newObj / new.
*/
template<class... ArgTypes>
class DefaultNotifier : public RequireNewAlloc<Base, DefaultNotifier<ArgTypes...> >
    , BDN_IMPLEMENTS INotifier<ArgTypes...>
{
public:
    DefaultNotifier()
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
        // see doc_input/notifier_internal.md for more information about why this has to redirect
        // to the main thread.
        
        asyncCallFromMainThread( std::bind<void, std::function<void(ArgTypes...)>, ArgTypes...>( strongMethod(this, &DefaultNotifier::doNotifyFromMainThread) , std::forward<ArgTypes>(args)... ) );
    }

   
    void unsubscribeAll() override
    {
        MutexLock lock(_mutex);

        _subMap.clear();

        NotificationState* pCurr = _pFirstNotificationState;            
        while(pCurr!=nullptr)
        {
            pCurr->nextItemIt = _subMap.end();
            pCurr = pCurr->pNext;
        }        
    }

    
private:

    int64_t subscribeInternal(const std::function<void(ArgTypes...)>& func)
    {
        MutexLock lock(_mutex);

        int64_t subId = _nextSubId;
        _nextSubId++;
        
        _subMap[subId] = Sub_(func);

        return subId;
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
        NotificationState* pNext = nullptr;

        typename std::map<int64_t, Sub_ >::iterator   nextItemIt;        
    };
    

    void unsubscribe(int64_t subId)
    {
        MutexLock lock(_mutex);

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
    
    void doNotifyFromMainThread(ArgTypes... args)
    {
        // we do not want to hold a mutex while we call each subscriber. That would create the potential
        // for deadlocks. However, we need to hold a mutex to access our internal structures, up to
        // the point in time when we actually make the call.

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
                        MutexUnlock unlock(_mutex);
                    
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
    NotificationState*                  _pFirstNotificationState = nullptr;
};
    
}


#endif


