#ifndef _BDN_EventSource_H_
#define _BDN_EventSource_H_

#include <bdn/Base.h>

#include <functional>
#include <list>

namespace bdn
{

template<class EVENT>
class EventSource : public Base
{
public:
    EventSource()
    {
    }
    
    void subscribe( const std::function<void(EVENT&)>& func)
    {
        _subscriberList.push_back(func);
    }
    
    template<class OWNER>
    void subscribeMember( OWNER* pOwner, const std::function<void(OWNER*,EVENT&)>& func)
    {
        subscribe( std::bind(func, pOwner, std::placeholders::_1 ) );
    }
    
    void deliver(EVENT& evt)
    {
        for(std::function<void(EVENT&)>& subscriber: _subscriberList)
        {
            subscriber(evt);
        }
    }
    
protected:
    std::list< std::function<void(EVENT&)> > _subscriberList;
};
    
}


#endif


