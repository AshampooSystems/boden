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
        Size clientSizeWithoutScrollbars = win32RectToRect(winRect, uiScaleFactor).getSize();

        // subtract the size of the border.
        clientSizeWithoutScrollbars.width -= _nonClientMargins.left + _nonClientMargins.right;
        clientSizeWithoutScrollbars.height -= _nonClientMargins.top + _nonClientMargins.bottom;

        if(clientSizeWithoutScrollbars.width<0)
            clientSizeWithoutScrollbars.width = 0;
        if(clientSizeWithoutScrollbars.height<0)
            clientSizeWithoutScrollbars.height = 0;
        
        ScrollViewLayoutHelper helper(vertBarWidth, horzBarHeight);    

        helper.calcLayout(pOuterView, clientSizeWithoutScrollbars );

        _showHorizontalScrollBar = helper.getHorizontalScrollBarVisible();
        _showVerticalScrollBar = helper.getVerticalScrollBarVisible();

        Rect contentViewBounds = helper.getContentViewBounds();
        Size scrolledAreaSize = helper.getScrolledAreaSize();
        Size viewPortSize = helper.getViewPortSize();
        
        SIZE scrollAreaSizeInPixels = sizeToWin32Size(scrolledAreaSize, uiScaleFactor);
        SIZE viewPortSizeInPixels = sizeToWin32Size(viewPortSize, uiScaleFactor);
        
        // update our style so that the scroll bars are added / removed
        POINT scrollPos = updateScrollInfo(_showHorizontalScrollBar, _showVerticalScrollBar, viewPortSizeInPixels, scrollAreaSizeInPixels);
                
        ::SetWindowPos(
            _pContentContainer->getHwnd(),
            NULL,
            -scrollPos.x,
            -scrollPos.y,
            scrollAreaSizeInPixels.cx,
            scrollAreaSizeInPixels.cy,
            SWP_NOZORDER | SWP_NOOWNERZORDER);

        P<View> pContentView = pOuterView->getContentView();
        if(pContentView!=nullptr)
            pContentView->adjustAndSetBounds(contentViewBounds);
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


POINT ScrollViewCore::updateScrollInfo(bool horzScrollBar, bool vertScrollBar, const SIZE& viewPortSizeInPixels, const SIZE& fullContentSizeInPixels)
{
    POINT scrollPos{0,0};

    HWND     hwnd = getHwnd();
    if(hwnd==NULL)
    {
        // our window is already destroyed. Nothing to do.
    }
    else
    {
        LONG_PTR currStyle = ::GetWindowLongPtr( hwnd, GWL_STYLE );
    
        LONG_PTR newStyle = currStyle & (~(WS_VSCROLL | WS_HSCROLL));

        if(horzScrollBar)
            newStyle |= WS_HSCROLL;
        if(vertScrollBar)
            newStyle |= WS_VSCROLL;

        if(newStyle!=currStyle)
        {
            ::SetWindowLongPtr( hwnd, GWL_STYLE, newStyle);
            ::SetWindowPos( hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        }

        scrollPos = getScrollPositionInPixels();

        SCROLLINFO scrollInfo;
        memset(&scrollInfo, 0, sizeof(scrollInfo));
        
        scrollInfo.cbSize = sizeof(scrollInfo);
        scrollInfo.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;

        scrollInfo.nMin = 0;
                
        if(horzScrollBar)
        {
            scrollInfo.nMax = fullContentSizeInPixels.cx;
            scrollInfo.nPage = viewPortSizeInPixels.cx;

            int maxPos = scrollInfo.nMax - scrollInfo.nPage;  
            if(maxPos<0)
                maxPos=0;
            if(scrollPos.x>maxPos)
                scrollPos.x = maxPos;            
        }
        else
        {
            scrollInfo.nMax = 0;
            scrollInfo.nPage = 0;

            scrollPos.x = 0;
        }

        scrollInfo.nPos = scrollPos.x;

        ::SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, TRUE);  // TRUE means redraw scrollbar
            

        if(vertScrollBar)
        {
            scrollInfo.nMax = fullContentSizeInPixels.cy;
            scrollInfo.nPage = viewPortSizeInPixels.cy;

            int maxPos = scrollInfo.nMax - scrollInfo.nPage;
            if(maxPos<0)
                maxPos=0;
            if(scrollPos.y>maxPos)
                scrollPos.y = maxPos;
        }
        else
        {
            scrollInfo.nMax = 0;
            scrollInfo.nPage = 0;

            scrollPos.y = 0;
        }

        scrollInfo.nPos = scrollPos.y;

        ::SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);  // TRUE means redraw scrollbar        
    }

    return scrollPos;
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

        // and make it the actual position. If we do not do this then the scrollbar will
        // jump back to the old position when the users stops scrolling.

        info.fMask = SIF_POS;
 
        ::SetScrollInfo(windowHandle, barType, &info, TRUE);  // TRUE means redraw scrollbar
        
        POINT scrollPos = getScrollPositionInPixels(ScrollPosType::current);
        updateContentContainerPos(scrollPos);

        context.setResult(0, false);
    }
    else
        ViewCore::handleMessage(context, windowHandle, message, wParam, lParam);
}

void ScrollViewCore::updateContentContainerPos(const POINT& scrollPosInPixels)
{
    ::SetWindowPos( _pContentContainer->getHwnd(), NULL, -scrollPosInPixels.x, -scrollPosInPixels.y, 0, 0, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE );        
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


void ScrollViewCore::scrollAreaToVisible(const Rect& area)
{
    HWND hwnd = getHwnd();
    if(hwnd!=NULL)
    {
        RECT viewPortRect = {0};
        ::GetClientRect( hwnd, &viewPortRect);

        int viewPortWidth = viewPortRect.right - viewPortRect.left;
        int viewPortHeight = viewPortRect.bottom - viewPortRect.top;

        POINT         scrollPos{0,0};
        
        SCROLLINFO horzScrollInfo;
        memset(&horzScrollInfo, 0, sizeof(horzScrollInfo));
        horzScrollInfo.cbSize = sizeof(horzScrollInfo);
        horzScrollInfo.fMask = SIF_POS | SIF_RANGE;

        SCROLLINFO vertScrollInfo = horzScrollInfo;
        
        if(::GetScrollInfo(hwnd, SB_HORZ, &horzScrollInfo))
            scrollPos.x = horzScrollInfo.nPos;

        if(::GetScrollInfo(hwnd, SB_VERT, &vertScrollInfo))
            scrollPos.y = vertScrollInfo.nPos;
    
        RECT currVisibleRect;
        currVisibleRect.left = scrollPos.x;
        currVisibleRect.top = scrollPos.y;
        currVisibleRect.right = scrollPos.x+viewPortWidth;
        currVisibleRect.bottom = scrollPos.y+viewPortHeight;

        double scaleFactor = getUiScaleFactor();

        RECT targetRect = rectToWin32Rect(area, scaleFactor );
        
        if(targetRect.right > currVisibleRect.right )
            scrollPos.x = targetRect.right - viewPortWidth;
        if( targetRect.left<currVisibleRect.left )
            scrollPos.x = targetRect.left;


        if(targetRect.bottom > currVisibleRect.bottom )
            scrollPos.y = targetRect.bottom - viewPortHeight;
        if( targetRect.top<currVisibleRect.top )
            scrollPos.y = targetRect.top;

        if(scrollPos.x<0)
            scrollPos.x = 0;
        if(scrollPos.x > horzScrollInfo.nMax )
            scrollPos.x = horzScrollInfo.nMax;

        if(scrollPos.y<0)
            scrollPos.y = 0;
        if(scrollPos.y > vertScrollInfo.nMax )
            scrollPos.y = vertScrollInfo.nMax;

        if(scrollPos.x != horzScrollInfo.nPos)
        {
            // scroll position has changed
            horzScrollInfo.nPos = scrollPos.x;

            ::SetScrollInfo(hwnd, SB_HORZ, &horzScrollInfo);
        }

        if(scrollPos.y != vertScrollInfo.nPos)
        {
            // scroll position has changed
            horzScrollInfo.nPos = scrollPos.x;

            ::SetScrollInfo(hwnd, SB_HORZ, &horzScrollInfo);
        }


        

                

        HWND contentContainerHwnd = _pContentContainer->getHwnd();
        if(contentContainerHwnd!=NULL)
        {
            RECT contentContainerRect = {0};
            ::GetClientRect( contentContainerHwnd, &contentContainerRect);









    
}


}
}



