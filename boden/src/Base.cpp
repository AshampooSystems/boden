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

    P<IBase> newStrongReference()
    {
        MutexLock lock(mutex);

        // _pObject will be null if the object was already deleted.
        // That is exactly what we want.

        return pObject;
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
        // Yes, we could, but we have to keep in mind that new strong references can be
        // created at any point before we mark the object as deleted. This causes two problems:

        // 1) This object is probably actually a subclass of Base. When the Base destructor
        //    is called then the sub-class destructors have already been executed. So at this
        //    point the object is actually already half-destructed. If someone creates a new
        //    strong reference and accesses the object at any point between the time the initial
        //    sub-class destructor was called and the time we mark the object as deleted then
        //    the access uses a half-destroyed object. That can cause crashes and other bad stuff.

        // 2) When someone creates a new strong reference after the destruction was initiated then
        //    the caller expects the object to stay alive while he holds the reference.
        //    But the destruction cannot be stopped or aborted and the object will be deleted.

        // nevertheless, it may sometimes be useful to create a weak pointer to an object when
        // one knows that it will not be accessed while destruction is in progress.
        // So we allow it, even though it is not recommended.

        // In any case, we have to mark the object as deleted here.
        {
            MutexLock lock( pWeakReferenceState->mutex );
            pWeakReferenceState->pObject = nullptr;
        }

        // and release our reference to the shared state
        pWeakReferenceState->releaseRef();
	}
}

void Base::refCountReachedZero()
{
    if(_refCount>0)
    {
        // object has been revived. Do not actually delete it.
        return;
    }
        
    // The object WILL be deleted. Notify our weak references
    // that it will be gone soon.

    // Note that at this point new strong references (and new weak ones from the the strong ones)
    // could still be created from another thread. So we have to expect that.

    // Note that if the header is null then we know that we have never had any weak references. We also know
    // we currently do not have any strong references. So if the header is null then we know that there are
    // no references AT ALL to this object. That means that we do not have to worry about new references
    // being created, since for that we would need at least one weak or strong reference.
    // So there are no concurrency issued if the header is null.

    WeakReferenceState_* pWeakRefState = _weakReferenceState.load();
    if(pWeakRefState!=nullptr)
    {
        // we did have weak references at some point. So we have to set the pointer
        // to ourselves in the shared state to null now, before we continue deleting
        // ourselves. We must hold a mutex during this.
        {
            MutexLock lock( pWeakRefState->mutex );

            // While the mutex is locked no new strong references can be created
            // from the existing weak references.
            // So here we will do another check to see what the current refCount is. If it
            // is still 0 then we know that we can proceed to delete ourselves. Otherwise we abort
            // the process.
            if(_refCount>0)
            {
                // abort deletion - we got a new reference.
                return;
            }

            // at this point we know that we will be deleted. Set the pointer to ourselves in the
            // shared weak reference state to null.
            pWeakRefState->pObject = nullptr;

            // we can release the mutex now. pObject is null, so no new strong references will be created.
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


