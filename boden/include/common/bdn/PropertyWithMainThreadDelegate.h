#ifndef BDN_PropertyWithMainThreadDelegate_H_
#define BDN_PropertyWithMainThreadDelegate_H_

#include <bdn/RequireNewAlloc.h>
#include "init.h"


/** A property implementation helper that redirects accesses to the property value
	to the main thread and then executes the accesses via a custom delegate object.
	
	The inner delegate object will only be called from the main thread.

	The property object itself can be used normally from any thread.

	Note that PropertyWithMainThreadDelegate objects MUST be allocated with newObj.	
	
	*/	
template<typename ValType>
class PropertyWithMainThreadDelegate : public RequireNewAlloc< Property<ValType>, PropertyWithMainThreadDelegate >
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

		fetchValueFromDelegate();
	}

	~PropertyWithMainThreadDelegate()
	{
		// make sure that the core does not use the delegate anymore
		dispose();
	}


	void set(const ValType& val) override
	{
		bool valueChanged = false;

		{
			MutexLock lock(_mutex);

			if(val!=_value)
			{
				_value = val;
				valueChanged = true;

				// schedule the delegate to be updated

				scheduleUpdate(
					[val](PropertyWithMainThreadDelegate* pThis)
					{
						pThis->pDelegate->setValue(val);
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
	}

	Property<ValType>& operator=(const ReadProperty<ValType>& val) override
	{
		set(val);
	}

	
	void bind(ReadProperty<ValType>& sourceProperty) override
	{
        _pBindSourceSubscription = sourceProperty.onChange().template subscribeMember<DefaultProperty>(this, &DefaultProperty::bindSourceChanged);
        
        bindSourceChanged(sourceProperty);
    }


	
	void fetchValueFromDelegate()
	{
		{
			// we want to get the current value of the delegate and set it as the value
			// of the property.
			// Invalidate all pending delegate updates. We do not want the delegate to be changed
			// because of old pending writes. Instead we want to use the delegate's CURRENT
			// value as that of the property.
			MutexLock lock(_mutex);

			_firstValidUpdateId = _nextUpdateId;
		}

		scheduleUpdate(
			[](PropertyWithMainThreadDelegate* pThis)
			{
				// note that the mutex is locked during this.
				// We also know that the property has not been disposed yet, otherwise
				// this function would not be called.
					
				ValType newValue = pThis->_pDelegate->getValue();

				if(newValue!=pThis->_value)
				{
					pThis->_value = newValue;

					{
						MutexUnlock unlock(pThis->_mutex);

						_onChange.notify(*pThis);
					}
				}
			} );
	}


protected:
	virtual void bindSourceChanged(const ReadProperty<ValType>& prop)
    {
        set( prop.get() );
    }

	
	void scheduleUpdate( std::function<void(PropertyWithMainThreadDelegate*)> updateFunc)
	{
		// keep ourselves alive until the op is done. Note that we inherit from
		// RequireNewAlloc, so we can do this.
		P<PropertyWithMainThreadDelegate> pThis = this;
			
		int		updateId;

		{
			MutexLock lock(_mutex);

			updateId = _nextUpdateId;
			_nextUpdateId++;
		}

		callFromMainThread(
			[pThis, updateId, updateFunc]()
			{
				MutexLock lock(pThis->_mutex);

				if(updateId >= pThis->_firstValidUpdateId
					&& pThis->_pDelegate!=nullptr )
				{
					pThis->_firstValidUpdateId = updateId+1;
					updateFunc(pThis);					
				}
			} );			
	}



	Mutex		_mutex;
	int64_t		_nextUpdateId = 0;
	int64_t		_firstValidUpdateId = 0;
};


#endif

