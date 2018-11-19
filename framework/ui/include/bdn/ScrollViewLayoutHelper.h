#ifndef BDN_ScrollViewLayoutHelper_H_
#define BDN_ScrollViewLayoutHelper_H_

#include <bdn/ScrollView.h>
#include <bdn/Dip.h>

namespace bdn
{

    /** Helper class for laying out scroll views and calculating their preferred
       size. This can be used by IScrollViewCore implementations (although it is
       not mandatory).

        ScrollViewLayoutHelper is a light weight class. It is perfectly ok to
       instantiate it as a temporary object for a single calculation and throw
       it away afterwards
        */
    class ScrollViewLayoutHelper : public Base
    {
      public:
        /** \param vertBarWidth width of the vertical scroll bar in DIPs
            \param horzBarHeight height of the horizontal scroll bar in DIPs.
            */
        ScrollViewLayoutHelper(double vertBarWidth, double horzBarHeight)
        {
            _vertBarWidth = vertBarWidth;
            _horzBarHeight = horzBarHeight;
        }

        /** Calculates the layout for the scroll view. This does not actually
           modify the scroll view or its content view - it merely initializes
           the other member of the ScrollViewLayoutHelper. To finish the layout
           the scrollview implementation has to assign apply the resulting
           values to the scroll view and content view (see
           getHorizontalScrollBarVisible(), getVerticalScrollBarVisible(),
           getContentViewBounds() ).

            This function can handle cases when scrollView or its content view
           are null. In that case the layout result values will be initialized
           to default values.

            \param viewPortSizeWhenNotShowingScrollBars size of the scrollview's
           content view port (the area where the visible part of the content is
           displayed) when no scroll bars are shown.
        */
        void calcLayout(ScrollView *scrollView, Size viewPortSizeWhenNotShowingScrollBars)
        {
            _showHorizontalScrollBar = false;
            _showVerticalScrollBar = false;
            _contentViewBounds = Rect();
            _viewPortSize = viewPortSizeWhenNotShowingScrollBars;

            P<View> contentView;
            if (scrollView != nullptr)
                contentView = scrollView->getContentView();

            bool horzScrollEnabled = false;
            bool vertScrollEnabled = false;
            Margin outerPadding;

            if (scrollView != nullptr) {
                horzScrollEnabled = scrollView->horizontalScrollingEnabled();
                vertScrollEnabled = scrollView->verticalScrollingEnabled();

                Nullable<UiMargin> pad = scrollView->padding();
                if (!pad.isNull())
                    outerPadding = scrollView->uiMarginToDipMargin(pad);
            }

            for (int itNum = 0;; itNum++) {
                Size actualContentSize;
                Margin contentMargin;
                if (contentView != nullptr) {
                    contentMargin = scrollView->uiMarginToDipMargin(contentView->margin());

                    // if the amount of available space for the content view is
                    // limited then we must ask the content view for a dynamic
                    // preferred size. Some view's (like text views) might want
                    // to adapt.

                    actualContentSize = contentView->calcPreferredSize();

                    // In the first iteration (itNum==0) _viewPortSize is the
                    // size with no scrollbars shown. In the second iteration
                    // the viewport size is smaller when scrollbars are shown.
                    // This is what we want, since the space that is available
                    // for the content depends on the effective viewport size
                    // (if scrolling is disabled in that direction)
                    Size contentAvailableSpace = _viewPortSize - outerPadding - contentMargin;

                    bool wideEnough = true;
                    bool highEnough = true;
                    if (horzScrollEnabled)
                        contentAvailableSpace.width = Size::componentNone();
                    else
                        wideEnough = (Dip(actualContentSize.width) <= contentAvailableSpace.width);

                    if (vertScrollEnabled)
                        contentAvailableSpace.height = Size::componentNone();
                    else
                        highEnough = (Dip(actualContentSize.height) <= contentAvailableSpace.height);

                    if (!wideEnough || !highEnough)
                        actualContentSize = contentView->calcPreferredSize(contentAvailableSpace);

                    Rect bounds(Point(), actualContentSize);
                    Rect adjustedBounds = contentView->adjustBounds(bounds, RoundType::nearest, RoundType::up);

                    actualContentSize = adjustedBounds.getSize();
                }

                // reset the scrollbar booleans and viewport size.
                // Usually these will end up the same as in the first iteration,
                // but we nevertheless calculate them again.
                _showHorizontalScrollBar = false;
                _showVerticalScrollBar = false;
                _viewPortSize = viewPortSizeWhenNotShowingScrollBars;

                Size contentSizeWithMarginAndPadding = actualContentSize + contentMargin + outerPadding;

                // now we calculate whether or not we need scroll bars
                _showHorizontalScrollBar =
                    (Dip(contentSizeWithMarginAndPadding.width) > _viewPortSize.width && horzScrollEnabled);
                if (_showHorizontalScrollBar)
                    _viewPortSize.height -= _horzBarHeight;
                _showVerticalScrollBar =
                    (Dip(contentSizeWithMarginAndPadding.height) > _viewPortSize.height && vertScrollEnabled);
                if (_showVerticalScrollBar) {
                    _viewPortSize.width -= _vertBarWidth;

                    if (!_showHorizontalScrollBar) {
                        // adding the vertical scroll bar might have decreased
                        // the viewport width enough so that we now also need a
                        // horizontal scroll bar. So check that again
                        _showHorizontalScrollBar =
                            (Dip(contentSizeWithMarginAndPadding.width) > _viewPortSize.width && horzScrollEnabled);
                        if (_showHorizontalScrollBar)
                            _viewPortSize.height -= _horzBarHeight;
                    }
                }

                // If the content is smaller than the viewport then we enlarge
                // it. If the developer does not want that then he can add an
                // intermediate container and put the "real" content inside
                // that.
                if (contentSizeWithMarginAndPadding.width < _viewPortSize.width)
                    contentSizeWithMarginAndPadding.width = _viewPortSize.width;
                if (contentSizeWithMarginAndPadding.height < _viewPortSize.height)
                    contentSizeWithMarginAndPadding.height = _viewPortSize.height;

                // if the content is bigger than the viewport and we have
                // scrolling disabled then we clip the content.
                bool widthClipped = false;
                bool heightClipped = false;
                if (!horzScrollEnabled && Dip(contentSizeWithMarginAndPadding.width) > _viewPortSize.width) {
                    widthClipped = true;
                    contentSizeWithMarginAndPadding.width = _viewPortSize.width;
                }
                if (!vertScrollEnabled && Dip(contentSizeWithMarginAndPadding.height) > _viewPortSize.height) {
                    heightClipped = true;
                    contentSizeWithMarginAndPadding.height = _viewPortSize.height;
                }

                _contentViewBounds = Rect(Point(0, 0), contentSizeWithMarginAndPadding);
                _contentViewBounds -= outerPadding;
                _contentViewBounds -= contentMargin;

                _scrolledAreaSize = contentSizeWithMarginAndPadding;

                if (itNum == 0 && contentView != nullptr &&
                    ((widthClipped && _showVerticalScrollBar) || (heightClipped && _showHorizontalScrollBar))) {
                    // we have shown a scrollbar on a side where the content was
                    // clipped. That means that the viewport size has changed in
                    // a way that is relevant for the content view. We need to
                    // do another iteration and give the content view a change
                    // to react to the new viewport size..
                    continue;
                }

                // no further iteration necessary
                break;
            }
        }

        /** Returns true if the horizontal scroll bar should be shown.*/
        bool getHorizontalScrollBarVisible() const { return _showHorizontalScrollBar; }

        /** Returns true if the vertical scroll bar should be shown.*/
        bool getVerticalScrollBarVisible() const { return _showVerticalScrollBar; }

        /** Returns the rect that the content view should have inside the
           scrollable inner coordinate space. It is assumed that the origin of
           that scrollable space is (0,0) - if that is not correct for the
            particular scroll view implementation then the returned Rectangle
           must be adapted accordingly by the caller.
            */
        Rect getContentViewBounds() const { return _contentViewBounds; }

        /** Returns the size of the scrolled area inside the scroll view. This
           is initialized by calcLayout().

            The scrolled area is the area occupied by the content view, plus the
           content view margins and the scroll view padding.
            */
        Size getScrolledAreaSize() const { return _scrolledAreaSize; }

        /** Returns the size of the view port that shows the scrolled content.
           I.e. the amount of content that can at most be visible at the same
           time. Scroll bars are not part of the view port.*/
        Size getViewPortSize() const { return _viewPortSize; }

        /** Calculates the preferred size of the scroll view (see
           View::calcPreferredSize()).

            This function can handle cases when scrollView or its content view
           are null. If scrollView is null then a zero size is returned. If its
           content view is null then the preferred size for an empty scroll view
           is returned.
            */
        Size calcPreferredSize(ScrollView *scrollView, const Size &availableSpace = Size::none()) const
        {
            P<View> contentView;
            if (scrollView != nullptr)
                contentView = scrollView->getContentView();

            Margin outerPadding;
            Size maxSize(Size::none());
            if (scrollView != nullptr) {
                Nullable<UiMargin> pad = scrollView->padding();
                if (!pad.isNull())
                    outerPadding = scrollView->uiMarginToDipMargin(pad);

                maxSize = scrollView->preferredSizeMaximum();
            }

            maxSize.applyMaximum(availableSpace);

            bool widthConstrained = std::isfinite(maxSize.width);
            bool heightConstrained = std::isfinite(maxSize.height);

            bool horzScrollEnabled = false;
            bool vertScrollEnabled = false;

            if (scrollView != nullptr) {
                horzScrollEnabled = scrollView->horizontalScrollingEnabled();
                vertScrollEnabled = scrollView->verticalScrollingEnabled();
            }

            Margin contentMargin;
            if (contentView != nullptr)
                contentMargin = scrollView->uiMarginToDipMargin(contentView->margin());

            Size prefSize;
            Size contentSize;

            bool showHorizontalScrollBar = false;
            bool showVerticalScrollBar = false;

            for (int itNum = 0;; itNum++) {
                if (contentView != nullptr) {
                    // the available space we communicate to the content view is
                    // based on maxSize, which is the combination of
                    // availableSpace and the scroll view max preferred size.
                    // Note that we leave the content available space at
                    // unlimited in directions in which scrolling is enabled.

                    Size contentAvailableSpace = Size::none();

                    if (!horzScrollEnabled && widthConstrained)
                        contentAvailableSpace.width = maxSize.width - (outerPadding.left + contentMargin.left +
                                                                       contentMargin.right + outerPadding.right);

                    if (!vertScrollEnabled && heightConstrained)
                        contentAvailableSpace.height = maxSize.height - (outerPadding.top + contentMargin.top +
                                                                         contentMargin.bottom + outerPadding.bottom);

                    if (std::isfinite(contentAvailableSpace.height)) {
                        if (showHorizontalScrollBar)
                            contentAvailableSpace.height -= _horzBarHeight;

                        if (contentAvailableSpace.height < 0)
                            contentAvailableSpace.height = 0;
                    }

                    if (std::isfinite(contentAvailableSpace.width)) {
                        if (showVerticalScrollBar)
                            contentAvailableSpace.width -= _vertBarWidth;

                        if (contentAvailableSpace.width < 0)
                            contentAvailableSpace.width = 0;
                    }

                    contentSize = contentView->calcPreferredSize(contentAvailableSpace);
                } else
                    contentSize = Size(0, 0);

                prefSize = contentSize + contentMargin + outerPadding;

                // reset the scrollbar booleans. They might be set from the
                // previous iteration but we want to start with no scrollbars.
                showHorizontalScrollBar = false;
                showVerticalScrollBar = false;

                // we passed the available space to the content view. If it
                // exceeds it and we cannot scroll then that is because its
                // content cannot fit into that size. In that case we should
                // also report the bigger size and not clip it. So we only do
                // clipping if horz scrolling is enabled.
                if (widthConstrained && Dip(prefSize.width) > maxSize.width && horzScrollEnabled) {
                    prefSize.width = maxSize.width;

                    // we need to scroll horizontally. Add the scroll bar height
                    // to our preferred height. Set the preferred width to the
                    // available width

                    prefSize.height += _horzBarHeight;
                    showHorizontalScrollBar = true;
                }

                // if width is not constrained then we request the full
                // preferred size of the content view so that scrolling is
                // notnecessary (whether is is theoretically enabled or not).
                // That way we ensure that the scroll view does not clip the
                // content unnecessarily when enough space is available.

                // Note that scroll views have a high shrink priority, so if the
                // total size of the scroll view and its siblings exceeds the
                // available space then the scroll view will be shrunk first. So
                // it is no problem to initially request a big preferred size.

                if (heightConstrained && Dip(prefSize.height) > maxSize.height && vertScrollEnabled) {
                    prefSize.height = maxSize.height;

                    prefSize.width += _vertBarWidth;
                    showVerticalScrollBar = true;

                    // the vertical scrollbar increases our preferred width.
                    // This might cause us to need horizontal scrolling as well,
                    // if that is not yet active. If we are already scrolling
                    // horizontally then we need to reduce the prefSize.width to
                    // maxSize.width again.

                    if (widthConstrained && Dip(prefSize.width) > maxSize.width && horzScrollEnabled) {
                        prefSize.width = maxSize.width;

                        showHorizontalScrollBar = true;
                    }
                }

                if (scrollView != nullptr) {
                    // apply the minimum constraint
                    prefSize.applyMinimum(scrollView->preferredSizeMinimum());

                    // also apply the preferredSizeMaximum. We already applied
                    // it at the start to take the constraint into account from
                    // the beginning, but it may be that prefSize is bigger than
                    // the max here because the content window does not fit. So
                    // we clip the result against the max here, because we never
                    // want it to be exceeded. Note that we do NOT clip against
                    // availableSpace, because we WANT that to be exceeded if
                    // the children do not fit.
                    prefSize.applyMaximum(scrollView->preferredSizeMaximum());
                }

                Size contentSizeAtPrefSizeWithoutScrolling = prefSize - outerPadding - contentMargin;

                bool widthClipped =
                    !showHorizontalScrollBar && Dip(contentSizeAtPrefSizeWithoutScrolling.width) < contentSize.width;
                bool heightClipped =
                    !showVerticalScrollBar && Dip(contentSizeAtPrefSizeWithoutScrolling.height) < contentSize.height;

                // if we have limited space then we already communicated that to
                // the content view when we retrieved its preferred size. So if
                // the content view got clipped then this can have two reasons:
                // 1) A scrollbar was shown and thus reduced the available
                // space. 2) The content view reported a preferred size that was
                // bigger than the available space
                //    we reported to it. I.e. its content cannot be reduced to
                //    that size.

                // In the case of 2 we cannot do much. In the case of 1 we
                // should give the content view a chance to adapt its preferred
                // size to the reduced available space

                if (itNum == 0 && contentView != nullptr &&
                    ((widthClipped && showVerticalScrollBar) || (heightClipped && showHorizontalScrollBar))) {
                    // the available space or min/max limits have constrained
                    // the preferred size of the scrollview. The result of this
                    // is that the content was clipped to a smaller size than
                    // its preferred size.

                    // We now need to give the content view a chance to adapt to
                    // the available space. For example, if a text view's width
                    // is clipped then it might want to enlarge its height to
                    // display all text. And there might be enough available
                    // space for that.

                    // We do that by simply doing another iteration.

                    // Now, how often do we do this? What if the second
                    // iteration causes further clipping - do we do a third
                    // iteration then?

                    // It is important to keep in mind WHY we want to do another
                    // iteration. It is not actually because of the clipping -
                    // that might be unavoidable. The real reason is that the
                    // amount of available space for the content view has
                    // changed due to a scroll bar being shown.

                    // And this can only happen once, in the first iteration.
                    // Consider this:

                    // If scrolling is enabled in both directions then no
                    // clipping will ever happen. The scrollview just scrolls to
                    // show the content view at its preferred size.

                    // If scrolling is disabled in both directions then clipping
                    // may happen, but the amount of available space for the
                    // content view does not change, since no scrollbars are
                    // shown.

                    // So the only case when we need a second iteration is when
                    // scrolling is only enabled in one direction and the scroll
                    // bar is shown. In the next iteration even less space is
                    // available, but no additional scroll bar can be displayed,
                    // so there is never a case when the available space for the
                    // content view is further reduced. So there is no need for
                    // a third iteration.

                    // So, just do the whole calculation again. Note that the
                    // state of the show..ScrollBar booleans will carry over to
                    // the next iteration and the available space that is
                    // communicated to the content view is reduced accordingly.

                    continue;
                }

                // no further iteration necessary
                break;
            }

            return prefSize;
        }

      private:
        double _vertBarWidth = 0;
        double _horzBarHeight = 0;

        bool _showHorizontalScrollBar = false;
        bool _showVerticalScrollBar = false;
        Rect _contentViewBounds;
        Size _scrolledAreaSize;
        Size _viewPortSize;
    };
}

#endif
