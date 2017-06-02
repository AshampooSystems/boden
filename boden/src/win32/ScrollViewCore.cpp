#include <bdn/init.h>
#include <bdn/win32/ScrollViewCore.h>

#include <bdn/win32/util.h>
#include <bdn/ScrollViewLayoutHelper.h>

namespace bdn
{
namespace win32
{

BDN_SAFE_STATIC_IMPL( ScrollViewCore::ScrollViewCoreClass, ScrollViewCore::ScrollViewCoreClass::get );


ScrollViewCore::ScrollViewCore(ScrollView* pOuter)
    : ViewCore( pOuter,
                ScrollViewCore::ScrollViewCoreClass::get().getName(),
				"ScrollView",
				WS_CHILD | WS_BORDER,
				0)
{
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
        
        RECT winClientRect;
        ::GetClientRect(hwnd, &winClientRect);
        Rect clientRectWithoutScrollBars = win32RectToRect(winClientRect, getUiScaleFactor());                               

        // the client rect automatically excludes the space needed for the scroll bars.
        // We want the potential client size without scrollbars, so we enlarge is accordingly.
        if(_showHorizontalScrollBar)
            clientRectWithoutScrollBars.height += horzBarHeight;
        if(_showVerticalScrollBar)
            clientRectWithoutScrollBars.width += vertBarWidth;
        
        ScrollViewLayoutHelper helper(vertBarWidth, horzBarHeight);    

        helper.calcLayout(pOuterView, clientRectWithoutScrollBars.getSize() );

        _showHorizontalScrollBar = helper.getHorizontalScrollBarVisible();
        _showVerticalScrollBar = helper.getVerticalScrollBarVisible();

        Rect contentViewBounds = helper.getContentViewBounds();
        Size scrolledAreaSize = helper.getScrolledAreaSize();
        Size viewPortSize = helper.getViewPortSize();

        // update our style so that the scroll bars are added / removed
        updateScrollInfo(_showHorizontalScrollBar, _showVerticalScrollBar, viewPortSize, scrolledAreaSize);

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


void ScrollViewCore::updateScrollInfo(bool horzScrollBar, bool vertScrollBar, const Size& viewPortSize, const Size& fullContentSize)
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

        if(horzScrollBar)
            newStyle |= WS_HSCROLL;
        if(vertScrollBar)
            newStyle |= WS_VSCROLL;

        if(newStyle!=currStyle)
            ::SetWindowLongPtr( hwnd, GWL_STYLE, newStyle);

        SCROLLINFO scrollInfo;
        memset(&scrollInfo, 0, sizeof(scrollInfo));
        
        scrollInfo.cbSize = sizeof(scrollInfo);
        scrollInfo.fMask = SIF_PAGE | SIF_RANGE;

        scrollInfo.nMin = 0;

        double  uiScaleFactor = getUiScaleFactor();

        if(horzScrollBar)
        {
            scrollInfo.nMax = std::lround(fullContentSize.width*uiScaleFactor);
            scrollInfo.nPage = std::lround(viewPortSize.width*uiScaleFactor);
            ::SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, TRUE);  // TRUE means redraw scrollbar
        }

        if(vertScrollBar)
        {
            scrollInfo.nMax = std::lround(fullContentSize.height*uiScaleFactor);
            scrollInfo.nPage = std::lround(viewPortSize.height*uiScaleFactor);
            ::SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);  // TRUE means redraw scrollbar
        }
    }
}


}
}



