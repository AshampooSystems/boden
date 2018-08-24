#ifndef BDN_WIN32_ThreadLocalStorageManager_H_
#define BDN_WIN32_ThreadLocalStorageManager_H_

#include <bdn/win32/win32Error.h>

#include <unordered_map>

#include <Windows.h>

namespace bdn
{
namespace win32
{

/** Manager for win32 thread local storage.

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
        
		// we use FlsAlloc instead of TlsAlloc. It has the advantage that we can
		// pass a cleanup callback. When no fibers are used then it behaves exactly like
		// TlsAlloc (plus the cleanup). When fibers are used then the storage will be fiber-local,
		// which is an added benefit.
        _key = ::FlsAlloc(&ThreadLocalStorageManager::_threadExitCleanup);
        if(_key == FLS_OUT_OF_INDEXES)
        {
            BDN_WIN32_throwLastError( ErrorFields().add("func", "FlsAlloc")
                                        .add("action", "ThreadLocalStorageManager constructor") );
        }
    }
    
    
    ~ThreadLocalStorageManager()
    {
        ::FlsFree(_key);
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
        std::unordered_map<int, ValueHolder >* pMap = static_cast< std::unordered_map<int, ValueHolder >* >( ::FlsGetValue(_key) );
        
        if(pMap==nullptr)
        {
            pMap = ::new std::unordered_map<int, ValueHolder >;
            
            if(!::FlsSetValue(_key, pMap ))            
            {
                BDN_WIN32_throwLastError( 
                              ErrorFields() .add("func", "FlsSetValue")
                                            .add("action", "ThreadLocalStorageManager::getThreadValueHolder") );
                
            }
        }
        
        return (*pMap)[slot];
    }
    
    
	/** Returns a reference to a global local storage manager instance.*/
    static ThreadLocalStorageManager& get();


protected:
    static void WINAPI _threadExitCleanup(PVOID pValue)
    {
        std::unordered_map<int, ValueHolder >* pMap = static_cast< std::unordered_map<int, ValueHolder >* >(pValue);
        
        if(pMap!=nullptr)
            delete pMap;
    }
    

	DWORD				_key;
    std::atomic<int>    _nextSlotId;
};

}
}


#endif
