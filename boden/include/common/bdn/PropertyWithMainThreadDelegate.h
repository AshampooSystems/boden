#ifndef BDN_PropertyWithMainThreadDelegate_H_
#define BDN_PropertyWithMainThreadDelegate_H_

#include <bdn/RequireNewAlloc.h>
#include <bdn/Property.h>
#include <bdn/mainThread.h>

namespace bdn
{

/** A property implementation helper that redirects accesses to the property value
	to the main thread and then executes the accesses via a custom delegate object.
	
	The inner delegate object will only be called from the main thread.

	The property object itself can be used normally from any thread.

	Note that PropertyWithMainThreadDelegate objects MUST be allocated with newObj.	
	
	*/	
template<class ValType>
class PropertyWithMainThreadDelegate : public RequireNewAlloc< Property<ValType>, PropertyWithMainThreadDelegate<ValType> >
{
public:
	class IDelegate : BDN_IMPLEMENTS IBase
	{
	public:

		virtual void	set(const ValType& val)=0;
		virtual ValType get() const=0;

	};

	PropertyWithMainThreadDelegate(IDelegate* pDelegate)
	{
		_pDelegate = pDelegate;

		updateCachedValue();
	}
	

	/** Detaches the delegate object. The delegate object will not be accessed anymore
		by the property after dispose() returns.

		The property keeps its current value and can be read and changed as normal afterwards.
		But the	delegate will not be affected.
		*/
	void detachDelegate()
	{
		MutexLock lock(_mutex);

		_pDelegate = nullptr;			
	}


	void set(const ValType& val) override
	{
		bool valueChanged = false;

		{
			MutexLock lock(_mutex);

			// if there are pending fetch operations then we invalidate those.
			_firstValidFetchId = _nextFetchId;
			
			if(val!=_value)
			{
				_value = val;
				valueChanged = true;

				// schedule the delegate to be updated

				scheduleOp(_nextSetId, _firstValidSetId,
					[val](PropertyWithMainThreadDelegate* pThis)
					{
						pThis->_pDelegate->set(val);
					} );
			}
		}

		if(valueChanged)
			_onChange.notify(*this);
	}

	ValType get() const override
	{
		MutexLock lock(_mutex);
		return _value;
	}


	Notifier< const ReadProperty<ValType>& >& onChange() override
	{
		return _onChange;
	}



	Property<ValType>& operator=(const ValType& val) override
	{
		set(val);

		return *this;
	}

	Property<ValType>& operator=(const ReadProperty<ValType>& val) override
	{
		set(val);

		return *this;
	}

	
	void bind(ReadProperty<ValType>& sourceProperty) override
	{
        _pBindSourceSubscription = sourceProperty.onChange().template subscribeMember<PropertyWithMainThreadDelegate>(this, &PropertyWithMainThreadDelegate::bindSourceChanged);
        
        bindSourceChanged(sourceProperty);
    }


	/** Tells the property to fetch the current value from the delegate object.
	
		If updateCachedValue is called from the main thread then the update happens immediately.
		
		If updateCachedValue is called from another thread then the update is scheduled and will be
		performed at some point in the future (when the main thread message loop executes pending events).
		In that case, updateCachedValue will return immediately and the value
		of the property will remain unchanged for a short time, until the scheduled update is actually
		executed.
		If the property is manually set to a new value while such a delayed fetch operation is pending then 
		then the pending fetch is invalidated and the property will keep the manually set new value.
		The delegate will also be updated to this new value.
	*/
	void updateCachedValue()
	{
		{
			// we want to get the current value of the delegate and set it as the value
			// of the property.
			// Invalidate all pending delegate set operations. We do not want the delegate to be changed
			// because of old pending writes. Instead we want to use the delegate's CURRENT
			// value as that of the property.
			MutexLock lock(_mutex);

			_firstValidSetId = _nextSetId;
		}

		scheduleOp( _nextFetchId, _firstValidFetchId,
			[](PropertyWithMainThreadDelegate* pThis)
			{
				// note that the mutex is locked when this is called.
				// We also know that the property has not been disposed yet, otherwise
				// this function would not be called.
					
				ValType newValue = pThis->_pDelegate->get();

				if(newValue != pThis->_value)
				{
					pThis->_value = newValue;

					{
						MutexUnlock unlock(pThis->_mutex);

						pThis->_onChange.notify(*pThis);
					}
				}
			} );
	}


private:
	virtual void bindSourceChanged(const ReadProperty<ValType>& prop)
    {
        set( prop.get() );
    }

	
	void scheduleOp(int64_t& nextOpId, int64_t& firstValidOpId, std::function<void(PropertyWithMainThreadDelegate*)> updateFunc)
	{
		// keep ourselves alive until the op is done. Note that we inherit from
		// RequireNewAlloc, so we can do this.
		P<PropertyWithMainThreadDelegate> pThis = this;
			
		int64_t		opId;

		{
			MutexLock lock(_mutex);

			opId = nextOpId;
			nextOpId++;
		}

		callFromMainThread(
			[pThis, opId, updateFunc, &firstValidOpId]()
			{
				MutexLock lock(pThis->_mutex);

				if(opId >= firstValidOpId
					&& pThis->_pDelegate!=nullptr )
				{
					// invalidate all earlier pending updates. It does not make sense
					// to execute them
					firstValidOpId = opId+1;
					updateFunc(pThis);
				}
			} );			
	}


	ValType			_value;

	mutable Mutex	_mutex;
	
	int64_t			_nextSetId = 0;
	int64_t			_firstValidSetId = 0;

	int64_t			_nextFetchId = 0;
	int64_t			_firstValidFetchId = 0;

	P<IDelegate>	_pDelegate;
	
	Notifier< const ReadProperty<ValType>& >	_onChange;

	P<IBase>									_pBindSourceSubscription;
};

}


#endif

