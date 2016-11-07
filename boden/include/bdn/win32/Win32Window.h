#ifndef BDN_WIN32_Win32Window_H_
#define BDN_WIN32_Win32Window_H_

#include <Windows.h>

namespace bdn
{
namespace win32
{

/** Represents a "Window" in the Win32 sense of the word. This is different than
	a bdn::Window (which only refers to top level window). Under Win32, all views, top level windows and
	even some other non-visible objects are implemented as a Win32 window.	
*/
class Win32Window : public Base
{
public:
	/** @param className the name of the window class. For custom classes, take a look at 
			Win32WindowClass - it can provide assistance in registering new window classes.

			If the window class has a window proc that does NOT call Win32Window::windowProc,
			then Win32Window will automatically subclass the window to ensure that the message handling
			works. So you can also pass system window classes like "BUTTON" here - they will be automatically
			subclassed.

		@param name the window name. Depending on the window class this can be a label, window title,
			or simply a hidden internal name.
		@param style window style flags. Google "MSDN Window Styles" to see a list
		@param exStyle extended window style flags. Google "MSDN Extended Window Styles" to see a list
		@param parentHwnd handle to the parent window. This can be NULL if the window has no parent.
		*/
	Win32Window(	const String& className,
					const String& name,
					DWORD style,
					DWORD exStyle,
					HWND parentHwnd,
					int x=0,
					int y=0,
					int width=0,
					int height=0);
	~Win32Window();
	

	/** Destroys the window.*/
	void destroy();

	
	/** Returns the window's HWND. NULL if the window was already destroyed.*/
	HWND getHwnd() const
	{
		return _hwnd;
	}



	/** Static function that sets the text of a window (like the Windows function
		SetWindowText).*/
	static void setWindowText(HWND windowHandle, const String& text);


	/** Static function that returns the text of the specified window
		(like the Windows function GetWindowText)*/
	static String getWindowText(HWND windowHandle);
	

	/** Returns a pointer to the Win32Window object that is associated with the specified
		HWND, or nullptr if no Win32Window object is associated with it.

		It is safe to use this on ANY window handle, even ones not created by Win32Window.
		If the handle does not belong to a Win32Window object then nullptr will be returned.
		*/
	static Win32Window* getObjectFromHwnd(HWND hwnd);


	/** Static windowProc function that Window objects use to handle messages. You can use this
		when you create custom window classes for use with #Window (see #WindowClass).*/
	static LRESULT CALLBACK windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);


    /** The message context is used to track the results of a message
        and control how it is processed further.*/
	class MessageContext
	{
	public:
		MessageContext()
		{
			_callDefaultHandler = true;
			_resultSet = false;
			_result = 0;
		}

		/** Sets the result value of the message that is being processed.
			
			Note that it is NOT required to call this. When you do not call this then
			the system's default handler for the message is called and its result value is used.
		
			@param result result value of the message
			@param callDefaultHandler indicates whether or not the system's default handler should be called
				for this message. Note that even if this is true then the result value set with this function
				will be used instead of the one from the default handler.
		*/
		void setResult(LRESULT result, bool callDefaultHandler )
		{
			_result = result;
			_resultSet = true;
			_callDefaultHandler = callDefaultHandler;
		}

		
		/** If the result was set with setResult() then this function stores it in the result
			parameter and returns true.
			
			If the result was not set with setResult() then the result parameter is not modified
			and the function returns false.			
		*/
		bool getResultIfSet(LRESULT& result) const
		{
			if(_resultSet)
			{
				result = _result;
				return true;
			}

			return false;
		}

		bool getCallDefaultHandler() const
		{
			return _callDefaultHandler;
		}

	protected:
		LRESULT _result;
		bool    _resultSet;
		bool	_callDefaultHandler;
	};


protected:
	class ClassBase : public Base
	{
	public:
		ClassBase(const String& name);

		void ensureRegistered();

	protected:
		String		_name;
		bool		_registered;

		WNDCLASSEX	_info;
	};
	friend class ClassBase;


	

	/** Called when the window is about to be destroyed.*/
	virtual void notifyDestroy();

	
	/** Called when a message is received. Derived classes can override this.
	
		By default the default message handler of the system is automatically called after this
		and its return value is the result of the message.

		If you want to override the default result, or if you want to prevent the default handler
		from being called altogether, then you can call MessageContext::overrideResult on the provided
		context object.
	*/
	virtual void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT windowProcImpl(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	HWND _hwnd = NULL;

private:	
	static LRESULT _callDefaultWindowProc(Win32Window* pThis, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void handleCreation(HWND hwnd);
    void handleDestruction();

	bool	_creationHandled = false;

	WNDPROC _prevWindowProc = NULL;
};

}
}

#endif

