#ifndef BDN_ThreadSafeNotifier_H_
#define BDN_ThreadSafeNotifier_H_


#include <bdn/IAsyncNotifier.h>
#include <bdn/ISyncNotifier.h>
#include <bdn/NotifierBase.h>
#include <bdn/mainThread.h>
#include <bdn/RequireNewAlloc.h>

#include <bdn/Map.h>

namespace bdn
{


/** A thread safe notifier implementation. ThreadSafeNotifier supports both the
	IAsyncNotifier and the ISyncNotifier interfaces.

    ThreadSafeNotifier objects MUST be allocated with newObj / new.
*/
template<class... ARG_TYPES>
class ThreadSafeNotifier
    : public RequireNewAlloc< NotifierBase< Mutex, ARG_TYPES... >, ThreadSafeNotifier<ARG_TYPES...> >
    , BDN_IMPLEMENTS IAsyncNotifier<ARG_TYPES...>
	, BDN_IMPLEMENTS ISyncNotifier<ARG_TYPES...>
{
private:
    using BASE = NotifierBase< Mutex, ARG_TYPES... >;
    
public:
    ThreadSafeNotifier()
    {
    }

    ~ThreadSafeNotifier()
    {
    }
        
	
	void notify(ARG_TYPES... args) override
	{
		BASE::doNotify( std::forward<ARG_TYPES>(args)... );
	}
    
    void postNotification(ARG_TYPES... args) override
    {
        // see doc_input/notifier_internal.md for more information about why this has to redirect
        // to the main thread.
        
        asyncCallFromMainThread(
            std::bind<void, std::function<void(ARG_TYPES...)>, ARG_TYPES...>(
                strongMethod(this, &ThreadSafeNotifier::notify),
                std::forward<ARG_TYPES>(args)... ) );
    }

    
private:
};
    
}


#endif


