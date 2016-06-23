#ifndef BDN_WIN32_MessageWindowBase_H_
#define BDN_WIN32_MessageWindowBase_H_

#include <bdn/win32/Win32WindowClass.h>
#include <bdn/win32/Win32Window.h>

namespace bdn
{
namespace win32
{

/** Base class for special windows that are only used to receive window messages and are not visible
	to the user.

	This class is only available on Windows Classic / Win32 platforms.
*/
class MessageWindowBase : public Win32Window
{
public:
	/** @param windowName the name of the window. Since the window is invisible
			this is mainly useful for debugging and logging and if some process needs to find the window
			by its name.*/
	MessageWindowBase(const String& windowName);
	
	
protected:
	class MessageWindowClass : public Win32WindowClass
	{
	public:
		MessageWindowClass()
			: Win32WindowClass("bdn::MessageWindow", &Win32Window::windowProc)
		{
			ensureRegistered();
		}
		
		static P<MessageWindowClass> get()
		{
			static SafeInit<MessageWindowClass> init;

			return init.get();
		}

	};
	friend class MessageWindowClass;
};


}
}


#endif