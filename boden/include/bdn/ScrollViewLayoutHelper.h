#ifndef BDN_ScrollViewLayoutHelper_H_
#define BDN_ScrollViewLayoutHelper_H_

#include <bdn/ScrollView.h>

namespace bdn
{

    
/** Helper class for laying out scroll views and calculating their preferred size.
    This can be used by IScrollViewCore implementations (although it is not mandatory).

    ScrollViewLayoutHelper is a light weight class. It is perfectly ok to instantiate it
    as a temporary object for a single calculation and throw it away afterwards
    */
class ScrollViewLayoutHelper : public Base
{
public:
    /** \param vertBarWidth width of the vertical scroll bar in DIPs
        \param horzBarHeight height of the horizontal scroll bar in DIPs.
        */
    ScrollViewLayoutHelper(double horzBarHeight, double vertBarWidth)
    {
        _horzBarHeight = horzBarHeight;
        _vertBarWidth = vertBarWidth;
    }


    /** Calculates the layout for the scroll view. This does not actually modify
        the scroll view or its content view - it merely initializes the other member
        of the ScrollViewLayoutHelper. To finish the layout the scrollview implementation
        has to assign apply the resulting values to the scroll view and content view
        (see getHorizontalScrollBarVisible(), getVerticalScrollBarVisible(), getContentViewBounds() ).

        This function can handle cases when pScrollView or its content view are null. In that case
        the layout result values will be initialized to default values.
    
        \param viewPortSizeWhenNotShowingScrollBars size of the scrollview's content view port 
            (the area where the visible part of the content is displayed) when
            no scroll bars are shown.
    */
    void calcLayout(ScrollView* pScrollView, Size viewPortSizeWhenNotShowingScrollBars)
    {
        _showHorizontalScrollBar = false;
        _showVerticalScrollBar = false;
        _contentViewBounds = Rect();
        _viewPortSize = viewPortSizeWhenNotShowingScrollBars;

        P<View> pContentView;
        if(pScrollView!=nullptr)
            pContentView = pScrollView->getContentView();
            
        Size    actualContentSize;
        Margin  contentMargin;
        if(pContentView!=nullptr)
        {
            actualContentSize = pContentView->sizingInfo().get().preferredSize;                
            contentMargin = pScrollView->uiMarginToDipMargin(pContentView->margin());
        }
                        
        Margin  outerPadding;
        if(pScrollView!=nullptr)
        {
            Nullable<UiMargin> pad = pScrollView->padding();
            if(!pad.isNull())
                outerPadding = pScrollView->uiMarginToDipMargin(pad);
        }

        Size    contentSizeWithMarginAndPadding = actualContentSize + contentMargin + outerPadding;

        bool    horzScrollEnabled = false;
        bool    vertScrollEnabled = false;

        if(pScrollView!=nullptr)
        {
            horzScrollEnabled = pScrollView->horizontalScrollingEnabled();
            vertScrollEnabled = pScrollView->verticalScrollingEnabled();
        }                                
                                           
        // now we calculate whether or not we need scroll bars
        _showHorizontalScrollBar = (contentSizeWithMarginAndPadding.width > _viewPortSize.width && horzScrollEnabled);
        if(_showHorizontalScrollBar)
            _viewPortSize.height -= _horzBarHeight;
        _showVerticalScrollBar = (contentSizeWithMarginAndPadding.height > _viewPortSize.height && vertScrollEnabled);                        
        if(_showVerticalScrollBar)
        {
            _viewPortSize.width -= _vertBarWidth;

            if(!_showHorizontalScrollBar)
            {                
                // adding the vertical scroll bar might have decreased the viewport width enough so that we now
                // also need a horizontal scroll bar. So check that again
                _showHorizontalScrollBar = (contentSizeWithMarginAndPadding.width > _viewPortSize.width && horzScrollEnabled);
                if(_showHorizontalScrollBar)
                    _viewPortSize.height -= _horzBarHeight;
            }
        }
            
        // If the content is smaller than the viewport then we enlarge it. If the
        // developer does not want that then he can add an intermediate container
        // and put the "real" content inside that.
        if(contentSizeWithMarginAndPadding.width < _viewPortSize.width)
            contentSizeWithMarginAndPadding.width = _viewPortSize.width;
        if(contentSizeWithMarginAndPadding.height < _viewPortSize.height)
            contentSizeWithMarginAndPadding.height = _viewPortSize.height;

        _contentViewBounds = Rect( Point(0,0), contentSizeWithMarginAndPadding);
        _contentViewBounds -= outerPadding;
        _contentViewBounds -= contentMargin;
                
        _scrolledAreaSize = contentSizeWithMarginAndPadding;
    }


    /** Returns true if the horizontal scroll bar should be shown.*/
    bool getHorizontalScrollBarVisible() const
    {
        return _showHorizontalScrollBar;
    }


    /** Returns true if the vertical scroll bar should be shown.*/
    bool getVerticalScrollBarVisible() const
    {
        return _showVerticalScrollBar;
    }

    /** Returns the rect that the content view should have inside the scrollable inner coordinate space.
        It is assumed that the origin of that scrollable space is (0,0) - if that is not correct for the
        particular scroll view implementation then the returned Rectangle must be adapted accordingly by the
        caller.
        */
    Rect getContentViewBounds() const
    {
        return _contentViewBounds;
    }


    /** Returns the size of the scrolled area inside the scroll view. This is initialized by calcLayout().

        The scrolled area is the area occupied by the content view, plus the content view
        margins and the scroll view padding.
        */
    Size getScrolledAreaSize() const
    {
        return _scrolledAreaSize;
    }


    /** Returns the size of the view port that shows the scrolled content. I.e. the amount of content
        that can at most be visible at the same time. Scroll bars are not part of the view port.*/
    Size getViewPortSize() const
    {
        return _viewPortSize;
    }



    /** Calculates the preferred size of the scroll view (see View::calcPreferredSize()).

        This function can handle cases when pScrollView or its content view are null.
        If pScrollView is null then a zero size is returned. If its content view is null
        then the preferred size for an empty scroll view is returned.
        */
    Size calcPreferredSize( ScrollView* pScrollView, const Size& availableSpace ) const
    {
        P<View> pContentView;
        if(pScrollView!=nullptr)
            pContentView = pScrollView->getContentView();

        Size    contentSize;
        Margin  contentMargin;
        if(pContentView!=nullptr)
        {
            contentSize = pContentView->sizingInfo().get().preferredSize;;
            contentMargin = pScrollView->uiMarginToDipMargin(pContentView->margin());
        }

        Margin  outerPadding;
        Size    maxSize(Size::none());
        if(pScrollView!=nullptr)
        {
            Nullable<UiMargin> pad = pScrollView->padding();
            if(!pad.isNull())
                outerPadding = pScrollView->uiMarginToDipMargin(pad);

            maxSize = pScrollView->preferredSizeMaximum();
        }
        
        maxSize.applyMaximum(availableSpace);

        bool    widthConstrained = std::isfinite(maxSize.width);
        bool    heightConstrained = std::isfinite(maxSize.height);

        Size    contentSizeWithMarginAndPadding = contentSize + outerPadding + contentMargin;

        Size    prefSize( contentSizeWithMarginAndPadding );

        bool    horzScrollEnabled = false;
        bool    vertScrollEnabled = false;

        if(pScrollView!=nullptr)
        {
            horzScrollEnabled = pScrollView->horizontalScrollingEnabled();
            vertScrollEnabled = pScrollView->verticalScrollingEnabled();
        }


        bool    showHorizontalScrollBar = false;
        bool    showVerticalScrollBar = false;

        if(widthConstrained && prefSize.width>maxSize.width && horzScrollEnabled)
        {
            // we need to scroll horizontally. Add the scroll bar height to our preferred height. Set the
            // preferred width to the available width
            prefSize.width = maxSize.width;
            prefSize.height += _horzBarHeight;
            showHorizontalScrollBar = true;
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
            prefSize.width += _vertBarWidth;
            showVerticalScrollBar = true;

            // the vertical scrollbar increases our preferred width.
            // This might cause us to need horizontal scrolling as well, if that is not yet active.
            // If we are already scrolling horizontally then we need to reduce the prefSize.width
            // to maxSize.width again.

            if(widthConstrained && prefSize.width>maxSize.width && horzScrollEnabled)
            {
                prefSize.width = maxSize.width;
                showHorizontalScrollBar = true;
            }
        }

        if(pScrollView!=nullptr)
        {
            // apply the minimum constraint
            prefSize.applyMinimum( pScrollView->preferredSizeMinimum() );

            // also apply the preferredSizeMaximum. We already applied it at the start to
            // take the constraint into account from the beginning, but it may be that prefSize
            // is bigger than the max here because the content window does not fit.
            // So we clip the result against the max here, because we never want it to be exceeded.
            // Note that we do NOT clip against availableSpace, because we WANT that to be exceeded
            // if the children do not fit.
            prefSize.applyMaximum( pScrollView->preferredSizeMaximum() );
        }

        return prefSize;
    }

private:
    double _horzBarHeight = 0;
    double _vertBarWidth = 0;

    bool _showHorizontalScrollBar = false;
    bool _showVerticalScrollBar = false;
    Rect _contentViewBounds;
    Size _scrolledAreaSize;
    Size _viewPortSize;
};
    
    
}



#endif

