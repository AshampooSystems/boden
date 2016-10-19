#include <bdn/init.h>
#include <bdn/win32/WindowCore.h>

#include <bdn/win32/win32Error.h>
#include <bdn/win32/hresultError.h>
#include <bdn/NotImplementedError.h>
#include <bdn/win32/util.h>
#include <bdn/PixelAligner.h>

#include <ShellScalingApi.h>

namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( WindowCore::WindowCoreClass, WindowCore::WindowCoreClass::get );


WindowCore::WindowCore(Window* pWindow)
	: ViewCore(	pWindow,
				WindowCore::WindowCoreClass::get().getName(),
				pWindow->title().get(),
				WS_OVERLAPPEDWINDOW | WS_POPUPWINDOW,
				WS_EX_APPWINDOW )
{
	initUiScaleFactor();
}


WindowCore::~WindowCore()
{
    Win32Window::destroy();
}


void WindowCore::initUiScaleFactor()
{
	HMONITOR	hMonitor = ::MonitorFromWindow(getHwnd(), MONITOR_DEFAULTTONEAREST);

	UINT dpiX = 0;
	UINT dpiY = 0;
	HRESULT result = ::GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX ,&dpiY);
	if(FAILED(result))
	{
		throw hresultToSystemError( result, ErrorFields().add("func", "GetDpiForMonitor")
												.add("action", "WindowCore::initUiScaleFactor") );
	}

	// X and Y dpi values are always the same for windows programs.
	setWindowsDpiValue( dpiY );
}

void WindowCore::setWindowsDpiValue(int dpi)
{
	// note that the DPI we get here is not a real physical DPI. Rather it is a more complicated
	// value that takes into account the device type and even user settings.
	// It is basically the UI scale factor that Windows wants us to use, multiplied by 96.

	setUiScaleFactor( ((double)dpi) / 96.0 );	
}


void WindowCore::dpiChanged(int newDpi, const RECT* pSuggestedNewRect )
{
	setWindowsDpiValue(newDpi);

	if(pSuggestedNewRect!=NULL)
	{
        Rect newRect = win32RectToRect(*pSuggestedNewRect, getUiScaleFactor() );

        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
		    pView->adjustAndSetBounds( newRect );
	}
}

void	WindowCore::setTitle(const String& title)
{
	Win32Window::setWindowText( getHwnd(), title );
}


/*
void WindowCore::autoSize()
{
	// lock the hierarchy mutex, so that no changes can occur to the hierarchy during this
	MutexLock lock( View::getHierarchyAndCoreMutex() );

	P<View> pContentView = cast<Window>(_pOuterViewWeak)->getContentView();

	RECT windowRect;
	::GetWindowRect(_hwnd, &windowRect);
	HANDLE hMonitor = MonitorFromRect(windowRect, MONITOR_DEFAULTTONEAREST);

	MONITORINFO monitorInfo = {0};

	monitorInfo.cbSize = sizeof(monitorInfo);
	::GetMonitorInfo(hMonitor, &monitorInfo);

	RECT screenWorkRect = monitorInfo.rcWork;

	int screenWidth = screenWorkRect.right - screenWorkRect.left;
	int screenHeight = screenWorkRect.bottom - screenWorkRect.top;

	SIZE maxContentSize = inverseofAdjustWindowRect(screenSize);
	int maxContentWidth = maxContentSize.cx;
	int maxContentHeight = maxContentSize.cy;
	
	// start with the preferred size
	Size size = pContentView->getPreferredSize();

	// clip width
	if(contentSize.width>maxContentWidth)
	{
		contentSize.width = maxContentWidth;

		// we must readjust the height, since it might depend on the width
		contentSize.height = pContentView->calcMinHeightFromWidth(contentSize.width);
	}

	if(contentSize.height>maxContentHeight)
	{
		contentSize.height = maxContentHeight;

		contentSize.width = pContentView->calcMinWidthFromHeight(contentSize.height);

		if(contentSize.width>maxContentWidth)
		{
			// the content simply does not fit into the available space.
			// Just set it to the maximum.
			contentSize.width = maxContentWidth;
		}
	}

	Size windowSize = windowSizeFromContentSize(contentSize);

	setSize(windowSize);	
}*/


Rect WindowCore::getContentArea()
{
	RECT clientRect;
	::GetClientRect(getHwnd(), &clientRect);

	return win32RectToRect(clientRect, getUiScaleFactor() );
}


Size WindowCore::getMinimumSize() const
{
	Size minSize;
	
	minSize.width = ::GetSystemMetrics(SM_CXMINTRACK);
	minSize.height = ::GetSystemMetrics(SM_CYMINTRACK);

    // we return the size in DIPs, so it is correct to just use
    // the raw values (not scaled by UI scale factor).

	return minSize;
}

Size WindowCore::calcPreferredSize(double availableWidth, double availableHeight) const
{
	// the implementation for this must be provided by the outer Window object.
	throw NotImplementedError("WindowCore::calcPreferredSize");	
}

	
	

Size WindowCore::calcWindowSizeFromContentAreaSize(const Size& contentAreaSize)
{
    // we need to calculate this in real pixels.

    // round the content area size UP to the next pixel here. Having a window that is 1 pixel "too large"
    // is usually preferable to having one in which the content does not fit.
    double scaleFactor = getUiScaleFactor();
    Rect contentArea( Point(), contentAreaSize);
    contentArea = PixelAligner( scaleFactor ).alignRect( contentArea, RoundType::nearest, RoundType::up);

    RECT rect = rectToWin32Rect(contentArea, scaleFactor );

	DWORD style = ::GetWindowLongW(getHwnd(), GWL_STYLE);
	DWORD exStyle = ::GetWindowLongW(getHwnd(), GWL_EXSTYLE);

	HMENU menuHandle = ::GetMenu( getHwnd() );

	if(!::AdjustWindowRectEx( &rect, style, (menuHandle!=NULL) ? TRUE : FALSE, exStyle ) )
	{
		BDN_WIN32_throwLastError( ErrorFields().add("func", "AdjustWindowRectEx")
											.add("action", "WindowCore::calcWindowSizeFromContentAreaSize")
											.add("contentAreaSize", std::to_string(contentAreaSize.width)+"x"+std::to_string(contentAreaSize.height) ));
	}

    Rect windowRect = win32RectToRect( rect, scaleFactor );

    return windowRect.getSize();
}


Size WindowCore::calcContentAreaSizeFromWindowSize(const Size& windowSize)
{
	// there isn't an inverse function of AdjustWindowRect in the Win32 api.

	// Thankfully, on windows we can assume that the size of the decoration around
	// the content area (border, title bar, etc.) is the same for any content area size.
	// Note that STRICTLY speaking that is not actually true, since at some point
	// a window menu can become multiline (if the window is not wide enough). But AdjustWindowRect
	// doesn't handle that anyway, so for the time being we also ignore it.

	// So we use a fairly large fake content area size
	Size dummyContentAreaSize(10000, 10000);

	Size dummyWindowSize = calcWindowSizeFromContentAreaSize(dummyContentAreaSize);

	Size diffSize = dummyWindowSize - dummyContentAreaSize;

	Size contentAreaSize = windowSize - diffSize;

	if(contentAreaSize.width<0)
		contentAreaSize.width=0;
	if(contentAreaSize.height<0)
		contentAreaSize.height=0;

	return contentAreaSize;
}

Rect WindowCore::getScreenWorkArea() const
{
	HMONITOR	hMonitor = ::MonitorFromWindow(getHwnd(), MONITOR_DEFAULTTONEAREST);

	MONITORINFO monitorInfo = {0};

	monitorInfo.cbSize = sizeof(monitorInfo);
	if(!::GetMonitorInfo(hMonitor, &monitorInfo))
	{
		BDN_WIN32_throwLastError( ErrorFields().add("func", "MonitorFromWindow")
											.add("context", "WindowCore::getScreenWorkArea")  );
	}
	
	return win32RectToRect(monitorInfo.rcWork, getUiScaleFactor() );
}



void WindowCore::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_CLOSE)
	{
		::PostQuitMessage(0);
	}

	else if(message==WM_DPICHANGED)
	{
		WindowCore::dpiChanged( HIWORD(wParam), (const RECT*)lParam );
	}

	ViewCore::handleMessage(context, windowHandle, message, wParam, lParam);
}

}
}



