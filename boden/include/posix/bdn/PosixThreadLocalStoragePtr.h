#ifndef BDN_PosixThreadLocalStoragePtr_H_
#define BDN_PosixThreadLocalStoragePtr_H_

#include <bdn/PosixThreadLocalStorageManager.h>

namespace bdn
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
class PosixThreadLocalStoragePtr
{
public:
    PosixThreadLocalStoragePtr()
    {
        _slotId = PosixThreadLocalStorageManager::get()->createSlot();
    }
    
    
    PosixThreadLocalStoragePtr(ObjectType* p)
    : PosixThreadLocalStoragePtr()
    {
        assign(p);
    }
    
    template<class F>
    PosixThreadLocalStoragePtr(P<F> p)
    : PosixThreadLocalStoragePtr()
    {
        assign(p);
    }
    
    
    ~PosixThreadLocalStoragePtr()
    {
        // nothing to clean up. The thread local stuff is released when
        // the thread exits.
    }
    
    
    void assign(ObjectType* pObj)
    {
        P<IBase>& pPtrRef = PosixThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        pPtrRef.assign(pObj);
    }
    
    template<class F>
    void assign(P<F>&& pObj)
    {
        P<IBase>& pPtrRef = PosixThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        pPtrRef.assign(pObj);
    }
    
    
    PosixThreadLocalStoragePtr<ObjectType>& operator=(ObjectType* pObj)
    {
        assign(pObj);
        return *this;
    }
    
    
    PosixThreadLocalStoragePtr<ObjectType>& operator=(const P<ObjectType>& pObj)
    {
        assign(pObj.getPtr());
        return *this;
    }
    
    
    template<class O>
    inline PosixThreadLocalStoragePtr<ObjectType>& operator=(const P<O>& pObj)
    {
        assign(pObj.getPtr());
        return *this;
    }
    
    
    ObjectType* detachPtr()
    {
        P<IBase>& pPtrRef = PosixThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        static_cast<ObjectType*>( pPtrRef.detachPtr() );
    }
    
    
    
    PosixThreadLocalStoragePtr<ObjectType>& attachPtr(ObjectType* pObj)
    {
        P<IBase>& pPtrRef = PosixThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
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
    
    bool operator==(const PosixThreadLocalStoragePtr<ObjectType>& p) const
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
    
    bool operator!=(const PosixThreadLocalStoragePtr<ObjectType>& p) const
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
        P<IBase>& pPtrRef = PosixThreadLocalStorageManager::get()->getThreadPtrRef(_slotId);
        
        return cast<ObjectType>(pPtrRef.getPtr());
    }
    
protected:

    int _slotId;
};
    
}


#endif
