#include <bdn/init.h>
#include <bdn/win32/ScrollViewCore.h>

#include <bdn/win32/util.h>
#include <bdn/ScrollViewLayoutHelper.h>


namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( ScrollViewCore::ScrollViewCoreClass, ScrollViewCore::ScrollViewCoreClass::get );
BDN_SAFE_STATIC_IMPL( ScrollViewCore::ContentContainerClass, ScrollViewCore::ContentContainerClass::get );


ScrollViewCore::ScrollViewCore(ScrollView* pOuter)
    : ViewCore( pOuter,
                ScrollViewCore::ScrollViewCoreClass::get().getName(),
				"ScrollView",
				WS_CHILD,
				0)
{
    // we need to know the size of our nonclient border for various things. Calculate that

    HWND hwnd = getHwnd();
    if(hwnd!=NULL)
    {
        RECT rect{0,0,0,0};
        ::AdjustWindowRect( &rect, ::GetWindowLongPtr( getHwnd(), GWL_STYLE), FALSE );
        _nonClientMargins.left = -rect.left;
        _nonClientMargins.top = -rect.top;
        _nonClientMargins.right = rect.right;
        _nonClientMargins.bottom = rect.bottom;    
    }

    _pContentContainer = newObj<ContentContainer>( getHwnd() );
}

ScrollViewCore::ContentContainer::ContentContainer(HWND parentHwnd)
    : Win32Window(  ScrollViewCore::ContentContainerClass::get().getName(),
                    "ScrollViewContentContainer",
                    WS_CHILD | WS_VISIBLE,
                    0,
                    parentHwnd )
{
}


HWND ScrollViewCore::getParentHwndForChildren() const
{
    if(_pContentContainer!=nullptr)
        return _pContentContainer->getHwnd();
    else
        return NULL;
}

        
void ScrollViewCore::setHorizontalScrollingEnabled(const bool& enabled)
{
    // we don't care. The setting is only used in layout and there we get it from
    // the outer view directly.
}

void ScrollViewCore::setVerticalScrollingEnabled(const bool& enabled)
{
    // we don't care. The setting is only used in layout and there we get it from
    // the outer view directly.
}



void ScrollViewCore::layout()
{
    P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );
    HWND          hwnd = getHwnd();

    if(pOuterView==nullptr || hwnd==NULL)
    {
        // the outer view has already been destroyed or our window has already been destroyed.
        // No layout necessary. Do nothing.
    }
    else
    {
        // these values are unscaled (i.e. for scale factor 1). That means that
        // they are in DIPs.
        int     horzBarHeight = ::GetSystemMetrics(SM_CYHSCROLL);
        int     vertBarWidth = ::GetSystemMetrics(SM_CXVSCROLL);
        
        double  uiScaleFactor = getUiScaleFactor();
                
        // the client rect automatically excludes the space needed for the scroll bars.
        // Subtracting it manually does not work properly in all cases since there seems to be short
        // time period in which the client rect is not yet updated after a scrollbar is hidden or shown.
        // So instead we use the window size, not the client size:

        RECT winRect;
        ::GetWindowRect(hwnd, &winRect);
        Size viewPortSizeWithoutScrollbars = win32RectToRect(winRect, uiScaleFactor).getSize();

        // subtract the size of the border.
        viewPortSizeWithoutScrollbars.width -= _nonClientMargins.left + _nonClientMargins.right;
        viewPortSizeWithoutScrollbars.height -= _nonClientMargins.top + _nonClientMargins.bottom;

        if(viewPortSizeWithoutScrollbars.width<0)
            viewPortSizeWithoutScrollbars.width = 0;
        if(viewPortSizeWithoutScrollbars.height<0)
            viewPortSizeWithoutScrollbars.height = 0;
        
        ScrollViewLayoutHelper helper(vertBarWidth, horzBarHeight);    

        helper.calcLayout(pOuterView, viewPortSizeWithoutScrollbars );

        _showHorizontalScrollBar = helper.getHorizontalScrollBarVisible();
        _showVerticalScrollBar = helper.getVerticalScrollBarVisible();

        Rect contentViewBounds = helper.getContentViewBounds();
        Size clientSize = helper.getScrolledAreaSize();
        Size viewPortSize = helper.getViewPortSize();
        
        _clientSizePixels = sizeToWin32Size(clientSize, uiScaleFactor);               
        _viewPortSizePixels = sizeToWin32Size(viewPortSize, uiScaleFactor);

        // make sure that our scroll position is valid
        if(_showHorizontalScrollBar)
        {
            int maxPos = _clientSizePixels.cx - _viewPortSizePixels.cx;
            if(maxPos<0)
                maxPos=0;
            if(_scrollPositionPixels.x>maxPos)
                _scrollPositionPixels.x = maxPos;
        }
        else
            _scrollPositionPixels.x = 0;

        if(_showVerticalScrollBar)
        {
            int maxPos = _clientSizePixels.cy - _viewPortSizePixels.cy;
            if(maxPos<0)
                maxPos=0;
            if(_scrollPositionPixels.y>maxPos)
                _scrollPositionPixels.y = maxPos;
        }
        else
            _scrollPositionPixels.y = 0;

        // update our style so that the scroll bars are added / removed
        // and the scroll infos.
        updateWin32ScrollInfo();

        // move and resize the content container
        ::SetWindowPos(
            _pContentContainer->getHwnd(),
            NULL,
            -_scrollPositionPixels.x,
            -_scrollPositionPixels.y,
            _clientSizePixels.cx,
            _clientSizePixels.cy,
            SWP_NOZORDER | SWP_NOOWNERZORDER);

        P<View> pContentView = pOuterView->getContentView();
        if(pContentView!=nullptr)
            pContentView->adjustAndSetBounds(contentViewBounds);        

        updateVisibleClientRect();
    }
}
	
Size ScrollViewCore::calcPreferredSize( const Size& availableSpace ) const
{
    P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );
    HWND          hwnd = getHwnd();

    if(pOuterView==nullptr || hwnd==NULL)
    {
        // the outer view has already been destroyed or our window has already been destroyed.
        return Size();
    }
    else
    {
        int     horzBarHeight = ::GetSystemMetrics(SM_CYHSCROLL);
        int     vertBarWidth = ::GetSystemMetrics(SM_CXVSCROLL);

        ScrollViewLayoutHelper helper(vertBarWidth, horzBarHeight);    

        return helper.calcPreferredSize(pOuterView, availableSpace );
    }
}


void ScrollViewCore::updateWin32ScrollInfo()
{
    HWND     hwnd = getHwnd();
    if(hwnd==NULL)
    {
        // our window is already destroyed. Nothing to do.
    }
    else
    {
        LONG_PTR currStyle = ::GetWindowLongPtr( hwnd, GWL_STYLE );
    
        LONG_PTR newStyle = currStyle & (~(WS_VSCROLL | WS_HSCROLL));

        if(_showHorizontalScrollBar)
            newStyle |= WS_HSCROLL;
        if(_showVerticalScrollBar)
            newStyle |= WS_VSCROLL;

        if(newStyle!=currStyle)
        {
            ::SetWindowLongPtr( hwnd, GWL_STYLE, newStyle);
            ::SetWindowPos( hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        }
        
        SCROLLINFO scrollInfo;
        memset(&scrollInfo, 0, sizeof(scrollInfo));
        
        scrollInfo.cbSize = sizeof(scrollInfo);
        scrollInfo.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;

        scrollInfo.nMin = 0;
                
        if(_showHorizontalScrollBar)
        {
            scrollInfo.nMax = _clientSizePixels.cx;
            scrollInfo.nPage = _viewPortSizePixels.cx;
        }
        else
        {
            scrollInfo.nMax = 0;
            scrollInfo.nPage = 0;
        }

        scrollInfo.nPos = _scrollPositionPixels.x;

        ::SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, TRUE);  // TRUE means redraw scrollbar
            

        if(_showVerticalScrollBar)
        {
            scrollInfo.nMax = _clientSizePixels.cy;
            scrollInfo.nPage = _viewPortSizePixels.cy;
        }
        else
        {
            scrollInfo.nMax = 0;
            scrollInfo.nPage = 0;
        }

        scrollInfo.nPos = _scrollPositionPixels.y;

        ::SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);  // TRUE means redraw scrollbar        
    }
}


void ScrollViewCore::handleMessage(MessageContext& context, HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message==WM_VSCROLL || message==WM_HSCROLL)
    {
        int barType = (message==WM_VSCROLL) ? SB_VERT : SB_HORZ;

        SCROLLINFO info;
        memset(&info, 0, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = SIF_RANGE | SIF_PAGE;

        // these scroll messages are sent in various circumstances.
        // If action is SB_THUMBTRACK or SB_THUMBPOSITION then the tracking
        // pos will already be the desired new scroll position.
        // If action is anything else then we must calculate the position
        // ourselves.

        int     action = LOWORD(wParam);
        
        bool    basePosIsTrackPos = ( action==SB_THUMBPOSITION || action==SB_THUMBTRACK );

        if(basePosIsTrackPos)
            info.fMask |= SIF_TRACKPOS;
        else
            info.fMask |= SIF_POS;

        if(!::GetScrollInfo(windowHandle, barType, &info))
        {
            info.nPos = 0;
            info.nTrackPos = 0;
            info.nMax = 0;
            info.nPage = 0;
        }

        if(basePosIsTrackPos)
            info.nPos = info.nTrackPos;

        int maxPos = info.nMax - info.nPage;
        if(maxPos<0)
            maxPos = 0;
        
        // now see how we need to modify the position

        if(action==SB_LINEDOWN)
        {
            // scroll 16 dips down / to the right
            info.nPos += std::lround(16 * getUiScaleFactor() );
        }
        else if(action==SB_LINEUP)
        {
            // scroll 16 dips up / to the left
            info.nPos -= std::lround(16 * getUiScaleFactor() );
        }
        else if(action==SB_PAGEUP)
            info.nPos -= info.nPage;

        else if(action==SB_PAGEDOWN)
            info.nPos += info.nPage;

        else if(action==SB_BOTTOM)
            info.nPos = maxPos;

        else if(action==SB_TOP)
            info.nPos = 0;

        else if(action==SB_THUMBPOSITION || action==SB_THUMBTRACK)
        {
            // position is already set. Nothing to do.
        }
        else if(action==SB_ENDSCROLL)
        {
            // nothing to do - we handle each scroll action immediately.
            // Just fall through and set the current tracking position as the
            // scroll position - this should not change anything.
        }
        else
        {
            // unknown action. Let this through to the default implementation so that
            // Windows can detect that we do not support this type of scrolling (and
            // can possibly give the user feedback of some kind)
            ViewCore::handleMessage(context, windowHandle, message, wParam, lParam);
            return;
        }              
        
        if(info.nPos<0)
            info.nPos=0;
        if(info.nPos > maxPos)
            info.nPos = maxPos;

        if(barType==SB_VERT)
            _scrollPositionPixels.y = info.nPos;
        else
            _scrollPositionPixels.x = info.nPos;

        // and make it the actual position. If we do not do this then the scrollbar will
        // jump back to the old position when the users stops scrolling.

        info.fMask = SIF_POS; 
        ::SetScrollInfo(windowHandle, barType, &info, TRUE);  // TRUE means redraw scrollbar
                
        updateContentContainerPos();
        updateVisibleClientRect();

        context.setResult(0, false);
    }
    else
        ViewCore::handleMessage(context, windowHandle, message, wParam, lParam);
}

void ScrollViewCore::updateContentContainerPos()
{
    ::SetWindowPos( _pContentContainer->getHwnd(), NULL, -_scrollPositionPixels.x, -_scrollPositionPixels.y, 0, 0, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE );        
}

POINT ScrollViewCore::getScrollPositionInPixels(ScrollViewCore::ScrollPosType scrollPosType)
{
    POINT         scrollPos{0,0};
    HWND          hwnd = getHwnd();

    if(hwnd!=NULL)
    {
        SCROLLINFO info;
        memset(&info, 0, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = (scrollPosType==ScrollPosType::track) ? SIF_TRACKPOS : SIF_POS;
        
        if(::GetScrollInfo(hwnd, SB_HORZ, &info))
            scrollPos.x = (scrollPosType==ScrollPosType::track) ? info.nTrackPos : info.nPos;

        if(::GetScrollInfo(hwnd, SB_VERT, &info))
            scrollPos.y = (scrollPosType==ScrollPosType::track) ? info.nTrackPos : info.nPos;        
    }

    return scrollPos;
}


void ScrollViewCore::scrollClientRectToVisible(const Rect& rect)
{
    HWND hwnd = getHwnd();
    if(hwnd!=NULL)
    {
        POINT         newScrollPos = _scrollPositionPixels;

        RECT currVisibleRect;
        currVisibleRect.left = _scrollPositionPixels.x;
        currVisibleRect.top = _scrollPositionPixels.y;
        currVisibleRect.right = _scrollPositionPixels.x + _viewPortSizePixels.cx;
        currVisibleRect.bottom = _scrollPositionPixels.y + _viewPortSizePixels.cy;

        POINT maxScrollPos{ _clientSizePixels.cx - _viewPortSizePixels.cx,
                            _clientSizePixels.cy - _viewPortSizePixels.cy };

        if(maxScrollPos.x < 0)
            maxScrollPos.x = 0;
        if(maxScrollPos.y < 0)
            maxScrollPos.y = 0;
        
        double scaleFactor = getUiScaleFactor();

        RECT targetRect = rectToWin32Rect(rect, scaleFactor );

        // handle infinity positions.
        if( !std::isfinite(rect.x) )
        {
            targetRect.left = (rect.x>0) ? _clientSizePixels.cx : 0;
            targetRect.right = targetRect.left;
        }
        if( !std::isfinite(rect.y) )
        {
            targetRect.top = (rect.y>0) ? _clientSizePixels.cy : 0;
            targetRect.bottom = targetRect.top;
        }

        // now we clip the target rect to the client area
        if(targetRect.right > _clientSizePixels.cx)
            targetRect.right = _clientSizePixels.cx;
        if(targetRect.left > _clientSizePixels.cx)
            targetRect.left = _clientSizePixels.cx;
        if(targetRect.right < 0)
            targetRect.right = 0;
        if(targetRect.left < 0)
            targetRect.left = 0;

        if(targetRect.bottom > _clientSizePixels.cy)
            targetRect.bottom = _clientSizePixels.cy;
        if(targetRect.top > _clientSizePixels.cy)
            targetRect.top = _clientSizePixels.cy;
        if(targetRect.bottom < 0)
            targetRect.bottom = 0;
        if(targetRect.top < 0)
            targetRect.top = 0;

        // if the target rect is bigger than the visible rect
        // then we want to scroll as little as possible, to
        // fill the viewport with ANY part of the target rect.

        if( targetRect.right-targetRect.left > _viewPortSizePixels.cx)
        {
            if( currVisibleRect.left>=targetRect.left && currVisibleRect.right<=targetRect.right)
            {
                // if the current visible rect is already inside the target rect then
                // we do not want to scroll
                targetRect.left = currVisibleRect.left;
                targetRect.right = currVisibleRect.right;
            }
            else
            {
                // we want to scroll towards the of the target rect that is closest
                int leftDistance = std::abs( targetRect.left - currVisibleRect.left );
                int rightDistance = std::abs( targetRect.right - currVisibleRect.right );

                if(rightDistance<leftDistance)
                    targetRect.left = targetRect.right - _viewPortSizePixels.cx;
                else
                    targetRect.right = targetRect.left + _viewPortSizePixels.cx;
            }
        }

        if( targetRect.bottom-targetRect.top > _viewPortSizePixels.cy)
        {
            if( currVisibleRect.top>=targetRect.top && currVisibleRect.bottom<=targetRect.bottom)
            {
                targetRect.top = currVisibleRect.top;
                targetRect.bottom = currVisibleRect.bottom;
            }
            else
            {
                int topDistance = std::abs( targetRect.top - currVisibleRect.top );
                int bottomDistance = std::abs( targetRect.bottom - currVisibleRect.bottom );

                if(bottomDistance<topDistance)
                    targetRect.top = targetRect.bottom - _viewPortSizePixels.cy;
                else
                    targetRect.bottom = targetRect.top + _viewPortSizePixels.cy;
            }
        }


        if(targetRect.right > currVisibleRect.right )
            newScrollPos.x = targetRect.right - _viewPortSizePixels.cx;
        if( targetRect.left < currVisibleRect.left )
            newScrollPos.x = targetRect.left;
                        
        if(targetRect.bottom > currVisibleRect.bottom )
            newScrollPos.y = targetRect.bottom - _viewPortSizePixels.cy;
        if( targetRect.top < currVisibleRect.top )
            newScrollPos.y = targetRect.top;

        if(newScrollPos.x<0)
            newScrollPos.x = 0;
        if(newScrollPos.x > maxScrollPos.x )
            newScrollPos.x = maxScrollPos.x;

        if(newScrollPos.y<0)
            newScrollPos.y = 0;
        if(newScrollPos.y > maxScrollPos.y )
            newScrollPos.y = maxScrollPos.y;

        if(newScrollPos.x !=_scrollPositionPixels.x
            || newScrollPos.y != _scrollPositionPixels.y)
        {
            SCROLLINFO info;
            memset(&info, 0, sizeof(info));
            info.cbSize = sizeof(info);
            info.fMask = SIF_POS;

            if(newScrollPos.x != _scrollPositionPixels.x)
            {
                // horizontal scroll position has changed
                info.nPos = newScrollPos.x;
                ::SetScrollInfo(hwnd, SB_HORZ, &info, TRUE);            
            }

            if(newScrollPos.y != _scrollPositionPixels.y)
            {
                // vertical scroll position has changed
                info.nPos = newScrollPos.y;
                ::SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
            }

            _scrollPositionPixels = newScrollPos;

            updateContentContainerPos();
            updateVisibleClientRect();
        }
    }
}


void ScrollViewCore::updateVisibleClientRect()
{
    P<ScrollView> pOuterView = cast<ScrollView>( getOuterViewIfStillAttached() );
    if(pOuterView!=nullptr)
    {
        double scaleFactor = getUiScaleFactor();

        Point scrollPos = win32PointToPoint( _scrollPositionPixels, scaleFactor );
        Size  viewPortSize = win32SizeToSize( _viewPortSizePixels, scaleFactor );

        Rect  visibleClientRect(scrollPos, viewPortSize);

        pOuterView->_setVisibleClientRect( visibleClientRect );
    }
}

}
}



