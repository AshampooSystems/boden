#include <bdn/init.h>
#include <bdn/WindowCore.h>

#include <bdn/sysError.h>
#include <bdn/Win32UiProvider.h>

#include <ShellScalingApi.h>

namespace bdn
{


WindowCore::WindowCore(Window* pWindow)
	: ViewCore(	pWindow,
				WindowCoreClass::get()->getName(),
				pWindow->title().get(),
				WS_OVERLAPPEDWINDOW | WS_POPUPWINDOW,
				WS_EX_APPWINDOW,
				200,
				200,
				300,
				200 )
{
	_uiScaleFactor = 0;

	initUiScaleFactor();
}

void WindowCore::initUiScaleFactor()
{
	HMONITOR	hMonitor = ::MonitorFromWindow(getHwnd(), MONITOR_DEFAULTTONEAREST);

	UINT dpiX = 0;
	UINT dpiY = 0;
	HRESULT result = ::GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX ,&dpiY);
	if(FAILED(result))
	{
		throwHresultError( result, ErrorFields().add("func", "GetDpiForMonitor")
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
		Rect newRect(pSuggestedNewRect->left, pSuggestedNewRect->top, pSuggestedNewRect->right-pSuggestedNewRect->left, pSuggestedNewRect->bottom-pSuggestedNewRect->top);

		_pOuterViewWeak->bounds() = newRect;
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

	return Rect(clientRect.left, clientRect.top, clientRect.right-clientRect.left, clientRect.bottom-clientRect.top);
}


void WindowCore::layout()
{
	P<View> pContentView = cast<Window>(_pOuterViewWeak)->getContentView();

	if(pContentView==nullptr)
	{
		// nothing to do.
		return;
	}

	// just set our content window to the full client size.

	RECT clientRect;
	::GetClientRect(getHwnd(), &clientRect);

	Rect contentRect(clientRect.left, clientRect.top, clientRect.right-clientRect.left, clientRect.bottom-clientRect.top);

	// subtract our padding
	P<Win32UiProvider> pUiProvider = Win32UiProvider::get();
	contentRect -= pUiProvider->uiMarginToPixelMargin( _pOuterViewWeak->padding(), _uiScaleFactor );

	// subtract the content view's margins
	contentRect -= pUiProvider->uiMarginToPixelMargin( pContentView->margin(), _uiScaleFactor );
	
	pContentView->bounds() = contentRect;

	// note that we do not need to layout the content view. It will automatically
	// re-layout itself, if its size has changed.
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



