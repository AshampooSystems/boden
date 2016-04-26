#include <bdn/init.h>
#include <bdn/MessageWindowBase.h>

namespace bdn
{


MessageWindowBase::MessageWindowClass::MessageWindowClass()
{
	_name = "bdn::MessageWindow";

	WNDCLASSEX cls;
	memset(&cls, 0, sizeof(cls));

	cls.cbSize = sizeof(WNDCLASSEX);
	cls.lpfnWndProc = MessageWindow::windowProcCallback;
	cls.lpszClassName = _name.asWidePtr();

	::RegisterClassEx(&cls);	
}

MessageWindowBase::MessageWindow(const String& windowName)
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
		throwLastSysError();
}
	
LRESULT CALLBACK MessageWindowBase::windowProcCallback(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	MessageWindow* pThis;

	if(message==WM_CREATE)
	{
		CREATESTRUCT* pInfo = reinterpret_cast<CREATESTRUCT*>(lParam);

		pThis = reinterpret_cast<MessageWindow*>(pInfo->lpCreateParams);

		::SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else
		pThis = reinterpret_cast<MessageWindow*>(::GetWindowLongPtr(windowHandle, GWLP_USERDATA));

	if(pThis!=NULL)
		return pThis->windowProc(windowHandle, message, wParam, lParam);
	else		
		return CallWindowProc(DefWindowProc, windowHandle, message, wParam, lParam);
}


LRESULT MessageWindowBase::windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	return CallWindowProc(DefWindowProc, windowHandle, message, wParam, lParam);
}



}

