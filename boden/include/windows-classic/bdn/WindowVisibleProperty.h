#ifndef BDN_WindowVisibleProperty_H_
#define BDN_WindowVisibleProperty_H_

#include <bdn/PropertyWithMainThreadDelegate.h>

#include <bdn/WindowHandle.h>

namespace bdn
{
	

/** A property that controls the visibility of a window. This can be used to show/hide
	windows.

	Note that it is safe to use this property from any thread. Accesses will be
	rerouted to the main thread (and performed asynchronously).

	The property has a cached copy of the window visibility flag. If you need it to update its
	cached copy then you can call updateCachedValue(). updateCachedValue also redirects
	the operation to the main thread and may be performed asynchronously in the background.
	
	Note that WindowVisibleProperty objects MUST be allocated with newObj.	

	*/
class WindowVisibleProperty : public PropertyWithMainThreadDelegate<bool>
{
public:
	WindowVisibleProperty(WindowHandle* pHandle, bool initialValue)
		: PropertyWithMainThreadDelegate( newObj<Delegate>(pHandle), initialValue )
	{
	}
	

protected:
	class Delegate : public Base, BDN_IMPLEMENTS IDelegate
	{
	public:
		Delegate(WindowHandle* pHandle)
		{
			_pHandle = pHandle;
		}

		void set(const bool& visible) override
		{
			::ShowWindow( _pHandle->getHwnd(), visible ? SW_SHOW : SW_HIDE );
		}

		bool get() const override
		{
			return (::GetWindowLong(_pHandle->getHwnd(), GWL_STYLE) & WS_VISIBLE) == WS_VISIBLE;
		}

	protected:
		P<WindowHandle> _pHandle;
	};
};

}


#endif


