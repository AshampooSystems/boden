#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/sysError.h>

namespace bdn
{
	
Window::Window()
{
}


void Window::create(	Window* pParent,
						const String& className,
						const String& name,
						DWORD style,
						DWORD exStyle,					
						int x,
						int y,
						int width,
						int height )
{
	WindowBase::createBase(
		className,
		name,
		style,
		exStyle,
		(pParent!=nullptr) ? pParent->getHandle()->getHwnd() : NULL,
		x,
		y,
		width,
		height );

	_pParentWeak = pParent;
	if(_pParentWeak!=nullptr)
		_pParentWeak->_childMap[_pHandle->getHwnd()] = this;

	finishInit();
}


void Window::finishInit()
{
	::SendMessage(_pHandle->getHwnd(), WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), TRUE);

	_pText = newObj<WindowTextProperty>(_pHandle);
	_pVisible = newObj<WindowVisibleProperty>(_pHandle);
}


void Window::notifyDestroy()
{
	WindowBase::notifyDestroy();

	if(_pHandle!=nullptr && _pParentWeak!=nullptr)
		_pParentWeak->_childMap.erase( _pHandle->getHwnd() );
}

void Window::handleParentMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	// do nothing by default.
}

void Window::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* pChild = findChildForMessage(message, wParam, lParam);
	if(pChild!=nullptr)
		pChild->handleParentMessage(context, windowHandle, message, wParam, lParam);
}


P<Window> Window::findChildByHwnd(HWND hwnd)
{
	auto it = _childMap.find(hwnd);
	if(it!=_childMap.end())
		return it->second;
	else
		return nullptr;
}


P<Window> Window::findChildForMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND childHwnd = NULL;

	if(message==WM_COMMAND)
		childHwnd = (HWND)lParam;

	if(childHwnd!=NULL)
		return findChildByHwnd(childHwnd);
	
	return nullptr;
}




}

