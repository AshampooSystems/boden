#ifndef BDN_Win32ViewCore_H_
#define BDN_Win32ViewCore_H_

#include <bdn/WindowBase.h>
#include <bdn/IViewCore.h>

#include <map>

#include <Windows.h>

namespace bdn
{

/** Base class for 'Windows' in the Win32 sense. This is also used as the base class for non-visible
	windows like pure message windows and the like.
	
	This implements just the most basic functionality of creation, destruction and message handling.
	
	Actual visible windows should be derived from #Window instead!
*/
class Win32ViewCore : public WindowBase, BDN_IMPLEMENTS IViewCore
{
public:
	Win32ViewCore()
	{
	}
	

	HWND getHwnd()
	{
		return _pHandle->getHwnd();
	}
	
	void	setVisible(bool visible)
	{
		::ShowWindow( getHwnd(), visible ? SW_SHOW : SW_HIDE);		
	}

	bool	getVisible() const
	{
		return (::GetWindowLong( getHwnd(), GWL_STYLE) & WS_VISIBLE) == WS_VISIBLE;
	}
		



	/** Called when the outer view's parent has changed.
		
		tryChangeParentView should try to move the core over to the new parent.

		If successful then it should return true, otherwise false.

		If false is returned then this will cause the outer view object to
		automatically re-create the core for the new parent and release the current
		core object.		
		*/
	bool tryChangeParentView(View* pNewParent)
	{
		if( getViewHwnd(pNewParent)==_parentHwnd )
		{
			// the underlying win32 window is the same. So we are done.
			// Note that this case is quite common, since many View containers
			// are actually light objects that do not have their own backend
			// Win32 window. So often the Win32 window of some higher ancestor is
			// shared by many windows.
			return true;
		}
		else
		{
			// we cannot move a window to a new underlying win32 window.
			return false;
		}
	}


	/** Returns the handle of the win32 window for the specified view.
		If the view is a light view without its own win32 window then the
		handle of the first ancestor with a win32 is returned.

		Returns NULL if pView is null or if pView does not have an associated
		core object.
		*/
	static HWND getViewHwnd(View* pView)
	{
		if(pView!=nullptr)
		{
			IViewCore* pCore = pView->getViewCore();
			if(pCore!=nullptr)
				return cast<Win32ViewCore>(pCore)->getHwnd();
		}

		return NULL;
	}

	

	/** Destroys the window.*/
	void destroy();

	
	/** Returns the window's handle object.*/
	P<WindowHandle> getHandle()
	{
		return _pHandle;
	}



	/** Static function that sets the text of a window (like the Windows function
		SetWindowText).*/
	static void setWindowText(HWND windowHandle, const String& text);


	/** Static function that returns the text of the specified window
		(like the Windows function GetWindowText)*/
	static String getWindowText(HWND windowHandle);
	


	/** Static windowProc function that Window objects use to handle messages. You can use this
		when you create custom window classes for use with #Window (see #WindowClass).*/
	static LRESULT CALLBACK windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	

	/** Creates the window.
	
		@param className the name of the window class. For custom classes, take a look at 
			WindowClassBase - it can provide assistance in registering new window classes.
		@param name the window name. Depending on the window class this can be a label, window title,
			or simply a hidden internal name.
		@param style window style flags. Google "MSDN Window Styles" to see a list
		@param exStyle extended window style flags. Google "MSDN Extended Window Styles" to see a list
		@param parentHandle handle to the parent window. This can be NULL if the window has no parent.
	*/
	void createBase(	const String& className,
						const String& name,
						DWORD style,
						DWORD exStyle,
						HWND parentHandle,
						int x=0,
						int y=0,
						int width=0,
						int height=0 );

	

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
		
	P<WindowHandle>		_pHandle;	
};

}

#endif

