#ifndef BDN_FK_IDLERUNNER_HH_
#define BDN_FK_IDLERUNNER_HH_

#include <bdn/ISimpleCallable.h>

#include <list>

#import <CoreFoundation/CoreFoundation.h>

namespace bdn
{
    
namespace fk
{


/** A helper class that runs tasks when the app is idle.*/
class IdleRunner : public Base
{
public:
    IdleRunner()
    {
        _pCaller = newObj<Caller>();
    }
    
    ~IdleRunner();
    
    
    /** Schedule the specified callable to be called ONCE the next time
        the app goes into idle state.
        
        Also calls the callable if the app is already in idle state.
        */
    void callOnceWhenIdle( ISimpleCallable* pCallable);
    
    
    /** Returns the global IdleRunner object.*/
    static IdleRunner& get();
    
private:
    
    // we need to uninstall the observer asynchronously, because our destructor might be called from
    // another thread. Because of this we have to put everything that the observer uses in a separate
    // object that remains valid even after our destructor has been called.
    class Caller : public Base
    {
    public:
        void addCall(ISimpleCallable* pCallable)
        {
            _callList.push_back(pCallable);
        }
        
        void performCalls();
        
    private:
        std::list< P<ISimpleCallable> > _callList;
    };
    
    
    void ensureObserverInstalled();
    
    bool                    _observerInstalled = false;
    CFRunLoopObserverRef    _observer;
    
    P<Caller>               _pCaller;
};

}
    
}

#endif
