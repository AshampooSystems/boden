#ifndef BDN_WeakP_H_
#define BDN_WeakP_H_

#include <bdn/IWeakReferenceState.h>

namespace bdn
{


/** A weak pointer to an object that implements IBase. Weak pointers do not prevent the object to be deleted.

    IMPORTANT: It can be dangerous to create weak pointers to objects that were NOT allocated with newObj. That can
    lead to crashes and should be avoided.
    
    Weak pointers cannot be used to access the object directly because the object they point to can be deleted at any time.
    To access the object, a (possibly temporary) strong pointer must be created first (see toStrong() ).
    The strong pointer will be null if the object was already deleted. If it is not null then it can be used
    to access the object.
    
    Example:

    \code

    P< String >     pOriginal = newObj<String>("hello");
    WeakP< String > weakPtr = pOriginal;

    // A strong pointer has to be created from the weak pointer to access the object
    P< String >     pTempStrong = weakPtr.toStrong();
    
    // pStrong2 will be valid (not null), because the object still exists at this point.
    // Since we know that here, we can access the object directly.
    // Nevertheless, it is good practice to always check that the resulting pointer is really
    // valid.
    if(pTempStrong == nullptr)
    {
        // not what we expected. We have a bug.
        throw std::logic_error("Object unexpectedly deleted");
    }

    assert(pTempStrong != nullptr );
    (*pTempStrong) = "world";

    // after we are done, accessing the object, we have to release the temporary
    // strong reference, since it keeps the object alive.
    // Releasing the strong reference can be done by setting the strong pointer to nullptr,
    // or by ensuring that the strong pointer itself is destroyed.
    pTempStrong = nullptr;

    // at this point the only strong reference to the object is pOriginal.
    // We release that now. This will cause the object to be deleted.
    pOriginal = nullptr;

    // When we now try to get a strong pointer from the weak pointer, we will get null.
    P< String >     pTempStrong2 = weakPtr.toStrong();

    if(pTempStrong2==nullptr)
    {
        // this is what we expect to happen, since the object is now deleted.
        ...
    }

    \endcode
	*/
template<class T>
class WeakP
{
public:
	WeakP()
	{
	}

	WeakP(std::nullptr_t)
	{
	}

    WeakP(T* pObject)
    {        
        // note that we do not store a reference to the object itself.
        // Instead we only store a pointer to a shared data object that
        // all weak references share. This shared object also contains
        // a pointer to the object.
        if(pObject!=nullptr)
            _pState = pObject->getWeakReferenceState();
    }

    WeakP(const WeakP& p)
		: _pState( p._pState )
	{
	}

	WeakP(WeakP&& p)
        : _pState( std::move(p._pState) )
	{
	}
	
	template<class F>
	inline WeakP(const WeakP<F>& p)
		: _pState( p._getWeakReferenceState() )
	{
        // make sure that F is actually a compatible type
        T* dummy = (F*)nullptr;
        (void)dummy;    // prevent unused warning
	}
    
    template<class F>
	inline WeakP(const P<F>& p)
		: WeakP( p.getPtr() )
	{
        // make sure that F is actually a compatible type
        T* dummy = (F*)nullptr;
        (void)dummy;    // prevent unused warning
	}



    WeakP& operator=(std::nullptr_t)
	{
        _pState = nullptr;

        return *this;
	}

    WeakP& operator=(T* pObject)
    {        
        if(pObject!=nullptr)
            _pState = pObject->getWeakReferenceState();
        else
            _pState = nullptr;

        return *this;
    }

    WeakP& operator=(const WeakP& o)		
	{
        _pState = o._pState;

        return *this;
	}

	WeakP& operator=(WeakP&& o)        
	{
        _pState = std::move(o._pState);
        
        return *this;
    }
	
	template<class F>
	WeakP& operator=(const WeakP<F>& o)	
	{
        _pState = o._getWeakReferenceState();

        // make sure that F is actually a compatible type
        T* dummy = (F*)nullptr;
        (void)dummy;    // prevent unused warning

        return *this;
	}


    template<class F>
	WeakP& operator=(const P<F>& o)		
	{
        return operator=( o.getPtr() );
	}

    

	P<T> toStrong() const
    {
        if(_pState==nullptr)
        {
            // if _pShared is null then that means that we never had a reference
            // to an actual object. So we know that the result is always null.
            return nullptr;
        }
        else
        {
            return cast<T>( _pState->newStrongReference() );
        }
    }



    /** For internal use only - do not call.*/
    P<IWeakReferenceState> _getWeakReferenceState() const
    {
        return _pState;
    }
    
private:    
    P<IWeakReferenceState> _pState;
};


}


#endif

