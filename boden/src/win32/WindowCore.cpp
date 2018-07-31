#include <bdn/init.h>
#include <bdn/win32/WindowCore.h>

#include <bdn/win32/win32Error.h>
#include <bdn/win32/hresultError.h>
#include <bdn/NotImplementedError.h>
#include <bdn/win32/util.h>
#include <bdn/windowCoreUtil.h>

#include <bdn/win32/UiProvider.h>

#include <ShellScalingApi.h>

namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( WindowCore::WindowCoreClass, WindowCore::WindowCoreClass::get );


WindowCore::WindowCore(Window* pWindow)
	: ViewCore(	pWindow,
				WindowCore::WindowCoreClass::get().getName(),
				pWindow->title(),
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

void WindowCore::sizeChanged(int changeType )
{
	// whenever our size changes it means that we have to update our layout
    P<View> pView = getOuterViewIfStillAttached();
    if(pView!=nullptr)
		pView->needLayout( View::InvalidateReason::standardPropertyChanged );
}

void	WindowCore::setTitle(const String& title)
{
	Win32Window::setWindowText( getHwnd(), title );
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

Margin WindowCore::getNonClientMargin() const
{
    // In Win32 there is AdjustWindowRect to get the window size from the content area size,
    // given a set of style flags.

    // We use that to extract the size of the non-client border. Note that this will give
    // an incorrect result if Windows ever decides to adapt the border size to the client area size
    // (i.e. if bigger windows get a bigger border, for example). If something like that happens
    // then we will have to adapt calcPreferredSize and adjustAndSetBounds accordingly.
    
    RECT contentRect{500, 500, 800, 600};
	DWORD style = ::GetWindowLongW(getHwnd(), GWL_STYLE);
	DWORD exStyle = ::GetWindowLongW(getHwnd(), GWL_EXSTYLE);

	HMENU menuHandle = ::GetMenu( getHwnd() );

    RECT windowRect = contentRect;

	if(!::AdjustWindowRectEx( &windowRect, style, (menuHandle!=NULL) ? TRUE : FALSE, exStyle ) )
	{
		BDN_WIN32_throwLastError( ErrorFields().add("func", "AdjustWindowRectEx")
											.add("action", "WindowCore::getNonClientMargin") );
	}

    // the resulting rect is for an unspecified display, i.e. windows will assume
    // that the display's scale factor is 1. I.e. it is in DIPs.

    return Margin(
        contentRect.top-windowRect.top,
        windowRect.right-contentRect.right,
        windowRect.bottom-contentRect.bottom,
        contentRect.left-windowRect.left );
}
    
Size WindowCore::calcPreferredSize( const Size& availableSpace ) const
{
    P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
    if(pWindow!=nullptr)
        return defaultWindowCalcPreferredSizeImpl( pWindow, availableSpace, getNonClientMargin(), getMinimumSize() );
    else
        return getMinimumSize();
}

void WindowCore::layout()
{
    P<Window> pWindow = cast<Window>( getOuterViewIfStillAttached() );
    if(pWindow!=nullptr)
    {    
        RECT clientRect;
        ::GetClientRect( getHwnd(), &clientRect);

        defaultWindowLayoutImpl( pWindow, win32RectToRect(clientRect, getUiScaleFactor() ) );
    }
}

void WindowCore::requestAutoSize()
{
    P<Window> pOuterView = cast<Window>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
    {
        P<UiProvider> pProvider = tryCast<UiProvider>( pOuterView->getUiProvider() );
        if(pProvider!=nullptr)
            pProvider->getLayoutCoordinator()->windowNeedsAutoSizing( pOuterView );
    }
}

void WindowCore::requestCenter()
{
    P<Window> pOuterView = cast<Window>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
    {
        P<UiProvider> pProvider = tryCast<UiProvider>( pOuterView->getUiProvider() );
        if(pProvider!=nullptr)
            pProvider->getLayoutCoordinator()->windowNeedsCentering( pOuterView );
    }
}
	

void WindowCore::autoSize()
{
    P<Window> pOuterView = cast<Window>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
        defaultWindowAutoSizeImpl( pOuterView, getScreenWorkArea().getSize() );
}

void WindowCore::center()
{
    P<Window> pOuterView = cast<Window>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
        defaultWindowCenterImpl( pOuterView, getScreenWorkArea() );
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

    else if(message==WM_SIZE)
	{
        WindowCore::sizeChanged(wParam);

        // we invalidate the window contents whenever the size changes. Otherwise
        // we have found that some controls (e.g. static text) are only partially updated (seen on Windows 10).
        ::InvalidateRect(windowHandle, NULL, NULL);
	}
	

	ViewCore::handleMessage(context, windowHandle, message, wParam, lParam);
}

}
}



