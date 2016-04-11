#ifndef BDN_EventDispatcher_H_
#define BDN_EventDispatcher_H_


namespace bdn
{
    
/** Manages the event loop of the app. */
class EventDispatcher : public Base
{
public:

    
    /** Schedules the specified function to be called from the main UI
        thread.
     
        This is often used to schedule UI elements to be updated from
        a worker thread.
     */
    static void asyncCallFromMainThread( std::function func );

    
    
};

    
    
}


#endif