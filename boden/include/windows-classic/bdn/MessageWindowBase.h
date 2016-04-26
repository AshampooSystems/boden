#ifndef BDN_MessageWindow_H_
#define BDN_MessageWindow_H_

#include <windows.h>

namespace bdn
{

/** Base class for special windows that are only used to receive window messages and are not visible
	to the user.

	This class is only available on Windows Classic / Win32 platforms.
*/
class MessageWindow : public Base
{
public:
	/** @param windowName the name of the window. Since the window is invisible
			this is mainly useful for debugging and logging and if some process needs to find the window
			by its name.*/
	MessageWindow(const String& windowName);


	/** Returns the window handle.*/
	HWND getHandle()
	{
		return _windowHandle;
	}

protected:

	class MessageWindowClass : public Base
	{
	public:
		MessageWindowClass();

		String getName() const
		{
			return _name;
		}

		static P<MessageWindowClass> get()
		{
			static SafeInit<MessageWindowClass> init;

			return init.get();
		}

	protected:
		String _name;
	};
	friend class MessageWindowClass;
	

	static LRESULT CALLBACK windowProcCallback(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	
	virtual LRESULT windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	

	HWND _windowHandle;
};


}


#endif