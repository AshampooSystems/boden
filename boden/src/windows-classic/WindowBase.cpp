#include <bdn/init.h>
#include <bdn/WindowBase.h>

#include <bdn/sysError.h>
#include <bdn/log.h>

namespace bdn
{
	
WindowBase::WindowBase()
{
}

void WindowBase::createBase(
	const String& className,
	const String& name,
	DWORD style,
	DWORD exStyle,
	HWND parentHandle,
	int x,
	int y,
	int width,
	int height )
{
	HWND handle = ::CreateWindowEx(
		exStyle,
		className.asWidePtr(),
		name.asWidePtr(),
		style,
		x,
		y,
		width,
		height,
		parentHandle,
		NULL,
		::GetModuleHandle(NULL),
		this );

	if(handle==NULL)
	{
		BDN_throwLastSysError( ErrorFields().add("func", "CreateWindowEx")
											.add("context", "Window::create") );
	}

	_pHandle = newObj<WindowHandle>(handle);
}

WindowBase::~WindowBase()
{
	destroy();
}


void WindowBase::destroy()
{
	if(_pHandle!=nullptr)
		_pHandle->destroy();
}


void WindowBase::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	// do nothing by default
}


LRESULT WindowBase::windowProcImpl(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{	
	MessageContext context;

	handleMessage(context, windowHandle, message, wParam, lParam);

	LRESULT result=0;

	if(context.getCallDefaultHandler())
		result = CallWindowProc(DefWindowProc, windowHandle, message, wParam, lParam);
	
	context.getResultIfSet(result);

	return result;
}


void WindowBase::notifyDestroy()
{
	if(_pHandle!=nullptr)
		_pHandle->notifyDestroy();		
}
	
LRESULT CALLBACK WindowBase::windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowBase* pThis = NULL;

	if(message==WM_CREATE)
	{
		CREATESTRUCT* pInfo = reinterpret_cast<CREATESTRUCT*>(lParam);

		pThis = static_cast<WindowBase*>(pInfo->lpCreateParams);

		::SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else
		pThis = reinterpret_cast<WindowBase*>(::GetWindowLongPtr(windowHandle, GWLP_USERDATA));

	LRESULT result = 0;
	bool    callDefault = true;

	if(pThis!=NULL)
	{
		try
		{
			result = pThis->windowProcImpl(windowHandle, message, wParam, lParam);
			
			// windowProc automatically calls the default handler. So nothing more to do here.
			callDefault = false;
		}
		catch(std::exception& e)
		{
			logError(e, "Exception thrown by in Window::windowProc. Ignoring.");

			// fall through and call DefWindowProc
		}
	}

	if(message==WM_DESTROY)
	{
		if(pThis!=nullptr)
			pThis->notifyDestroy();

		::SetWindowLongPtr(windowHandle, GWLP_USERDATA, NULL );

		if(pThis!=nullptr)
			pThis->_pHandle = nullptr;
	}

	if(callDefault)
		result = CallWindowProc(DefWindowProc, windowHandle, message, wParam, lParam);	

	return result;
}
	

void WindowBase::setWindowText(HWND windowHandle, const String& text)
{
	::SetWindowText(windowHandle, text.asWidePtr() );
}
	
String WindowBase::getWindowText(HWND windowHandle)
{
	String result;

	int		prevLen = 0;
	while(true)
	{
		int len = ::GetWindowTextLength(windowHandle);

		// make sure that we use a bigger buffer in each iteration
		if(len < prevLen*2)
			len = prevLen*2;

		len += 10;					

		wchar_t* pBuf = new wchar_t[len+1];

		int actualLen = GetWindowText(windowHandle, pBuf, len);
		if( actualLen < len)
		{
			// success.
			pBuf[actualLen] = 0;

			result = pBuf;
			delete[] pBuf;

			break;
		}

		delete[] pBuf;

		// our buffer was not big enough (i.e. someone changed the text in between).
		// It is uncertain whether this can actually ever happen, but we handle it anyway.
		prevLen = len;
	}

	return result;
}




}

