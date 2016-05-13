#ifndef BDN_PropertyWithMainThreadDelegate_H_
#define BDN_PropertyWithMainThreadDelegate_H_




/** A property implementation helper that redirects accesses to the property value
	to the main thread and then executes the accesses via a custom delegate object.
	
	The inner delegate object will only be called from the main thread.

	The property object itself can be used normally from any thread.
	
	*/	
template<typename ValType>
class PropertyWithMainThreadDelegate : public Property<ValType>
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
		_pCore = newObj<Core>(pDelegate);

		fetchValueFromDelegate();
	}

	~PropertyWithMainThreadDelegate()
	{
		// make sure that the core does not use the delegate anymore
		_pCore->dispose();
	}

	/** Causes property to update its internal cached value and obtain
		the current one from the delegate.
		
		This function can be called from any thread.
		
		If the function is not called from the main thread then the update
		may happen with a slight delay.
		*/
	void fetchValueFromDelegate()
	{
		_pCore->fetchValueFromDelegate();
	}
	
	void set(const ValType& val) override
	{
		_pCore->set(val);
	}

	Property<ValType>& operator=(const ValType& val) override
	{
		_pCore->set(val);
	}

	
    ValType get() const override
	{
		return _pCore->get();
	}
		
	Notifier< const ReadProperty& >& onChange()
	{
		return _pCore->onChange();
	}
	
	void bind(ReadProperty<ValType>& sourceProperty) override
	{
        _pBindSourceSubscription = sourceProperty.onChange().template subscribeMember<DefaultProperty>(this, &DefaultProperty::bindSourceChanged);
        
        bindSourceChanged(sourceProperty);
    }

protected:
	virtual void bindSourceChanged(const ReadProperty<ValType>& prop)
    {
        set( prop.get() );
    }

	// a core class. The actual property object is just a thin wrapper around this core.
	// We do that because the Core object must be able to keep itself alive, which may not
	// be possible for the actual property object (since Properties are often not allocated with
	// new).
	class Core : public Base
	{
	public:
		Core(IDelegate* pDelegate)
		{
			_pDelegate = pDelegate;
		}

		void dispose()
		{
			MutexLock lock(_mutex);

			_pDelegate = nullptr;

			// invalidate all pending updates. We should
			// not touch the delegate anymore
			_nextValidUpdateId = _nextUpdateId;
		}
	
		void fetchValueFromDelegate()
		{
			{
				// invalidate all pending updates. We do not want the delegate to be changed
				// because of old pending writes.
				MutexLock lock(_mutex);

				_firstValidUpdateId = _nextUpdateId;
			}

			scheduleUpdate(
				[](Core* pThis)
				{
					// note that the mutex is locked during this.
					ValType newValue = pThis->_pDelegate->getValue();

					if(newValue!=pThis->_value)
					{
						pThis->_value = newValue;

						{
							MutexUnlock unlock(pThis->_mutex);

							_onChange.notify(*pThis->pOuterProp);
						}
					}
				} );
		}

		void set(const ValType& val)
		{
			bool valueChanged = false;

			{
				MutexLock lock(_mutex);

				if(val!=_value)
				{
					_value = val;
					valueChanged = true;

					scheduleUpdate(
						[val](Core* pThis)
						{
							pThis->pDelegate->setValue(val);
						} );
				}
			}

			if(valueChanged)
				_onChange.notify(*_pOuterProp); XXX possible reference to deleted object
		}

		ValType get() const
		{
			MutexLock lock(_mutex);
			return _value;
		}


		Notifier< const ReadProperty& >& onChange()
		{
			return _onChange;
		}


		void scheduleUpdate( std::function<void(Core*)> updateFunc)
		{
			// keep ourselves alive until the op is done
			P<Core> pThis = this;
			
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

					if(updateId >= pThis->_firstValidUpdateId && pThis->_pDelegate!=nullptr )
					{
						pThis->firstValidUpdateId = updateId+1;
						updateFunc(pThis);					
					}
				} );			
		}



		P<PendingOpId>	pFirstValidOpId;
		int64_t			opId;

		{
			MutexLock lock(_mutex);

			pFirstValidOpId = _pFirstValidOpId;
			
			opId = _nextOpId;
			_nextOpId++;
		}

		callFromMainThread(
			[pFirstValidOpId, opId]()
			{
				// if the value of pFirstValidPendingOpId is bigger than our op id
				// then someone else has already applied a later update. We must not
				// overwrite that.

				// Note that pFirstValidPendingOpId is also updated when the Property object
				// is destructed, so this check also ensures that we do not access
				// a destroyed object.

				int64_t firstValidOpId = pFirstValidOpId->val.load();
				if(opId >= firstValidOpId)
				{
					::SetWindowText(windowHandle, newValue.asWidePtr() );					

					// if applied update Id may have changed in the meantime.
					// Not by another update (since those are always performed
					// in the main thread), but in some special circumstances the
					// update Id is changed to invalidate older updates
					// (for example when the window handle changes).

					// So we only overwrite it here if its value is still the same.
					std::atomic_compare_exchange_weak(&pAppliedUpdateId->val, &appliedUpdateId, updateId);
				}			
				
			
			})


		bool   valueChanged = false;

		{
			MutexLock lock(_mutex);

			if(windowHandle!=_windowHandle)
			{
				_windowHandle = windowHandle;

				// first we mark all pending updates for the old window
				// as invalid.
				_pAppliedUpdateId->val = _nextUpdateId;
				_nextUpdateId++;

				// update our value to that of the window.
				String newValue;
				if(_windowHandle != NULL )
					newValue = Window::getWindowText(_windowHandle);

				if(newValue!=_value)
				{
					_value = newValue;
					valueChanged = true;
				}
			}
		}

		if(valueChanged)
			_onChange.notify(*this);
	}		




	struct AppliedUpdateId : public Base
	{
		std::atomic<int64_t> val;
	};


	void onValueChanged() override
	{
		// this is called while the mutex is locked.
		// So there are no race conditions here.
		int		updateId = _nextUpdateId;
		_nextUpdateId++;

		String	newValue = _value;

		// cause SetWindowText to be called from the main thread.
		// The following code also ensures that the correct value ends up being
		// set if multiple pending update calls are performed out of order.
		// It also ensures that a pending update is not performed if the Property was
		// deleted in the meantime.
		P<AppliedUpdateId>	pAppliedUpdateId = _pAppliedUpdateId;
		HWND				windowHandle = _windowHandle;

		callFromMainThread(
			[pAppliedUpdateId, updateId, windowHandle, newValue]()
			{
				// if the value of _pAppliedUpdateId is bigger than our update Id
				// then someone else has already applied a later update. We must not
				// overwrite that.
				// Note that _pAppliedUpdateId is also updated when the Property object
				// was destructed, so this check also ensures that we do not access
				// a destroyed object.

				int64_t appliedUpdateId = pAppliedUpdateId->val.load();
				if(appliedUpdateId<updateId)
				{
					::SetWindowText(windowHandle, newValue.asWidePtr() );					

					// if applied update Id may have changed in the meantime.
					// Not by another update (since those are always performed
					// in the main thread), but in some special circumstances the
					// update Id is changed to invalidate older updates
					// (for example when the window handle changes).

					// So we only overwrite it here if its value is still the same.
					std::atomic_compare_exchange_weak(&pAppliedUpdateId->val, &appliedUpdateId, updateId);
				}			
			});
	};


	HWND							_windowHandle = NULL;

	int64_t							_nextUpdateId = 0;
	P<AppliedUpdateId>				_pAppliedUpdateId;
};


#endif

