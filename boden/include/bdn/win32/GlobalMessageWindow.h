#ifndef BDN_WIN32_GlobalMessageWindow_H_
#define BDN_WIN32_GlobalMessageWindow_H_

#include <bdn/win32/MessageWindowBase.h>
#include <bdn/RequireNewAlloc.h>
#include <bdn/ISimpleCallable.h>

#include <atomic>

namespace bdn
{
namespace win32
{

/** The global message window that is used to handle messages for the app.

	Call GlobalMessageWindow::get() to get the global instance.

	This class is only available on Windows Classic / Win32 platforms.
*/
class GlobalMessageWindow : public RequireNewAlloc<MessageWindowBase, GlobalMessageWindow>
{
public:
	GlobalMessageWindow();

	/** Returns a reference to the global message window instance. This instance is used
		to receive messages that are relevant for the app as a whole (like system notifications
		etc.)*/
	static GlobalMessageWindow& get();
	
	/** Causes the specified callable object to be called from the main thread.
		The ISimpleCallable::call method of the object will be called from 
		the normal message loop processing in the main thread.

        postCall can be used from any thread.
		*/
	void postCall(ISimpleCallable* pCallable);	

protected:
	
	enum
	{
		MessageCall = WM_USER+100
	};

	virtual void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
};


}
}


#endif