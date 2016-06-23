#ifndef BDN_PosixThreadLocalStorageManager_H_
#define BDN_PosixThreadLocalStorageManager_H_

#include <bdn/sysError.h>

#include <unordered_map>

#include <pthread.h>

namespace bdn
{

/** Manager for Posix-style thread local storage.

    IMPORTANT: It is rarely advisable to use this directly. Instead
    you should use the macro #BDN_STATIC_THREAD_LOCAL_PTR to create thread-local
    objects.

	*/
class PosixThreadLocalStorageManager : public Base
{
public:
	PosixThreadLocalStorageManager()
    {
        _nextSlotId=0;
        
        int result = pthread_key_create(&_key, &PosixThreadLocalStorageManager::_threadExitCleanup);
        if(result!=0)
        {
            throwSysError( result,
                           ErrorFields().add("func", "pthread_key_create")
                                        .add("action", "PosixThreadLocalStorageManager constructor") );
        }
    }
    
    
    ~PosixThreadLocalStorageManager()
    {
        pthread_key_delete(_key);
    }
    
    
	/** Creates a new slot for a value.
		Slots do not have to be deleted.
		
		The number of slots is not limited. The
		whole manager uses only one of the system's
		thread local storage slots.
		*/
	int		 createSlot()
    {
        return _nextSlotId.fetch_add(1);
    }
	
    

	/** Returns a reference to the thread-local pointer for the specified slot.
        */
    P<IBase>&	getThreadPtrRef(int slot)
    {
        std::unordered_map<int, P<IBase> >* pMap = static_cast< std::unordered_map<int, P<IBase> >* >( pthread_getspecific(_key) );
        
        if(pMap==nullptr)
        {
            pMap = ::new std::unordered_map<int, P<IBase> >;
            
            int result = pthread_setspecific(_key, pMap );
            if(result!=0)
            {
                throwSysError( result,
                              ErrorFields() .add("func", "pthread_setspecific")
                                            .add("action", "PosixThreadLocalStorageManager::getThreadPtrRef") );
                
            }
        }
        
        return (*pMap)[slot];
    }
    
    
    static P<PosixThreadLocalStorageManager> get()
	{
        static SafeInit<PosixThreadLocalStorageManager> init;
        
        return init.get();
    }
    

protected:
    static void _threadExitCleanup(void* pValue)
    {
        std::unordered_map<int, P<IBase> >* pMap = static_cast< std::unordered_map<int, P<IBase> >* >(pValue);
        
        if(pMap!=nullptr)
            delete pMap;
    }
    

	pthread_key_t       _key;
    std::atomic<int>    _nextSlotId;
};


}


#endif
