#ifndef BDN_GlobalMessageWindow_H_
#define BDN_GlobalMessageWindow_H_

#include <bdn/MessageWindowBase.h>
#include <bdn/ISimpleCallable.h>

namespace bdn
{

/** The global message window that is used to handle messages for the app.

	Call GlobalMessageWindow::get() to get the global instance.

	This class is only available on Windows Classic / Win32 platforms.
*/
class GlobalMessageWindow : public MessageWindowBase
{
public:
	GlobalMessageWindow();

	/** Returns a pointer to the global message window instance. This instance is used
		to receive messages that are relevant for the app as a whole (like system notifications
		etc.)*/
	static P<GlobalMessageWindow> get()
	{
		static SafeInit<GlobalMessageWindow> init;

		return init.get();
	}
	
	/** Causes the specified callable object to be called from the main thread.
		The ISimpleCallable::call method of the object will be called from 
		the normal message loop processing in the main thread.
		*/
	void postCall(ISimpleCallable* pCallable);	

protected:
	
	enum
	{
		MessageCall = WM_USER+100
	};

	virtual LRESULT windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

};


}


#endif