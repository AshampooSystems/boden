#ifndef BDN_WIN32_ThreadLocalStorage_H_
#define BDN_WIN32_ThreadLocalStorage_H_

#include <bdn/win32/ThreadLocalStorageManager.h>

namespace bdn
{
namespace win32
{
    
/** Implements thread local storage using the win32 API.
    
    IMPORTANT: It is rarely advisable to use this directly. Instead
    you should use the macros #BDN_SAFE_STATIC and #BDN_SAFE_STATIC_THREAD_LOCAL_IMPL to create thread-local
    objects.
    
    The template parameter ValueType can be any C++ type (class, struct, int, ...)
    that is constructible without parameters.
    
    The value object is allocated with new when it is first accessed
    in a thread. It is deleted with delete when the thread exits.
    
    When the value object is a simple type like int then it is default-initialized.
    For number that means that a new value instance will have the value 0.
 
 */
template<typename ValueType>
class ThreadLocalStorage
{
public:
    ThreadLocalStorage()
    {
        _slotId = ThreadLocalStorageManager::get().createSlot();
    }
    
    
    ~ThreadLocalStorage()
    {
        // nothing to clean up. This object is the same for all threads,
        // and it only channels accesses to the instance for the corresponding
        // thread.
        // The actual thread local value instances are released by ThreadLocalStorageManager
        // when the thread exits.
    }
    
    
    /** Returns a reference to the value for the current thread.
    
		The arguments are optional constructor parameters. If the thread local object
		for the current thread does not yet exist and must be created then these arguments
		are passed to the constructor of the object. They are optional (unless the ValueType
		requires constructor parameters).
		
	*/
	template<class... Arguments>
    ValueType& getValueRef(Arguments... args)
    {
        ThreadLocalStorageManager::ValueHolder& holder = ThreadLocalStorageManager::get().getThreadValueHolder(_slotId);
        
        ValueType* pValue = static_cast<ValueType*>( holder.getValue() );
        if(pValue==nullptr)
        {
            pValue = rawNew<ValueType>( std::forward<Arguments>(args)... );
            
            holder.assignValue(pValue, &ThreadLocalStorage::_deleteValue);
        }
        
        return *pValue;
    }
    
    
    /** Sets the value for the current thread.*/
    template<class F>
    ValueType& operator=(const F& val)
    {
        ValueType& ref = getValueRef();
        
        ref = val;
        
        return ref;
    }
    
    template<class F>
    bool operator==(const F& val)
    {
        return getValueRef() == val;
    }

    template<class F>
    bool operator!=(const F& val)
    {
        return getValueRef() != val;
    }
    
    
    operator ValueType&()
    {
        return getValueRef();
    }
    
    operator const ValueType&()
    {
        return getValueRef();
    }
    
protected:
    static void _deleteValue(void* pValue)
    {
        deleteOrReleaseRef( static_cast<ValueType*>(pValue) );
    }

    int _slotId;
};
    
    
}
}


#endif
