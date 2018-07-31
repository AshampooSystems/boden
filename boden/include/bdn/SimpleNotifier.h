#ifndef BDN_SimpleNotifier_H_
#define BDN_SimpleNotifier_H_

#include <bdn/DummyMutex.h>
#include <bdn/ISyncNotifier.h>
#include <bdn/NotifierBase.h>

namespace bdn
{


/** A simple, fast synchrnous notifier. See ISyncNotifier.

	SimpleNotifier is **not** thread safe.
	
	See ThreadSafeNotifier for an alternative that is thread safe and also
	supports IAsyncNotifier.

	SimpleNotifier objects MUST be allocated with newObj.
*/
template<class... ARG_TYPES>
class SimpleNotifier : public NotifierBase< DummyMutex, ARG_TYPES... >
								// note that we use DummyMutex (i.e. no actual mutex operations will happen)
	, BDN_IMPLEMENTS ISyncNotifier<ARG_TYPES...>
{
private:
    using BASE = NotifierBase< DummyMutex, ARG_TYPES... >;
    
public:
    SimpleNotifier()
    {
    }

    ~SimpleNotifier()
    {
    }
        
	
	void notify(ARG_TYPES... args)
	{
		BASE::doNotify( std::forward<ARG_TYPES>(args)... );
	}
    
	
private:
};
    
}


#endif


