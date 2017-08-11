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

class WindowCore : public ViewCore, BDN_IMPLEMENTS IWindowCore, BDN_IMPLEMENTS LayoutCoordinator::IWindowCoreExtension
{
public:
	WindowCore(Window* pWindow);			

    ~WindowCore();
	
	void	setTitle(const String& title) override;

	
    /** Returns the area of the screen that can be used by windows.
		That excludes taskbars, sidebars and the like (if they are always visible).
		The returned rect applies only to the screen that the window is currently on.
		Other screens can have different window areas.
        
        Note that the work area position may have negative coordinates on systems
        with multiple monitors. That can be normal.
     
        */
	Rect getScreenWorkArea() const;	

	Size getMinimumSize() const;

	Size calcPreferredSize( const Size& availableSpace = Size::none() ) const override;
    void layout() override;

	void requestAutoSize();
    void requestCenter();

    void center() override;
    void autoSize() override;
       

	
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
    void sizeChanged(int changeType);

	void handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
    /** Returns the size of the non-client area around the window.
        This includes the window border, titlebar, etc.*/
    Margin getNonClientMargin() const;

};

}
}

#endif

