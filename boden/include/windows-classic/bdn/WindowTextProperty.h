#ifndef BDN_WindowTextProperty_H_
#define BDN_WindowTextProperty_H_


#include <bdn/PropertyWithMainThreadDelegate.h>

#include <bdn/WindowHandle.h>
#include <bdn/WindowBase.h>

namespace bdn
{
	

/** A property implementation that directly accesses the text of a window
	(like using the Windows functions SetWindowText and GetWindowText).

	Note that it is safe to use this property from any thread. Accesses will be
	rerouted to the main thread (and performed asynchronously).

	The property has a cached copy of the window text. If you need it to update its
	cached copy then you can call updateCachedValue(). updateCachedValue also redirects
	the operation to the main thread and may be performed asynchronously in the background.
	
	Note that WindowTextProperty objects MUST be allocated with newObj.	

	*/
class WindowTextProperty : public PropertyWithMainThreadDelegate<String>
{
public:
	WindowTextProperty(WindowHandle* pHandle, const String& initialValue)
		: PropertyWithMainThreadDelegate( newObj<Delegate>(pHandle), initialValue )
	{
		_pOnDestroySub = pHandle->onDestroy().subscribeVoidMember<WindowTextProperty>(this, &WindowTextProperty::onWindowDestroy);
	}
	

protected:
	void onWindowDestroy()
	{
		// Window is being destroyed. Ensure that the delegate is not accessed.
		detachDelegate();
	}


	class Delegate : public Base, BDN_IMPLEMENTS IDelegate
	{
	public:
		Delegate(WindowHandle* pHandle)
		{
			_pHandle = pHandle;			
		}

		void set(const String& text) override
		{
			// this is only called from the main thread
			if(_pHandle!=nullptr)
				WindowBase::setWindowText( _pHandle->getHwnd(), text );
		}

		String get() const override
		{
			if(_pHandle!=nullptr)
				return WindowBase::getWindowText( _pHandle->getHwnd() );
			else
				return "";
		}

	protected:
		P<WindowHandle> _pHandle;		
	};

	P<IBase>		_pOnDestroySub;
};

}


#endif


