#include <bdn/init.h>
#include <bdn/mainThread.h>


#include <windows.h>

namespace bdn
{


class GlobalMessageWindow
{
public:
	GlobalMessageWindow()
	{
		WNDCLASSEX cls;
		memset(&cls, 0, sizeof(cls));

		cls.cbSize = sizeof(WNDCLASSEX);
		cls.lpfnWndProc = windowProc;
		cls.lpszClassName = L"bdn::globalMessageWindow";

		::RegisterClass(&cls);

		_windowHandle = ::CreateWindowEx(
			0,
			cls.lpszClassName,
			L"bdn.globalMessageWindow",
			WS_POPUP,
			0,
			0,
			0,
			0,
			NULL,
			NULL,
			::GetModuleInstance(),
			this );

		if(_windowHandle==NULL)
			throwLastSysError();
	}


	HWND getHandle()
	{
		return _windowHandle;
	}

	void postCallMessage(ICallable* pCallable)
	{
		pCallable->addRef();
		::PostMessage(_windowHandle, CallMessage, 0, (LPARAM)pCallable );
	}

	static P<GlobalMessageWindow> get()
	{
		static SafeInit<GlobalMessageWindow> init;

		init.get();
	}

protected:
	
	enum
	{
		CallMessage = WM_USER+100
	};


	static LRESULT windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(message==WM_CREATE)
		{
			CREATESTRUCT* pInfo = (CREATESTRUCT*)lParam;
			
			::SetWindowLongPtr(_windowHandle, (LONG_PTR)pInfo->lpCreateParams);
		}

		return CallWindowProc(DefWindowProc, windowHandle, message, wParam, lParam);
	}


	HWND _windowHandle;
};


void CallFromMainThread_::dispatch()
{
	GlobalMessageWindow::get()->postCallMessage(this);
}


}

