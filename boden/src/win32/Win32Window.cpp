#include <bdn/init.h>
#include <bdn/win32/Win32Window.h>

#include <bdn/win32/win32Error.h>
#include <bdn/log.h>

#include <unordered_map>


namespace bdn
{
namespace win32
{
	
class _GlobalRegistry
{
public:

	void registerWindow(HWND hwnd, Win32Window* pWindow)
	{
		MutexLock lock(_mutex);

		_windowMap[hwnd] = pWindow;
	}

	void unregisterWindow(HWND hwnd)
	{
		MutexLock lock(_mutex);

		_windowMap.erase(hwnd);
	}

	Win32Window* getObjectFromHwnd(HWND hwnd)
	{
		MutexLock lock(_mutex);

		auto it = _windowMap.find(hwnd);
		if(it==_windowMap.end())
			return nullptr;
		else
			return it->second;
	}

	static _GlobalRegistry& get()
	{
		static _GlobalRegistry reg;

		return reg;
	}

protected:
	Mutex	_mutex;
	std::unordered_map<HWND, Win32Window*>	_windowMap;
};


Win32Window::Win32Window(	const String& className,
							const String& name,
							DWORD style,
							DWORD exStyle,
							HWND parentHandle,
							int x,
							int y,
							int width,
							int height )
{
	HWND hwnd = ::CreateWindowEx(
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

	if(hwnd==NULL)
	{
		BDN_WIN32_throwLastError( ErrorFields().add("func", "CreateWindowEx")
											.add("context", "Window::create") );
	}

    // For our "own" window classes our WM_CREATE handler is called, since the correct windowProc
    // is specified in CreateWindow. If our WM_CREATE handler is called then handleCreation is called from
    // there.
    // But if the window class was registered by someone else then our window proc is not yet installed
    // and our WM_CREATE handler is not yet hooked up. In that case we need to call handleCreation
    // here, after CreateWindow returned.
    if(!_creationHandled)
    {
        handleCreation(hwnd);

        // we also need to subclass the window so that our own windowproc is called.
        _prevWindowProc = (WNDPROC)::SetWindowLongPtr( _hwnd, GWLP_WNDPROC, (LONG_PTR)&Win32Window::windowProc );
    }
}

Win32Window::~Win32Window()
{
	destroy();
}

void Win32Window::destroy()
{
	if(_hwnd!=NULL)
	{
        // DestroyWindow can send multiple notification messages
        // to different windows. If any of these causes this window to have its destruction
        // triggered again then that might cause a crash.
        // We protect against that by first calling our handleDestruction routine (which disconnects
        // us from the HWND and sets _hwnd to NULL) and then calling DestroyWindow.

        HWND hwndToDestroy = _hwnd;

        handleDestruction();

		::DestroyWindow(hwndToDestroy);
	}
}

void Win32Window::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	// do nothing by default
}


LRESULT Win32Window::windowProcImpl(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{	
	MessageContext context;

	handleMessage(context, windowHandle, message, wParam, lParam);

	LRESULT result=0;

	if(context.getCallDefaultHandler())
		result = _callDefaultWindowProc(this, windowHandle, message, wParam, lParam);
	
	context.getResultIfSet(result);

	return result;
}


void Win32Window::notifyDestroy()
{
}


Win32Window* Win32Window::getObjectFromHwnd(HWND hwnd)
{
	// first we check if the window is actually one with an associated Win32Window object.
	// That is more difficult than it sounds, since we cannot go by the window class
	// (we have to be able to use standard window classes like BUTTON with Win32 window).
	// So we use a global registry instead.
	return _GlobalRegistry::get().getObjectFromHwnd(hwnd);
}

void Win32Window::handleCreation(HWND hwnd)
{
    
    // the window might use a system defined window class. If that is
	// the case then our WM_CREATE handler is  we must now do some of the stuff that we normally do in our
	// custom WM_CREATE handler (which was not called, because our windowProc is
	// not used yet).
    if(!_creationHandled)
    {
        _hwnd = hwnd;
    
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( static_cast<Win32Window*>(this) ) );
		_GlobalRegistry::get().registerWindow(hwnd, this);		

		_creationHandled = true;
    }
}

void Win32Window::handleDestruction()
{
    // if the hwnd is null then that means that
    // we have already handled the destruction.
    if(_hwnd!=NULL)
    {
	    notifyDestroy();

	    if(_prevWindowProc!=NULL)
	    {
		    // un-subclass the window
		    ::SetWindowLongPtr(_hwnd, GWLP_WNDPROC, (LONG_PTR)_prevWindowProc );
	    }

        ::SetWindowLongPtr(_hwnd, GWLP_USERDATA, NULL );

        _GlobalRegistry::get().unregisterWindow(_hwnd);

        _hwnd = NULL;
    }
}
	
LRESULT CALLBACK Win32Window::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Win32Window* pThis = nullptr;

	if(message==WM_CREATE)
	{
		CREATESTRUCT* pInfo = reinterpret_cast<CREATESTRUCT*>(lParam);

		pThis = static_cast<Win32Window*>(pInfo->lpCreateParams);

        pThis->handleCreation(hwnd);		
	}
	else
    {
		pThis = reinterpret_cast<Win32Window*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));	
    }

	bool    callDefault = true;	
	LRESULT result = 0;

    if(pThis!=nullptr)
	{
        if(pThis->_hwnd==NULL)
        {
            // handleDestruction has already been called. Do not pass to our windowProc.
            // Just call the default.
            callDefault = true;
        }
        else if(message==WM_DESTROY)
        {
            pThis->handleDestruction();
            callDefault = true;
        }
        else
        {
		    try
		    {
			    result = pThis->windowProcImpl(hwnd, message, wParam, lParam);
			
			    // windowProc automatically calls the default handler. So nothing more to do here.
			    callDefault = false;
		    }
		    catch(std::exception& e)
		    {
			    logError(e, "Exception thrown by in Win32Window::windowProcImpl. Ignoring.");

			    // fall through and call DefWindowProc
		    }
        }
	}
	
	if(callDefault)
		result = _callDefaultWindowProc(pThis, hwnd, message, wParam, lParam);		

	return result;
}
	
LRESULT Win32Window::_callDefaultWindowProc(Win32Window* pThis, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(pThis!=nullptr && pThis->_prevWindowProc!=NULL)
	{
		// we have subclassed the window. So we must call the old window proc.
		return CallWindowProc(pThis->_prevWindowProc, hwnd, message, wParam, lParam);	
	}
	else
	{
		// call the default system window proc
		return CallWindowProc(DefWindowProc, hwnd, message, wParam, lParam);	
	}
}


void Win32Window::setWindowText(HWND hwnd, const String& text)
{
	::SetWindowText(hwnd, text.asWidePtr() );
}
	
String Win32Window::getWindowText(HWND hwnd)
{
	String result;

	int		prevLen = 0;
	while(true)
	{
		int len = ::GetWindowTextLength(hwnd);

		// make sure that we use a bigger buffer in each iteration
		if(len < prevLen*2)
			len = prevLen*2;

		len += 10;					

		wchar_t* pBuf = new wchar_t[len+1];

		int actualLen = GetWindowText(hwnd, pBuf, len);
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
}

