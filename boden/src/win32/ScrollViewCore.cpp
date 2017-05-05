#include <bdn/init.h>
#include <bdn/win32/ScrollViewCore.h>

#include <bdn/win32/util.h>

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
        
void ScrollViewCore::setHorizontalScrollingEnabled(bool enabled)
{
    // we don't care. The setting is only used in layout and there we get it from
    // the outer view directly.
}

void ScrollViewCore::setVerticalScrollingEnabled(bool enabled)
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
        P<View> pContentView = pOuterView->getContentView();
        if(pContentView!=nullptr)
        {
            Size    actualContentSize = pContentView->sizingInfo().get().preferredSize;
            Margin  contentMargin = uiMarginToDipMargin(pContentView->margin());
            Margin  ourPadding = uiMarginToDipMargin(pOuterView->padding());
            Size    contentSizeWithMarginAndPadding = actualContentSize + ourPadding + contentMargin;

            bool    horzScrollEnabled = pOuterView->horizontalScrollingEnabled();
            bool    vertScrollEnabled = pOuterView->verticalScrollingEnabled();

            // these values are unscaled (i.e. for scale factor 1). That means that
            // they are in DIPs.
            int     vertBarWidth = ::GetSystemMetrics(SM_CXVSCROLL);
            int     horzBarHeight = ::GetSystemMetrics(SM_CYHSCROLL);

            double  uiScaleFactor = getUiScaleFactor();

            RECT winClientRect;
            ::GetClientRect(hwnd, &winClientRect);
            Rect clientRect = win32RectToRect(winClientRect, uiScaleFactor);
                                

            // the client rect automatically excludes the space needed for the scroll bars.
            // We want the potential client size without scrollbars, so we enlarge is accordingly.
            if(_showHorizontalScrollBar)
                clientRect.height += horzBarHeight;
            if(_showVerticalScrollBar)
                clientRect.width += vertBarWidth;
            
            // now we have the full client rect without scrollbars
            Size viewPortSize = clientRect.getSize();

            _showHorizontalScrollBar = (contentSizeWithMarginAndPadding.width > viewPortSize.width && horzScrollEnabled);
            if(_showHorizontalScrollBar)
                viewPortSize.height -= horzBarHeight;
            _showVerticalScrollBar = (contentSizeWithMarginAndPadding.height > viewPortSize.height && vertScrollEnabled);                        
            if(_showVerticalScrollBar)
            {
                viewPortSize.width -= vertBarWidth;

                if(!_showHorizontalScrollBar)
                {                
                    // adding the vertical scroll bar might have decreased the viewport width enough so that we now
                    // also need a horizontal scroll bar
                    _showHorizontalScrollBar = (contentSizeWithMarginAndPadding.width > viewPortSize.width && horzScrollEnabled);
                    if(_showHorizontalScrollBar)
                        viewPortSize.height -= horzBarHeight;
                }
            }
            
            // update our style so that the scroll bars are added / removed
            updateScrollInfo(_showHorizontalScrollBar, _showVerticalScrollBar, viewPortSize, contentSizeWithMarginAndPadding);

            // If the content is smaller than the viewport then we enlarge it. If the
            // developer does not want that then he can add an intermediate container
            // and put the "real" content inside that.
            if(contentSizeWithMarginAndPadding.width < viewPortSize.width)
                contentSizeWithMarginAndPadding.width = viewPortSize.width;
            if(contentSizeWithMarginAndPadding.height < viewPortSize.height)
                contentSizeWithMarginAndPadding.height = viewPortSize.height;

            Rect actualContentRect( clientRect.getPosition(), contentSizeWithMarginAndPadding);
            actualContentRect -= ourPadding;
            actualContentRect -= contentMargin;

            pContentView->adjustAndSetBounds(actualContentRect);
        }
    }
}
	
Size ScrollViewCore::calcPreferredSize(double availableWidth=-1, double availableHeight=-1) const
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
        P<View> pContentView = pOuterView->getContentView();

        Size    contentSize;
        Margin  contentMargin;
        if(pContentView!=nullptr)
        {
            contentSize = pContentView->sizingInfo().get().preferredSize;;
            contentMargin = uiMarginToDipMargin(pContentView->margin());
        }

        Margin  ourPadding = uiMarginToDipMargin(pOuterView->padding());

        Size    contentSizeWithMarginAndPadding = contentSize + ourPadding + contentMargin;

        Size    prefSize( contentSizeWithMarginAndPadding );

        bool    horzScrollEnabled = pOuterView->horizontalScrollingEnabled();
        bool    vertScrollEnabled = pOuterView->verticalScrollingEnabled();

        // these values are unscaled (i.e. for scale factor 1). That means that
        // they are in DIPs.
        int     vertBarWidth = ::GetSystemMetrics(SM_CXVSCROLL);
        int     horzBarHeight = ::GetSystemMetrics(SM_CYHSCROLL);

        bool    horzScrollActive = false;
        bool    vertScrollActive = false;

        if(availableWidth!=-1 && prefSize.width>availableWidth && horzScrollEnabled)
        {
            // we need to scroll horizontally. Add the scroll bar height to our preferred height. Set the
            // preferred width to the available width
            prefSize.width = availableWidth;
            prefSize.height += horzBarHeight;
            horzScrollActive = true;
        }
        else if(availableWidth==-1 && horzScrollEnabled)
        {
            // we have "unlimited" width available and horz scrolling is enabled.

            // So in this case the question is what is our "preferred" width?
            // If we set it to the full content width then we might request a huge size that
            // exceeds the actual available space. Then other views might be shrunk together with us
            // below their preferred size to make everything fit. That is not what we want, because
            // we can shrink easily, without compromising our content.



            // might end up enlarging everything
            // to enormous sizes (e.g. creating a huge window, for example).
            // 

            // In this case we set our preferred size to a small value, because we CAN scroll
            // and do not need a lot of 
        }

        if(availableHeight!=-1 && prefSize.height>availableHeight && vertScrollEnabled)
        {
            // we need to scroll vertically.
            prefSize.height = availableHeight;
            prefSize.width += vertBarWidth;
            vertScrollActive = true;
        }

        
        // make sure that we are at least big enough to show the scroll bars if we need
        // them
        if(horzScrollEnabled && prefSize.height<horzBarHeight)
            prefSize.height = horzBarHeight;
        if(vertScrollEnabled && prefSize.width<vertBarWidth)
            prefSize.width = vertBarWidth;


        


        if(availableWidth==-1 || )
            


        }

    if(_)

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

        if(horzScrollBar)
        {
            scrollInfo.nMax = fullContentSize.width;
            scrollInfo.nPage = viewPortSize.width;
            ::SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, TRUE);  // TRUE means redraw scrollbar
        }

        if(vertScrollBar)
        {
            scrollInfo.nMax = fullContentSize.height;
            scrollInfo.nPage = viewPortSize.height;
            ::SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);  // TRUE means redraw scrollbar
        }
    }
}


}
}



