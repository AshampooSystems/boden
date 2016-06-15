#include <bdn/init.h>
#include <bdn/ViewCore.h>

namespace bdn
{

ViewCore::ViewCore(	View* pOuterView,
					const String& className,
					const String& name,
					DWORD style,
					DWORD exStyle,
					int x,
					int y,
					int width,
					int height )
	: Win32Window(	className,
					name,
					style,
					exStyle,
					ViewCore::getViewHwnd( pOuterView->getParentView() ),
					x,
					y,
					width,
					height )
{
	_pOuterViewWeak = pOuterView;
}


bool ViewCore::tryChangeParentView(View* pNewParentView)
{
	HWND ourHwnd = getHwnd();
	if(ourHwnd==NULL)
	{
		// Our win32 window is already destroyed. Return false.
		return false;
	}
	else
	{
		HWND currentParentHwnd = ::GetParent( ourHwnd );

		if( getViewHwnd(pNewParentView)==currentParentHwnd )
		{
			// the underlying win32 window is the same.
			// Note that this case is quite common, since many View containers
			// are actually light objects that do not have their own backend
			// Win32 window. So often the Win32 window of some higher ancestor is
			// shared by many windows.

			// This also happens when a child view's ordering position is changed inside
			// the parent. So we have to check if we need to change something there.
			updateOrderAmongSiblings();

			return true;
		}
		else
		{
			// we cannot move a window to a new underlying win32 window.
			return false;
		}
	}
}


	
void ViewCore::handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	// do nothing by default.
}

void ViewCore::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	ViewCore* pChildCore = findChildCoreForMessage(message, wParam, lParam);
	if(pChildCore!=nullptr)
		pChildCore->handleParentMessage(context, windowHandle, message, wParam, lParam);

	Win32Window::handleMessage(context, windowHandle, message, wParam, lParam);
}


P<ViewCore> ViewCore::findChildCoreForMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND childHwnd = NULL;

	if(message==WM_COMMAND)
		childHwnd = (HWND)lParam;

	if(childHwnd!=NULL)
		return cast<ViewCore>( getObjectFromHwnd(childHwnd) );
	else	
		return nullptr;
}


}

