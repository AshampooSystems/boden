#include <bdn/init.h>
#include <bdn/win32/ViewCore.h>

#include <bdn/win32/UiProvider.h>
#include <bdn/win32/util.h>

namespace bdn
{
namespace win32
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
					style | (pOuterView->visible().get() ? WS_VISIBLE : 0),
					exStyle,
					ViewCore::getViewHwnd( pOuterView->getParentView() ),
					x,
					y,
					width,
					height )
{
	_outerViewWeak = pOuterView;

	_uiScaleFactor = 0;

	View* pParentView = pOuterView->getParentView();
	if(pParentView==nullptr)
	{
		// we are a top level window without a parent. In that case the constructor
		// of the derived class will initialize the ui scale factor.
	}
	else
	{
		// we inherit the scale factor from our parent view

		P<ViewCore> pParentCore = cast<ViewCore>(pParentView->getViewCore());

		setUiScaleFactor( pParentCore->getUiScaleFactor() );
	}
}


ViewCore::~ViewCore()
{
}

void ViewCore::setUiScaleFactor(double factor)
{
	if(factor != _uiScaleFactor)
	{
		_uiScaleFactor = factor;

		// we must now update our font
		updateFont();

		// and we must also notify our child views.
        P<View> pOuter = getOuterViewIfStillAttached();
		std::list< P<View> > childViews;
        if(pOuter!=nullptr)
		    pOuter->getChildViews(childViews);

		for( const P<View>& pChildView: childViews)
		{
			P<ViewCore> pCore = cast<ViewCore>( pChildView->getViewCore() );
			if(pCore!=nullptr)
				pCore->setUiScaleFactor(factor);
		}
	}	
}


void ViewCore::updateFont()
{
    P<View> pOuter = getOuterViewIfStillAttached();
    if(pOuter!=nullptr)
    {
	    _pFont = UiProvider::get().getFontForView( pOuter, _uiScaleFactor);

	    ::SendMessage( getHwnd(), WM_SETFONT, (WPARAM)_pFont->getHandle(), FALSE);
    }
}


void	ViewCore::setVisible(const bool& visible)
{
	::ShowWindow( getHwnd(), visible ? SW_SHOW : SW_HIDE);		
}

void ViewCore::setPadding(const Nullable<UiMargin>& padding)
{
	// do nothing. We handle it on the fly when our preferred size is calculated.
}

void ViewCore::setPosition(const Point& pos)
{
    POINT winPoint = pointToWin32Point( pos, _uiScaleFactor);

	::SetWindowPos( getHwnd(), NULL, winPoint.x, winPoint.y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}


void ViewCore::setSize(const Size& size)
{
    SIZE winSize = sizeToWin32Size(size, _uiScaleFactor);

	::SetWindowPos( getHwnd(), NULL, 0, 0, winSize.cx, winSize.cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}


void ViewCore::setHorizontalAlignment(const View::HorizontalAlignment& align)
{
    // do nothing. The parent handles this.
}

void ViewCore::setVerticalAlignment(const View::VerticalAlignment& align)
{
    // do nothing. The parent handles this.
}


void ViewCore::updateOrderAmongSiblings()
{
    P<View> pOuter = getOuterViewIfStillAttached();
    if(pOuter!=nullptr)
    {
	    HWND ourHwnd = getHwnd();
	    if(ourHwnd!=NULL)
	    {
		    View* pParentView = pOuter->getParentView();

		    if(pParentView!=nullptr)
		    {		
			    View* pPrevSibling = pParentView->findPreviousChildView( pOuter );

			    if(pPrevSibling==nullptr)
			    {
				    // we are the first child
				    ::SetWindowPos(ourHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			    }
			    else
			    {
				    HWND prevSiblingHwnd = getViewHwnd(pPrevSibling);
						
				    if(prevSiblingHwnd!=NULL)
					    ::SetWindowPos(ourHwnd, prevSiblingHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			    }			
		    }
	    }
    }
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

	if(message==WM_SIZE)
	{
		// whenever our size changes it means that we have to update our layout
        P<View> pView = getOuterViewIfStillAttached();
        if(pView!=nullptr)
		    pView->needLayout();

        // we invalidate the window contents whenever the size changes. Otherwise
        // we have found that some controls (e.g. static text) are only partially updated (seen on Windows 10).
        ::InvalidateRect(windowHandle, NULL, NULL);
	}
	
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


double ViewCore::uiLengthToDips(const UiLength& uiLength) const
{
	return UiProvider::get().uiLengthToDips( uiLength, _uiScaleFactor );
}

Margin ViewCore::uiMarginToDipMargin(const UiMargin& margin) const
{
	return UiProvider::get().uiMarginToDipMargin( margin, _uiScaleFactor );
}


}
}

