#include <bdn/init.h>
#include <bdn/Base.h>


namespace bdn
{

struct Base::WeakReferenceState_  : public Base, BDN_IMPLEMENTS IWeakReferenceState
{
public:
    WeakReferenceState_(Base* pObject)
        : pObject(pObject)
    {
    }

    P<IBase> newStrongReference() override
    {
        Mutex::Lock lock(mutex);

        // _pObject will be null if the object was already deleted.
        // That is exactly what we want.

        // However, there is another edge case here that we need to handle.
        // The reference count might just have reached zero for the object
        // in another thread. In that case Base::_refCountReachedZero may be just
        // starting to execute, but it may not have updated pObject yet.
        // In these cases it is imperative that we do not add create a new strong
        // reference here, since the destruction has begun.

        if(pObject!=nullptr)
        {
            // increase the object's reference count again
            int refCountBefore = pObject->_refCount.fetch_add(1);

            if(refCountBefore<=0)
            {
                // the refcount had already reached zero. So we have the case mentioned above, that
                // Base::refCountReachedZero has started, but has not yet set pObject to null.
                // Undo the refcount modification.
                _refCount -= 1;

                // Note that no one else can have modified the reference count. Other weak references
                // to the same object cannot enter this code block since we are holding the mutex.
                // And strong references do not exist (ref count is zero).
                
                // So we know that our temporary increase and decrease cannot have caused bad effects
                // on some other thread.

                // return null since the object is already in the process of being destroyed.
                return nullptr;
            }
            else
            {
                // we have a small problem here. Base::addRef may have been overridden. The override
                // may do additional stuff - and the derived class will expect it to be called any
                // time a reference is added.
                // So we SHOULD call addRef here to be consistent with normal strong references.
                // So we do that and afterwards we decrement the reference count again (to undo the additional
                // increment).
                // Note that the initial direct ref count incrementation we do above is necessary to ensure
                // that we do not increment a reference count that is zero. We cannot do that just by calling
                // addRef.
                
                P<IBase> pResult(pObject);

                pObject->_refCount -= 1;

                return pResult;
            }
        }
        else
            return nullptr;
    }

    Mutex mutex;
    Base* pObject;
};


Base::~Base()
{
    WeakReferenceState_* pWeakReferenceState = _weakReferenceState.load();
    if(pWeakReferenceState!=nullptr)
    {
        // this should never happen. Weak references should ONLY be created
        // to objects that were allocated with newObj. And for those refCountReachedZero
        // is called when the last reference is released. Which in turn will set
        // _weakReferenceState to null before the object is actually deleted.

        // So if we get here then someone probably created the Base object on the stack
        // or as a normal member object and then created weak references to it.
        // And that should never be done.

        // Why should this never be done? Could we not simply mark the object
        // as deleted here in the destructor, the same way we do in refCountReachedZero?
        // Yes, we could, but we have to keep in mind that weak references exist for the purpose
        // of creating temporary strong references. And while these strong references exist
        // the object must be guaranteed to be kept alive. For a stack object the strong references
        // do not keep the object alive, thus crashes may occur.
        
        // However, this is completely analogous to directly creating strong pointers to the stack object.
        // Those also cannot keep the stack object alive, thus they might cause a crash. So this is essentially
        // the same case as with the weak pointers, except that for weak pointers we can see that
        // a weak pointer was created at some point and we cannot see that for strong pointers.

        // Despite all that, it may sometimes be useful to create a weak pointer to an object when
        // one knows that it will not be accessed before the object is destructed.
        // So we allow it, even though it is not recommended.

        // In any case, we have to mark the object as deleted here.
        {
            Mutex::Lock lock( pWeakReferenceState->mutex );
            pWeakReferenceState->pObject = nullptr;
        }

        // and release our reference to the shared state
        pWeakReferenceState->releaseRef();
	}
}

void Base::_refCountReachedZero()
{
    // the reference count has reached zero. That means that no more strong references exist.
    // BUT, weak references may still exist.

    // It might seem that a weak reference could create a new strong reference in another thread
    // while we execute here. BUT the weak reference implementation makes sure that no new
    // references can be created when the reference count has reached zero. So no new strong
    // references can be created.

    // Because of this it also means that the destruction of the object has already "officially" begun
    // when refCountReachedZero is called. Since weak references already consider the object gone,
    // we should not do anything here that aborts the deinitialization of the object. One exception to
    // this is the freedom we give deleteThis to prevent the actual deletion of the object and the release
    // of its memory. However, the semantics of this are that the original object is still considered to be gone,
    // but deleteThis can prevent the memory release to support efficient recycling of object resources. So whatever
    // the deleteThis function does, the result is always considered to be a new
    // object, even if it is at the same memory location.

    // Note that if _weakReferenceState is null here then we know that we have never had any weak references. We also know
    // we currently do not have any strong references. So if the _weakReferenceState is null then we know that there are
    // no references AT ALL to this object.

    WeakReferenceState_* pWeakRefState = _weakReferenceState.load();
    if(pWeakRefState!=nullptr)
    {
        // we did have weak references at some point. So we have to set the pointer
        // to ourselves in the shared state to null now, before we continue deleting
        // ourselves. We must hold a mutex during this.
        {
            Mutex::Lock lock( pWeakRefState->mutex );
            
            // at this point we know that we will be deleted. Set the pointer to ourselves in the
            // shared weak reference state to null.
            pWeakRefState->pObject = nullptr;

            // we can release the mutex now. pObject is null
        }

        // release our reference to the shared data. If there are weak pointers to us then they
        // will keep the shared data alive until they are gone.
        pWeakRefState->releaseRef();

        // also set the member pointer to the shared data to null. This should normally not be necessary,
        // but if the object is revived instead of actually deleted then we must leave
        // a clean state behind.
        _weakReferenceState = nullptr;
    }

    // Now we actually delete ourselves.

    // Set the refcount to a very small number. This is for situations where there are
	// addRef/releaseRef calls in the object's destructor. If we don't do this then
	// those calls could increase the ref count above 0 and then back to 0. That would
	// cause the object to be deleted a second time when it reach 0
	// again (causing a crash).
	_refCount -= _deleteThisRefCountDelta;

    deleteThis();        
}


P<IWeakReferenceState> Base::getWeakReferenceState()
{
    WeakReferenceState_* pData = _weakReferenceState.load();
        
    if(pData==nullptr)
    {
        P<WeakReferenceState_> pNewData = newObj<WeakReferenceState_>(this);

        WeakReferenceState_* pExpected = nullptr;
        
        if(_weakReferenceState.compare_exchange_strong(pExpected, pNewData.getPtr() ))
        {
            // successfully stored the pointer. Note that _weakReferenceState does not
            // call addRef/releaseRef automatically.
            // But pNewData has already addRef'ed. So we simply prevent pNewData from
            // releaseRef'ing by detaching the pointer from it. Conceptually this
            // reference is now owned by _weakReferenceState from now on.
            pData = pNewData.detachPtr();
        }
        else
        {
            // someone else already allocated the weak pointer data in the meantime.
            // Throw ours away.
            pData = _weakReferenceState.load();
        }
    }

    return pData;
}


}


