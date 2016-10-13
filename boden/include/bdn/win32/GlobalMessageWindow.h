#ifndef BDN_WIN32_GlobalMessageWindow_H_
#define BDN_WIN32_GlobalMessageWindow_H_

#include <bdn/win32/MessageWindowBase.h>
#include <bdn/RequireNewAlloc.h>
#include <bdn/ISimpleCallable.h>


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
    ~GlobalMessageWindow();

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



    
    /** Schedules the specified callable object to be called from the main thread
        when the application enters idle state (i.e. when all pending messages have been processed).

        Also performs the call when the application is currently already in idle state.

        The global GlobalMessageWindow instance returned by get() is notified of idle phases 
        automatically by the message queue. Other instances need to be notified manually by
        calling notifyIdleBegun().

        callOnceWhenIdle can be used from any thread.
        */
    void callOnceWhenIdle( ISimpleCallable* pCallable );


    /** This must be called when an idle phase begins. For the global GlobalMessageWindow instance
        it is called automatically.
        
        This must ONLY be called from the main thread.
        */
    void notifyIdleBegun();


protected:
	
	enum
	{
		MessageCall = WM_USER+100
	};

	virtual void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

    Mutex _idleMutex;
    std::list< P<ISimpleCallable> > _callOnceWhenIdleList;
};


}
}


#endif