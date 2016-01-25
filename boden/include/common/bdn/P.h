#ifndef BDN_P_H_
#define BDN_P_H_


namespace bdn
{

/** A smart pointer class that automatically deletes object when they are not needed anymore.
	
	This is implemented via reference counting with objects that implement #IBase
	(i.e. which most classes in this library implement).

	Note that in contrast to std::shared_ptr it is safe to mix and match P pointers with
	plain pointers. For example, the following is safe and results in the expected behaviour.

	\code
	P<SomeClass> pA = newObj<SomeClass>();
	SomeClass* pPlainA = pA;
	P<SomeClass pB = pPlainA;
	\endcode

	The only thing you need to keep in mind when using #P pointers is to avoid circular references.
	If an object A holds a pointer to an object B and B also holds a pointer to A then both objects
	will keep each other alive indefinitely and will never be deleted.
	If you need such circular references then one of them should be either a plain (non-smart) pointer
	or a weak pointer (see #WeakP).
	*/
template<class T>
class P
{
public:
	P()
		: _pObject(nullptr)
	{
	}


	P(const P& p)
		: P( p._pObject )
	{
	}
	
	template<class F>
	inline P(const P<F>& p)
		: P( p._getPtr() )
	{
	}

	P(T* p)
		: _pObject(p)
	{
		if(_pObject != nullptr)
			_pObject->addRef();
	}
	
	~P()
	{
		if(_pObject != nullptr)
			_pObject->releaseRef();
	}


	/** Assigns a pointer to an object to the smart pointer and increases
		the object's reference count.

		If the smart pointer already held a reference to another object then
		that reference is released.*/
	void assign(T* pObj)
	{
		T* pMyOld = _pObject;

		// it is important that we increase the reference count on the new object first.
		// Otherwise the following could cause the object to be deleted by mistake:
		// P<SomeClass> p = ...;
		// p = p;
		
		_pObject = pObj;
		if(_pObject != nullptr)
			_pObject->addRef();

		if(pMyOld != nullptr)
			pMyOld->releaseRef();
	}

	P<T>& operator=(T* pObj)
	{
		assign(pObj);
		return *this;
	}


	P<T>& operator=(const P<T>& pObj)
	{
		assign(pObj.getPtr());
		return *this;
	}


	template<class O>
	inline P<T>& operator=(const P<O>& pObj)
	{
		assign(pObj.getPtr());
		return *this;
	}


	/** Detaches the object without releasing the reference (without decrementing
		its reference counter).

		Afterwards the smart pointer will be null.

		Returns a plain pointer to the object that the smart pointer pointed to.
		*/
	T* detach()
	{
		T* pObj = _pObject;

		_pObject = nullptr;

		return pObj;
	}


	/** Sets the smart pointer to point to an object without incrementing its reference count.
		
		The object's reference count will be released as normal when the smart pointer is deleted
		or gets another pointer assigned.
	*/
	P& attach(T* pObj)
	{
		if(_pObject != nullptr)
			_pObject->releaseRef();

		_pObject = pObj;

		return *this;
	}


	/** Conversion operator to a plain pointer.*/
	operator T*() const
	{
		return _pObject;
	}


	/** Allows direct access to the members that this pointer points to.*/
	T* operator->() const
	{
		return _pObject;
	}


	bool operator==(const P<T>& p) const
	{
		return _pObject == p._pObject;
	}


	bool operator==(const T* p) const
	{
		return _pObject==p;
	}


	bool operator!=(const P<T>& pObj) const
	{
		return _pObject != pObj._pObject;
	}


	bool operator!=(const T* pObj) const
	{
		return _pObject != pObj;
	}


	/** Returns the value of the smart pointer as a plain pointer.*/
	T* getPtr() const
	{
		return _pObject;
	}


protected:
	T* _pObject;
};


}


#endif

