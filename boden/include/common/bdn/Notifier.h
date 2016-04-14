#ifndef BDN_EventSource_H_
#define BDN_EventSource_H_

#include <bdn/init.h>

#include <functional>
#include <list>

namespace bdn
{

template<class EventType>
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
    
    
    class Sub : public Base
    {
    public:
        ~Sub()
        {
            onDelete.call(*this);
            
            MutexLock lock(_parentMutex);
            
            if(_pParent!=nullptr)
                _pParent->unsubscribe(this);
        }
        
        EventSource<Sub> onDelete;
        
    protected:
        Sub(EventSource* pParent, const std::function<void(const EventType&)>& func)
        {
            _pParent = pParent;
            _func = func;
        }
        
        void detachFromParent()
        {
            MutexLock lock(_parentMutex);
            
            _pParent = nullptr;
        }
        
        void call(const EventType& e)
        {
            _func(e);
        }
        
        Mutex                                   _parentMutex;
        EventSource*                            _pParent;
        std::function<void(const EventType&)>   _func;
        
        friend class EventSource;
    };
    friend class Sub;
    
    
    P<IBase> subscribe( const std::function<void(const EventType&)>& func)
    {
        MutexLock lock(_mutex);
        
        P<Sub> pSub = newObj<Sub>(this, func);

        _subList.push_back( pSub );
        
        return pSub;
    }
    
    P<IBase> subscribe( const std::function<void()>& func)
    {
        return subscribe( VoidFunctionAdapter(func) );
    }
    
    
    template<class OwnerType>
    P<IBase> subscribeMember( OwnerType* pOwner, const std::function<void(OwnerType*,const EventType&)>& func)
    {
        return subscribe( std::bind(func, pOwner, std::placeholders::_1 ) );
    }
    
    template<class OwnerType>
    P<IBase> subscribeMember( OwnerType* pOwner, const std::function<void(OwnerType*)>& func)
    {
        return subscribe( VoidFunctionAdapter( std::bind(func, pOwner, std::placeholders::_1 ) ) );
    }
    
    
    void deliver(const EventType& evt)
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
            
            pSub->call(evt);
        }
    }
    
protected:
    
    void unsubscribe(Sub* pSub)
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
        
        void operator()(const EventType& evt)
        {
            _func();
        }
        
    protected:
        std::function<void()> _func;
    };
    
    Mutex               _mutex;
    
    // weak by design. We do not keep subscriptions alive
    std::list< Sub* >   _subList;
    std::list< Sub* >   _remainingSubListForCall;
};
    
}


#endif


