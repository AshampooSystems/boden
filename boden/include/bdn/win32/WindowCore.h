#ifndef BDN_WIN32_WindowCore_H_
#define BDN_WIN32_WindowCore_H_

#include <bdn/Window.h>
#include <bdn/win32/ViewCore.h>
#include <bdn/win32/Win32WindowClass.h>

#include <bdn/IWindowCore.h>


namespace bdn
{
namespace win32
{

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore
{
public:
	WindowCore(Window* pWindow);			

    ~WindowCore();
	
	void	setTitle(const String& title) override;

	Rect getContentArea() override;

	Size calcWindowSizeFromContentAreaSize(const Size& contentSize) override;

	Size calcContentAreaSizeFromWindowSize(const Size& windowSize) override;
	
	Rect getScreenWorkArea() const override;	

	Size getMinimumSize() const;

	Size calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const override;
	
protected:
	class WindowCoreClass : public Win32WindowClass
	{
	public:
		WindowCoreClass()
			: Win32WindowClass("bdnWindow", ViewCore::windowProc)
		{
			_info.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

			ensureRegistered();			
		}

		static WindowCoreClass& get();
	};

	void initUiScaleFactor();
	void setWindowsDpiValue(int dpi);

	void dpiChanged(int newDpi, const RECT* pSuggestedNewRect );

	void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;
};

}
}

#endif

