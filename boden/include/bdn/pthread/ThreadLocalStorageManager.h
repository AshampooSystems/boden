#ifndef BDN_PTHREAD_ThreadLocalStorageManager_H_
#define BDN_PTHREAD_ThreadLocalStorageManager_H_

#include <bdn/errno.h>

#include <unordered_map>

#include <pthread.h>

namespace bdn
{
namespace pthread
{

/** Manager for Posix-style thread local storage.

    IMPORTANT: It is rarely advisable to use this directly. Instead
    you should use the macros #BDN_SAFE_STATIC and #BDN_SAFE_STATIC_THREAD_LOCAL_IMPL to create thread-local
    objects.

	*/
class ThreadLocalStorageManager : public Base
{
public:

    struct ValueHolder
    {
        ValueHolder()
        {
            _pValue = nullptr;
            _valueDeleteFunc = nullptr;
        }
        
        ~ValueHolder()
        {
            _deleteValue();
        }
        
        void* getValue()
        {
            return _pValue;
        }
        
        void assignValue(void* pValue, void (*valueDeleteFunc)(void*) )
        {
            _deleteValue();
            _pValue = pValue;
            _valueDeleteFunc = valueDeleteFunc;
        }
        
    protected:
        void _deleteValue()
        {
            if(_pValue!=nullptr && _valueDeleteFunc!=nullptr)
                _valueDeleteFunc(_pValue);
            
            _pValue = nullptr;
            _valueDeleteFunc = nullptr;
        }
        
        void* _pValue;
        void (*_valueDeleteFunc)(void*);
    };

	ThreadLocalStorageManager()
    {
        _nextSlotId=0;
        
        int result = pthread_key_create(&_key, &ThreadLocalStorageManager::_threadExitCleanup);
        if(result!=0)
        {
            throw errnoToSystemError( result,
                           ErrorFields().add("func", "pthread_key_create")
                                        .add("action", "PosixThreadLocalStorageManager constructor") );
        }
    }
    
    
    ~ThreadLocalStorageManager()
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
    ValueHolder&	getThreadValueHolder(int slot)
    {
        std::unordered_map<int, ValueHolder >* pMap = static_cast< std::unordered_map<int, ValueHolder >* >( pthread_getspecific(_key) );
        
        if(pMap==nullptr)
        {
            pMap = ::new std::unordered_map<int, ValueHolder >;
            
            int result = pthread_setspecific(_key, pMap );
            if(result!=0)
            {
                throw errnoToSystemError( result,
                              ErrorFields() .add("func", "pthread_setspecific")
                                            .add("action", "PosixThreadLocalStorageManager::getThreadPtrRef") );
                
            }
        }
        
        return (*pMap)[slot];
    }
    
    
    static ThreadLocalStorageManager& get();
    

protected:
    static void _threadExitCleanup(void* pValue)
    {
        std::unordered_map<int, ValueHolder >* pMap = static_cast< std::unordered_map<int, ValueHolder >* >(pValue);
        
        if(pMap!=nullptr)
            delete pMap;
    }
    

	pthread_key_t       _key;
    std::atomic<int>    _nextSlotId;
};

}
}


#endif
