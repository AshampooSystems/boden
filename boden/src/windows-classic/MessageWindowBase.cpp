#include <bdn/init.h>
#include <bdn/MessageWindowBase.h>

#include <bdn/sysError.h>
#include <bdn/log.h>



namespace bdn
{


MessageWindowBase::MessageWindowClass::MessageWindowClass()
{
	_name = "bdn::MessageWindow";

	WNDCLASSEX cls;
	memset(&cls, 0, sizeof(cls));

	cls.cbSize = sizeof(WNDCLASSEX);
	cls.lpfnWndProc = MessageWindowBase::windowProcCallback;
	cls.lpszClassName = _name.asWidePtr();

	::RegisterClassEx(&cls);	
}

MessageWindowBase::MessageWindowBase(const String& windowName)
{
	// ensure that class is registered
	String className = MessageWindowClass::get()->getName();

	_windowHandle = ::CreateWindowEx(
		0,
		className.asWidePtr(),
		windowName.asWidePtr(),
		WS_POPUP,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		::GetModuleHandle(NULL),
		this );

	if(_windowHandle==NULL)
	{
		BDN_throwLastSysError( ErrorFields().add("func", "CreateWindowEx")
											.add("context", "MessageWindowBase constructor") );
	}
}


LRESULT MessageWindowBase::windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{	
	return CallWindowProc(DefWindowProc, windowHandle, message, wParam, lParam);
}

	
LRESULT CALLBACK MessageWindowBase::windowProcCallback(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	MessageWindowBase* pThis;

	if(message==WM_CREATE)
	{
		CREATESTRUCT* pInfo = reinterpret_cast<CREATESTRUCT*>(lParam);

		pThis = static_cast<MessageWindowBase*>(pInfo->lpCreateParams);

		::SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else
		pThis = reinterpret_cast<MessageWindowBase*>(::GetWindowLongPtr(windowHandle, GWLP_USERDATA));

	if(pThis!=NULL)
	{
		try
		{
			return pThis->windowProc(windowHandle, message, wParam, lParam);
		}
		catch(std::exception& e)
		{
			logError(e, "Exception thrown by in MessageWindowBase::windowProc. Ignoring.");

			// fall through and call DefWindowProc
		}
	}

	return CallWindowProc(DefWindowProc, windowHandle, message, wParam, lParam);	
}





}

