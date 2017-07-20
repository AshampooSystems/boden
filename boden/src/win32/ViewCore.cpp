#include <bdn/init.h>
#include <bdn/win32/ViewCore.h>

#include <bdn/win32/UiProvider.h>
#include <bdn/win32/util.h>

#include <bdn/PixelAligner.h>

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
					ViewCore::getViewParentHwndForChildren( pOuterView->getParentView() ),
					x,
					y,
					width,
					height )
{
	_outerViewWeak = pOuterView;

	_uiScaleFactor = 0;
    _emSizeDips = 0;
    

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

        _emSizeDips = _pFont->getSizePixels() / _uiScaleFactor;

	    ::SendMessage( getHwnd(), WM_SETFONT, (WPARAM)_pFont->getHandle(), FALSE);
    }
    else
        _emSizeDips = 0;
}


void	ViewCore::setVisible(const bool& visible)
{
	::ShowWindow( getHwnd(), visible ? SW_SHOW : SW_HIDE);		
}

void ViewCore::setPadding(const Nullable<UiMargin>& padding)
{
	// do nothing. We handle it on the fly when our preferred size is calculated.
}

void ViewCore::invalidateSizingInfo(View::InvalidateReason reason)
{
    // nothing to invalidate for ourselves (since we do not cache anything
    // in the core)
}

void ViewCore::needLayout(View::InvalidateReason reason)
{    
    P<View> pOuterView = getOuterViewIfStillAttached();
    if(pOuterView!=nullptr)
    {
        P<UiProvider> pProvider = tryCast<UiProvider>( pOuterView->getUiProvider() );
        if(pProvider!=nullptr)
            pProvider->getLayoutCoordinator()->viewNeedsLayout( pOuterView );
    }
}

void ViewCore::childSizingInfoInvalidated(View* pChild)
{
    P<View> pOuterView = getOuterViewIfStillAttached();
    if(pOuterView!=nullptr)
    {
        pOuterView->invalidateSizingInfo( View::InvalidateReason::childSizingInfoInvalidated );
        pOuterView->needLayout( View::InvalidateReason::childSizingInfoInvalidated );
    }
}

void ViewCore::layout()
{
    // do nothing in the base implementation. Most views do not have child views.
}


Rect ViewCore::adjustAndSetBounds(const Rect& requestedBounds)
{
    RECT winRect = rectToWin32Rect(requestedBounds, _uiScaleFactor);

    ::SetWindowPos(
        getHwnd(),
        NULL,
        winRect.left,
        winRect.top,
        winRect.right-winRect.left,
        winRect.bottom-winRect.top,
        SWP_NOACTIVATE | SWP_NOZORDER);

    return win32RectToRect( winRect, _uiScaleFactor );
}


Rect ViewCore::adjustBounds(const Rect& requestedBounds, RoundType positionRoundType, RoundType sizeRoundType ) const
{
    return PixelAligner( _uiScaleFactor ).alignRect(requestedBounds, positionRoundType, sizeRoundType);
}


void ViewCore::setHorizontalAlignment(const View::HorizontalAlignment& align)
{
    // do nothing. The View handles this.
}

void ViewCore::setVerticalAlignment(const View::VerticalAlignment& align)
{
    // do nothing. The View handles this.
}

void ViewCore::setPreferredSizeHint(const Size& hint)
{
    // do nothing in the base implementation. Most views do not use the size hint.
}

void ViewCore::setPreferredSizeMinimum(const Size& limit)
{
    // do nothing here. We use the property from the view directly when we need it.
}

void ViewCore::setPreferredSizeMaximum(const Size& limit)
{
    // do nothing here. We use the property from the view directly when we need it.
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

		if( getViewParentHwndForChildren(pNewParentView)==currentParentHwnd )
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
    switch( uiLength.unit )
    {
    case UiLength::Unit::none:
        return 0;

    case UiLength::Unit::dip:
        return uiLength.value;

    case UiLength::Unit::em:
        return uiLength.value * _emSizeDips;

    case UiLength::Unit::sem:
		return uiLength.value * getSemSizeDips();

    default:
		throw InvalidArgumentError("Invalid UiLength unit passed to ViewCore::uiLengthToDips: "+std::to_string((int)uiLength.unit) );
    }
}

Margin ViewCore::uiMarginToDipMargin(const UiMargin& margin) const
{
	return Margin(
            uiLengthToDips(margin.top),
            uiLengthToDips(margin.right),
            uiLengthToDips(margin.bottom),
            uiLengthToDips(margin.left) );
}


double ViewCore::getSemSizeDips() const
{
    if(_semSizeDipsIfInitialized==-1)
        _semSizeDipsIfInitialized = UiProvider::get().getSemSizeDips();

    return _semSizeDipsIfInitialized;
}



}
}

