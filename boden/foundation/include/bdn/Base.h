#ifndef BDN_Base_H_
#define BDN_Base_H_

#include <bdn/IBase.h>
#include <bdn/IWeakReferencable.h>

#if !BDN_PLATFORM_DOTNET
#include <atomic>
#endif

namespace bdn
{


/** Base class for most other classes. Provides an implementation
	for IBase.
	*/
class Base : BDN_IMPLEMENTS IWeakReferencable
{
public:
	Base()
        : _weakReferenceState(nullptr)
	{
		_refCount = 1;
	}

	Base(const Base& o)
        : _weakReferenceState(nullptr)
	{
		// copy nothing. This constructor only exists
		// to ensure that the reference count is not
		// copied.
		_refCount = 1;
	}

	virtual ~Base();

    /** Increases the object's reference count by one. This should normally NOT be called
		directly. Instead you should use #P smart pointers, which take care of this
		automatically.
		
		addRef implementations MUST be thread-safe.
    
        If this is overridden in a derived class then the override MUST call the base
        implementation to ensure that reference counting and weak pointers
        work correctly.
    */
	void addRef() const override
	{
#if BDN_PLATFORM_DOTNET
		System::Threading::Interlocked::Increment(_refCount);
#else
		_refCount++;
#endif
	}


    /** Decreases the object's reference count by one. When it reaches 0 then
		the object will delete itself.
		
		releaseRef implementations MUST be thread-safe.
    
        If this is overridden in a derived class then the override MUST call the base
        implementation to ensure that reference counting and weak pointers
        work correctly.
    */
	void releaseRef() const override
	{
#if BDN_PLATFORM_DOTNET
		// cannot use std::atomic. So we used Interlocked instead.		
		if(System::Threading::Interlocked::Decrement(_refCount)==0)
#else
		if(_refCount.fetch_sub(1)==1)
#endif
		{
			// the reference count has reached 0.

			const_cast<Base*>(this)->_refCountReachedZero();
		}

	}


	/** Returns the current reference count. Note that in a multi-threaded program it might
		be changed at any time from a different thread, even before this function returns.*/
	int getRefCount() const
	{
		return _refCount;
	}


	/** Returns true if the object is currently being deleted because the reference count had reached zero.
	 *
	 *  This function does NOT return true if the object is being destructed in other ways. For example,
	 *  the function will will return false if the object was allocated on the stack as a local variable
	 *  (instead of with newObj) and the corresponding function exits.*/
	bool isBeingDeletedBecauseReferenceCountReachedZero() const
	{
		return _refCount<0;
	}


	/** A helper enumeration. This is only used if the raw new operator
		needs to be called.*/
	enum class RawNew
	{
		/** Helper value for the raw new operator.*/
		Use
	};


	/** Behaves like the original 'plain' C++ new operator and returns a plain pointer instead of a smart pointer #P.
		
		This is provided for cases in which the caller does not want to use #newObj to create an
		object, but instead wants to use the plain new operator. Since that plain new operator has been hidden
		(made protected) for safety reasons, it cannot be called anymore. This special "RawNew" new operator offers
		a way to circumvent this.

		Use it like this:
		
		new(Base::RawNew::Use) MyClass;

		*/
	static inline void* operator new(size_t size, RawNew)
	{
		return operator new(size);
	}


	/** Raw delete operator - see corresponding operator new for an explanation. */
	inline void operator delete(void* p, RawNew)
	{
		operator delete(p);
	}
 
	inline void operator delete(void* p)
	{
		::operator delete(p);
	}


	/** Assignment operator. Does nothing - it only exists to ensure that
		the internal reference counter is not copied.*/
	Base& operator=(const Base& o)
	{
		// do nothing. This operator only exists to ensure that the reference count and
        // weak reference state is not copied.
		return *this;
	}

    
    P<IWeakReferenceState> getWeakReferenceState() override;

protected:

	enum
	{
		/** An internal value that is subtracted from the internal reference count
			when object deletion is in progress. Should normally not be used directly.*/
		_deleteThisRefCountDelta = 0x40000000
	};

	// prevent direct access to the normal new operator.
	// Users should use newObj.
	static inline void* operator new(size_t size)
	{
		return ::operator new(size);
	}
    


    /** Called when the object wants to delete itself (when the reference count
		reached 0). This can be overloaded to do custom cleanup or prevent normal
        deletion with the delete operator.

        Note that when deleteThis is called at a point in time when all existing weak pointers
        (see WeakP) to the object have already been invalidated / set to null. So the object is
        actually already considered "dead" at this point.

		deleteThis can also "revive" the object by calling #reviveDuringDeleteThis.
		This aborts the deletion process and returns a pointer with a new reference to the
		object. But since weak pointers were already invalidated, they will stay null and
        can NOT be used to access the revived object.

		Possible use cases for reviving are when the object is added to a 'recycle list' of free objects
		to be used again later.		
		*/
    virtual void deleteThis()
    {
        delete this;
    }
	
	/** This can be called during the execution of deleteThis() to "revive" the object. This means
        that the object is not actually deleted and a new strong reference to the object is returned.
        The caller must store this returned pointer/reference, otherwise the object is deleted again
        immediately when the returned pointer object is destroyed.

        Note that if weak pointers to the object existed then they have already been invalidated before
        deleteThis was called. So even if the object is revived then the weak pointers will still be null.
		*/
	P<IBase> reviveDuringDeleteThis()
	{
		_refCount += _deleteThisRefCountDelta + 1;

		P<IBase> pRef;
		pRef.attachPtr(this);
		return pRef;
	}

private:

    void _refCountReachedZero();



#if BDN_PLATFORM_DOTNET
	mutable int _refCount;
#else
	mutable volatile std::atomic<int> _refCount;
#endif

    struct WeakReferenceState_;
    friend struct WeakReferenceState_;

    std::atomic<WeakReferenceState_*> _weakReferenceState;
};
    
    
}


#endif

