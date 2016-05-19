#ifndef BDN_MessageWindowBase_H_
#define BDN_MessageWindowBase_H_

#include <bdn/WindowClass.h>
#include <bdn/WindowBase.h>

namespace bdn
{

/** Base class for special windows that are only used to receive window messages and are not visible
	to the user.

	This class is only available on Windows Classic / Win32 platforms.
*/
class MessageWindowBase : public WindowBase
{
public:
	/** @param windowName the name of the window. Since the window is invisible
			this is mainly useful for debugging and logging and if some process needs to find the window
			by its name.*/
	MessageWindowBase(const String& windowName);
	
	
protected:
	class MessageWindowClass : public WindowClass
	{
	public:
		MessageWindowClass()
			: WindowClass("bdn::MessageWindow", &WindowBase::windowProc)
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


#endif