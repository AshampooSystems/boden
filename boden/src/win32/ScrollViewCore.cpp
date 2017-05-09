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
        P<View> pContentView = pOuterView->getContentView();
        if(pContentView!=nullptr)
        {
            Size    actualContentSize = pContentView->sizingInfo().get().preferredSize;
            Margin  contentMargin = uiMarginToDipMargin(pContentView->margin());

            
            Margin  ourPadding;
            {
                Nullable<UiMargin> pad = pOuterView->padding();
                if(!pad.isNull())
                    ourPadding = uiMarginToDipMargin(pad);
            }

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
            
            // now we have the full client rect for the case where no scroll bars are shown
            Size viewPortSize = clientRect.getSize();

            // now we calculate whether or not we need scroll bars

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
                    // also need a horizontal scroll bar. So check that again
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
        P<View> pContentView = pOuterView->getContentView();

        Size    contentSize;
        Margin  contentMargin;
        if(pContentView!=nullptr)
        {
            contentSize = pContentView->sizingInfo().get().preferredSize;;
            contentMargin = uiMarginToDipMargin(pContentView->margin());
        }

        Margin  ourPadding;
        {
            Nullable<UiMargin> pad = pOuterView->padding();
            if(!pad.isNull())
                ourPadding = uiMarginToDipMargin(pad);
        }

        Size    maxSize = pOuterView->preferredSizeMaximum();
        maxSize.applyMaximum(availableSpace);

        bool    widthConstrained = std::isfinite(maxSize.width);
        bool    heightConstrained = std::isfinite(maxSize.height);

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

        if(widthConstrained && prefSize.width>maxSize.width && horzScrollEnabled)
        {
            // we need to scroll horizontally. Add the scroll bar height to our preferred height. Set the
            // preferred width to the available width
            prefSize.width = maxSize.width;
            prefSize.height += horzBarHeight;
            horzScrollActive = true;
        }
        else
        {
            // we either fit into the available space, or horizontal scrolling is disabled.

            // Set the preferred width to the content width
            prefSize.width = contentSizeWithMarginAndPadding.width;
            
            // Note that in the case that we have unlimited space available we also set
            // the preferred size to the full content size, so that scrolling is not
            // necessary (whether is is theoretically enabled or not).
            // That way we ensure that the scroll view does not clip the content
            // unnecessarily when enough space is available.
            
            // Note that scroll views have a high shrink priority, so if the total
            // size of the scroll view and its siblings exceeds the available space then the
            // scroll view will be shrunk first. So it is no problem to initially request a
            // big preferred size.
        }
        
        if(heightConstrained && prefSize.height>maxSize.height && vertScrollEnabled)
        {
            prefSize.height = maxSize.height;
            prefSize.width += vertBarWidth;
            vertScrollActive = true;

            // the vertical scrollbar increases our preferred width.
            // This might cause us to need horizontal scrolling as well, if that is not yet active.

            if(!horzScrollActive && widthConstrained && prefSize.width>maxSize.width && horzScrollEnabled)
            {
                prefSize.width = maxSize.width;
                horzScrollActive = true;
            }
        }

        // apply the minimum constraint
        prefSize.applyMinimum( pOuterView->preferredSizeMinimum() );

        return prefSize;
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



