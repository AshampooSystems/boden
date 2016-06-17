#ifndef BDN_WindowCore_H_
#define BDN_WindowCore_H_

#include <bdn/Window.h>
#include <bdn/ViewCore.h>
#include <bdn/Win32WindowClass.h>

#include <bdn/IWindowCore.h>


namespace bdn
{

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore
{
public:
	WindowCore(Window* pWindow);			
	
	void	setTitle(const String& title) override;

	Rect getContentArea() override;

	void layout();

protected:
	class WindowCoreClass : public Win32WindowClass
	{
	public:
		WindowCoreClass()
			: Win32WindowClass("bdnWindow", WindowCore::windowProc)
		{
			_info.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

			ensureRegistered();			
		}

		static P<WindowCoreClass> get()
		{
			static SafeInit<WindowCoreClass> init;

			return init.get();
		}		
	};

	void initUiScaleFactor();
	void setWindowsDpiValue(int dpi);

	void dpiChanged(int newDpi, const RECT* pSuggestedNewRect );

	void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;
};

}

#endif

