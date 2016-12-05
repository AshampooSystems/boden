#ifndef BDN_WEBEMS_MainDispatcher_H_
#define BDN_WEBEMS_MainDispatcher_H_

#include <bdn/IDispatcher.h>

namespace bdn
{
namespace webems
{

/** IDispatcher implementation for webems applications.
    */
class MainDispatcher : public Base, BDN_IMPLEMENTS IDispatcher
{
public:
	MainDispatcher();

    void dispose();
    

	void enqueue( std::function<void()> func, Priority priority = Priority::normal ) override;

	void enqueueInSeconds(double seconds, std::function<void()> func, Priority priority = Priority::normal ) override;

	void createTimer(
		double intervalSeconds,
		std::function< bool() > func ) override;
        
        
private:
        
    void processItem(std::list< std::function<void()> >& queue);
    
    struct TimedItem
    {
        TimedItem(const std::function< void() >& func, MainDispatcher* pDispatcher)
        : func(func)
        , pDispatcher(pDispatcher)
        {
        }
        
        std::function< void() >         func;
        std::list<TimedItem>::iterator  it;
        
        // strong reference. This is intended
        P<MainDispatcher>               pDispatcher;
    };
    
    void processTimedItem(TimedItem* pItem);
    
    
    struct Timer : public Base
    {
        Timer( const std::function<bool()>& func, MainDispatcher* pDispatcher)
        : func(func)
        , pDispatcher( pDispatcher )
        {
        }
        
        // strong reference to dispatcher. That is by design
        P<MainDispatcher> pDispatcher;
        
        std::function<bool()>           func;
        std::list< Timer >::iterator    it;
        bool                            disposed = false;
    };
    
    bool processTimer(Timer* pTimer);
    
   
    
    
    Mutex _queueMutex;
    
    std::list< std::function<void()> >  _normalQueue;
    std::list< std::function<void()> >  _idleQueue;
    
    std::list< TimedItem >  _timedItemList;    
    std::list< Timer >      _timerList;
};


}
}

#endif
