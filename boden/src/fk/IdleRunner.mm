#include <bdn/init.h>
#import <bdn/fk/IdleRunner.hh>

#include <bdn/log.h>
#include <bdn/mainThread.h>

#import <Foundation/Foundation.h>

namespace bdn
{
namespace fk
{
    
    
BDN_SAFE_STATIC_IMPL( IdleRunner, IdleRunner::get );


IdleRunner::~IdleRunner()
{
    if(_observerInstalled)
    {
        // unregister our observer asynchronously because the destructor might
        // be called from a different thread.
        // Note that the observer only holds a reference to the Caller object, which
        // remains valid as long as the observer exists.
        CFRunLoopObserverRef obs = _observer;
        asyncCallFromMainThread(
                                [obs]()
                                {
                                    CFRunLoopRemoveObserver([NSRunLoop mainRunLoop].getCFRunLoop, obs, kCFRunLoopCommonModes);
                                } );
        
        _observerInstalled = false;
    }
}

void IdleRunner::callOnceWhenIdle( ISimpleCallable* pCallable)
{
    // we must only access the run loop from the main thread.
    // Also, if the run loop is currently idle (=waiting) then
    // we need to break that idle phase so that we will get
    // a notification when the loop is idle next.
    
    // Both of these can be solved by scheduling the call async.
    
    P<IdleRunner>       pThis = this;
    P<ISimpleCallable>  pCallableWithRef = pCallable;
    asyncCallFromMainThread(
                            [pCallableWithRef, pThis]()
                            {
                                pThis->_pCaller->addCall(pCallableWithRef);
                                
                                pThis->ensureObserverInstalled();
                            } );
}

void IdleRunner::ensureObserverInstalled()
{
    if(!_observerInstalled)
    {
        P<Caller> pCaller = _pCaller;
        
        id handler = ^(CFRunLoopObserverRef observer, CFRunLoopActivity activity)
        {
            if(activity==kCFRunLoopBeforeWaiting)
                pCaller->performCalls();
        };
        
        _observer = CFRunLoopObserverCreateWithHandler(kCFAllocatorDefault, kCFRunLoopAllActivities, true, 0 /* order */, handler);
        CFRunLoopAddObserver([NSRunLoop mainRunLoop].getCFRunLoop, _observer, kCFRunLoopCommonModes);
        CFRelease(_observer);
        
        _observerInstalled = true;
    }
}


void IdleRunner::Caller::performCalls()
{
    // make a copy of the idle list, so that newly added idle handlers only
    // run in the next phase.
    std::list< P<ISimpleCallable> > toCall = _callList;
    _callList.clear();
    
    // we want idle tasks to be released immediately after they are executed.
    // So we remove them from the list immediately.
    while(!toCall.empty())
    {
        P<ISimpleCallable>& pCallable = toCall.front();
        
        try
        {
            pCallable->call();
        }
        catch(std::exception& e)
        {
            // log and ignore
            logError(e, "Exception thrown by idle task in IdleRunner_::beginningIdlePhase. Ignoring.");
        }
        
        toCall.pop_front();
    }
}


}
}
