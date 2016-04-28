#ifndef BDN_Frame_H_
#define BDN_Frame_H_

#include <bdn/init.h>
#include <bdn/IWindow.h>

#include <windows.h>

namespace bdn
{

class Frame : public Base, virtual public IWindow
{
public:
	Frame(const String& title)
	{
		WNDCLASSEX cls;

		memset(&cls, 0, sizeof(cls));
		cls.cbSize = sizeof(cls);
		cls.lpfnWndProc = windowProc;
		cls.lpszClassName = L"bodenFrame";
		cls.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

		::RegisterClassEx(&cls);

		_handle = ::CreateWindowEx(WS_EX_APPWINDOW, cls.lpszClassName, title.asWidePtr(), WS_OVERLAPPEDWINDOW | WS_POPUPWINDOW, 200, 200, 300, 200, NULL, NULL, NULL, NULL);			
	}

	~Frame()
	{
		if(_handle!=NULL)
		{
			::DestroyWindow(_handle);
			_handle = NULL;
		}
	}


	HWND getHandle()
	{
		return _handle;
	}

	void setTitle(const String& title)
	{
		::SetWindowText(_handle, title.asWidePtr() );
	}

		
	virtual void show(bool visible = true) override
	{
		::ShowWindow(_handle, visible ? SW_SHOW : SW_HIDE);
	}

	virtual void hide() override
	{
		show(false);
	}

protected:

	static LRESULT CALLBACK windowProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam);
		

	HWND _handle;
};


}

#endif
