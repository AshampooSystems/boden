#ifndef BDN_PTHREAD_ThreadLocalStoragePtr_H_
#define BDN_PTHREAD_ThreadLocalStoragePtr_H_

#include <bdn/pthread/ThreadLocalStorageManager.h>

namespace bdn
{
namespace pthread
{
    
/** A smart pointer implementation that stores the value in thread local storage.
    This is implemented using the pthreads library (only available on Posix systems).
    
    IMPORTANT: It is rarely advisable to use this directly. Instead
    you should use the macro #BDN_STATIC_THREAD_LOCAL_PTR to create thread-local
    objects.
 
    The objects of a thread will automatically be released
    when the thread exits.
    
    This class supports the same interface as the normal smart pointers
    #P.
 */
template<class ObjectType>
class ThreadLocalStoragePtr
{
public:
    ThreadLocalStoragePtr()
    {
        _slotId = ThreadLocalStorageManager::get()->createSlot();
    }
    
    
    ThreadLocalStoragePtr(ObjectType* p)
    : ThreadLocalStoragePtr()
    {
        assign(p);
    }
    
    template<class F>
    ThreadLocalStoragePtr(P<F> p)
    : ThreadLocalStoragePtr()
    {
        assign(p);
    }
    
    
    ~ThreadLocalStoragePtr()
    {
        // nothing to clean up. The thread local stuff is released when
        // the thread exits.
    }
    
    
    void assign(ObjectType* pObj)
    {
        P<IBase>& pPtrRef = ThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        pPtrRef.assign(pObj);
    }
    
    template<class F>
    void assign(P<F>&& pObj)
    {
        P<IBase>& pPtrRef = ThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        pPtrRef.assign(pObj);
    }
    
    
    ThreadLocalStoragePtr<ObjectType>& operator=(ObjectType* pObj)
    {
        assign(pObj);
        return *this;
    }
    
    
    ThreadLocalStoragePtr<ObjectType>& operator=(const P<ObjectType>& pObj)
    {
        assign(pObj.getPtr());
        return *this;
    }
    
    
    template<class O>
    inline ThreadLocalStoragePtr<ObjectType>& operator=(const P<O>& pObj)
    {
        assign(pObj.getPtr());
        return *this;
    }
    
    
    ObjectType* detachPtr()
    {
        P<IBase>& pPtrRef = ThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        static_cast<ObjectType*>( pPtrRef.detachPtr() );
    }
    
    
    
    ThreadLocalStoragePtr<ObjectType>& attachPtr(ObjectType* pObj)
    {
        P<IBase>& pPtrRef = ThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        pPtrRef.attachPtr(pObj);
        
        return *this;
    }
    
    
    operator ObjectType*() const
    {
        return getPtr();
    }
    
    
    ObjectType* operator->() const
    {
        return getPtr();
    }
    
    bool operator==(const P<ObjectType>& p) const
    {
        return getPtr() == p.getPtr();
    }
    
    bool operator==(const ThreadLocalStoragePtr<ObjectType>& p) const
    {
        return getPtr() == p.getPtr();
    }
    
    bool operator==(const ObjectType* p) const
    {
        return getPtr()==p;
    }
    
    bool operator==(std::nullptr_t) const
    {
        return getPtr()==nullptr;
    }
    
    
    bool operator!=(const P<ObjectType>& pObj) const
    {
        return getPtr() != pObj._pObject;
    }
    
    bool operator!=(const ThreadLocalStoragePtr<ObjectType>& p) const
    {
        return getPtr() != p.getPtr();
    }
    
    
    bool operator!=(const ObjectType* pObj) const
    {
        return getPtr() != pObj;
    }
    
    bool operator!=(std::nullptr_t) const
    {
        return getPtr()!=nullptr;
    }
    
    
    ObjectType* getPtr() const
    {
        P<IBase>& pPtrRef = ThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        return cast<ObjectType>(pPtrRef.getPtr());
    }
    
protected:

    int _slotId;
};
    
    
}
}


#endif
