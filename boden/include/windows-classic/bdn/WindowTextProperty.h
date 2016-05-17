#ifndef BDN_WindowTextProperty_H_
#define BDN_WindowTextProperty_H_


#include <bdn/DefaultProperty.h>
#include <bdn/mainThread.h>

#include <bdn/Window.h>

namespace bdn
{
	

/** A property implementation that directly accesses the text of a window
	(SetWindowText, GetWindowText).*/
class WindowTextProperty : public DefaultProperty<String>
{
public:
	WindowTextProperty(HWND windowHandle = NULL)
	{
		_pAppliedUpdateId = newObj<AppliedUpdateId>();

		_windowHandle = windowHandle;		
	}

	~WindowTextProperty()
	{
		// set the applied update ID to _nextUpdateId. That will automatically
		// cause all scheduled updates to be invalidated, thus ensuring that
		// the destroyed object is not accessed.
		MutexLock lock(_mutex);
		_pAppliedUpdateId->val = _nextUpdateId;
	}

	void setWindowHandle(HWND windowHandle)
	{
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
	

protected:
	struct AppliedUpdateId : public Base
	{
		std::atomic<int64_t> val;
	};


	void onValueChanged() override
	{
		// this is called while the mutex is locked.
		// So there are no race conditions here.
		int64_t		updateId = _nextUpdateId;
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

}


#endif


