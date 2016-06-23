#ifndef BDN_MessageWindowBase_H_
#define BDN_MessageWindowBase_H_

#include <windows.h>

namespace bdn
{

/** Base class for special windows that are only used to receive window messages and are not visible
	to the user.

	This class is only available on Windows Classic / Win32 platforms.
*/
class MessageWindowBase : public Base
{
public:
	/** @param windowName the name of the window. Since the window is invisible
			this is mainly useful for debugging and logging and if some process needs to find the window
			by its name.*/
	MessageWindowBase(const String& windowName);

	~MessageWindowBase();


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


	/** Called when a message is received. Derived classes can override this.
	
		By default the default message handler of the system is automatically called after this
		and its return value is the result of the message.

		If you want to override the default result, or if you want to prevent the default handler
		from being called altogether, then you can call MessageContext::overrideResult on the provided
		context object.
	*/
	virtual void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	

	LRESULT windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);	

	static LRESULT CALLBACK windowProcCallback(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	
	
	HWND	_windowHandle;
	
	LRESULT _messageResult;
	bool	_messageResultSet;

	bool	_callDefaultHandler;
};


}


#endif