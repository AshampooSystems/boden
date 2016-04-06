#ifndef BDN_Base_H_
#define BDN_Base_H_

#include <bdn/IBase.h>

#if !BDN_PLATFORM_DOTNET
#include <atomic>
#endif

namespace bdn
{

/** Base class for most other classes. Provides an implementation
	for IBase.
	*/
class Base : BDN_IMPLEMENTS IBase
{
public:
	Base()
	{
		_refCount = 1;
	}

	Base(const Base& o)
	{
		// copy nothing. This constructor only exists
		// to ensure that the reference count is not
		// copied.
		_refCount = 1;
	}

	virtual ~Base()
	{
	}


	void addRef() const override
	{
#if BDN_PLATFORM_DOTNET
		System::Threading::Interlocked::Increment(_refCount);
#else
		_refCount++;
#endif
	}

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

			// Set the refcount to a very small number. This is for situations where there are
			// addRef/releaseRef calls in the object's destructor. If we don't do this then
			// those calls could increase the ref count above 0 and then back to 0. That would
			// cause the object to be deleted a second time when it reach 0
			// again (causing a crash).
			_refCount -= _deleteThisRefCountDelta;

			const_cast<Base*>(this)->deleteThis();
		}

	}


	/** Returns the current reference count. Note that in a multi-threaded program it might
		be changed at any time from a different thread, even before this function returns.*/
	int getRefCount() const
	{
		return _refCount;
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
	inline void* operator new(size_t size, RawNew)
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
		// do nothing. This operator only exists to ensure that the reference count is not
		// copied.
		return *this;
	}

protected:

	enum
	{
		/** An internal value that is subtracted from the internal reference count
			when object deletion is in progress. Should normally not be used directly.*/
		_deleteThisRefCountDelta = 0x40000000
	};

	// prevent direct access to the normal new operator.
	// Users should use newObj.
	inline void* operator new(size_t size)
	{
		return ::operator new(size);
	}

	
	/** Called when the object wants to delete itself (when the reference count
		reaches 0). This can be overloaded to prevent the deletion from happening.

		deleteThis can "revive" the object by calling #cancelDeleteThisAndReturnNewReference.
		This aborts the deletion process and returns a pointer with a new reference to the
		object.

		Possible use cases are when the object is added to a 'recycle list' of free objects
		to be used again later.		
		*/
	virtual void deleteThis()
	{
		delete this;
	}


	/** This can be called from deleteThis to cancel the deletion. A new reference is added
		and a pointer with that reference is returned. The caller must store this pointer/reference,
		otherwise the object is deleted again immediately when the returned pointer object is destroyed.
		*/
	P<IBase> cancelDeleteThisAndReturnNewReference()
	{
		_refCount += _deleteThisRefCountDelta + 1;

		P<IBase> pRef;
		pRef.attach(this);
		return pRef;
	}

#if BDN_PLATFORM_DOTNET
	mutable int _refCount;
#else
	mutable volatile std::atomic<int> _refCount;
#endif
};
    
    
}


#endif


