#ifndef BDN_WindowHandle_H_
#define BDN_WindowHandle_H_

#include <bdn/RequireNewAlloc.h>
#include <bdn/mainThread.h>
#include <bdn/Notifier.h>

#include <windows.h>

namespace bdn
{
	
/** Represents a Windows window handle.

	The window is automatically destroyed when the WindowHandle object is deleted.

	WindowHandle objects provide a way for others to register for an onDestroy() event
	and be notified when the window is destroyed. Note that the WindowHandle object requires
	some cooperation from the window itself for this to work: the window must call notifyDestroy()
	when it receives the WM_DESTROY message.

	WindowHandle objects are also useful when window handles are shared between different
	objects.

	WindowHandle objects must be allocated with newObj.
	*/
class WindowHandle : public RequireNewAlloc<Base, WindowHandle>
{
public:
	WindowHandle(HWND handle)
	{
		_handle = handle;

		_destroyNotified = false;
	}

	~WindowHandle()
	{
		// do nothing here. The real implementation is in deleteThis.
	}


	/** Destroys the window. Automatically notifies the subscribers of onDestroy().

		This can be called from any thread. If it is called from a thread other than the
		main thread then the actual destruction happens asynchronously in the background
		(possibly after the function has returned).
		*/
	void destroy()
	{
		if(_handle!=NULL)
			_destroyImpl(this);
	}

	HWND getHwnd() const
	{
		return _handle;
	}

	Notifier<const WindowHandle&> & onDestroy()
	{
		return _onDestroy;
	}

	/** Tells the window handle to notify its onDestroy() subscribers that
		the window is being destroyed.*/
	void notifyDestroy()
	{
		if(!_destroyNotified)
		{
			_destroyNotified = true;

			_onDestroy.notify(*this);
		}		
	}

protected:
	void deleteThis() override
	{
		if(_handle!=NULL)
		{
			// the window still exists. We need to destroy it.
			// However, since we might not be in the main thread we cannot do it directly here.

			// So cancel our deletion for now
			P<IBase> pThisRef = cancelDeleteThisAndReturnNewReference();

			_destroyImpl(pThisRef);			
		}	
	}

	void _destroyImpl(P<IBase> pThisRef)
	{
		// and schedule deletion from the main thread
		callFromMainThread(
				[pThisRef, this]()
				{
					// no mutex necessary here. _handle is only modified from the main thread.
					if(_handle!=NULL)
					{
						// we intentionally do NOT call notifyDestroy here.
						// Windows can be destroyed automatically for various reasons
						// (for example, when the parent window is destroyed). For the onDestroy
						// notification to work properly in these cases, we require cooperation from
						// the window itself (it must call notifyDestroy).
						// If we were to call notifyDestroy here directly then we would hide bugs where
						// the window does not call notifyDestroy. So to ensure that we do not hide those
						// bugs we ONLY call DestroyWindow. DestroyWindow should send the WM_DESTROY message
						// to the window, which in turn should call our notifyDestroy function.

						::DestroyWindow(_handle);
						_handle = NULL;
					}
				} );
	}


	HWND	_handle;

	Notifier<const WindowHandle&>	_onDestroy;

	bool							_destroyNotified;
};


}

#endif