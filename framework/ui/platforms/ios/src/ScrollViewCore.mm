
#import <bdn/ios/ScrollViewCore.hh>

@interface BdnIosScrollViewDelegate_ : UIResponder <UIScrollViewDelegate>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ScrollViewCore> core;
@end

@implementation BdnIosScrollViewDelegate_

- (void)scrollViewDidScroll:(UIScrollView *)uiScrollView
{
    if (auto core = self.core.lock()) {
        core->updateVisibleClientRect();
    }
}
@end

@interface BodenUIScrollView : UIScrollView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

@implementation BodenUIScrollView

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}

@end

namespace bdn::detail
{
    CORE_REGISTER(ScrollView, bdn::ios::ScrollViewCore, ScrollView)
}

namespace bdn::ios
{
    BodenUIScrollView *_createScrollView() { return [[BodenUIScrollView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)]; }

    ScrollViewCore::ScrollViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, _createScrollView())
    {}

    void ScrollViewCore::init()
    {
        ViewCore::init();

        _uiScrollView = (UIScrollView *)uiView();

        _delegate = [[BdnIosScrollViewDelegate_ alloc] init];
        _delegate.core = shared_from_this<ScrollViewCore>();
        _uiScrollView.delegate = _delegate;

        horizontalScrollingEnabled.onChange() +=
            [=](auto va) { _uiScrollView.scrollEnabled = horizontalScrollingEnabled || verticalScrollingEnabled; };

        verticalScrollingEnabled.onChange() +=
            [=](auto va) { _uiScrollView.scrollEnabled = horizontalScrollingEnabled || verticalScrollingEnabled; };

        contentView.onChange() += [=](auto va) { updateContent(va->get()); };
    }

    void ScrollViewCore::updateContent(const std::shared_ptr<View> &content)
    {
        [[_uiScrollView subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];

        _childGeometry.reset();

        if (content != nullptr) {
            if (auto childCore = content->core<ViewCore>()) {
                _childGeometry = std::make_shared<Property<Rect>>();

                _childGeometry->bind(childCore->geometry);

                _childGeometry->onChange() += [=](auto va) {
                    CGSize s;
                    s.width = va->get().width;
                    s.height = va->get().height;
                    _uiScrollView.contentSize = s;
                };

                [_uiScrollView addSubview:childCore->uiView()];
            } else {
                throw std::runtime_error("Cannot add this type of View");
            }
        }

        markDirty();
    }

    void ScrollViewCore::scrollClientRectToVisible(const Rect &targetRect)
    {
        // handle infinities
        Size clientSize = iosSizeToSize(_uiScrollView.contentSize);
        Size viewPortSize = iosSizeToSize(_uiScrollView.bounds.size);

        // scrollRectToVisible will ignore calls with rects that are outside
        // the valid client area. So we need to clip it.

        double left = targetRect.x;
        double right = targetRect.x + targetRect.width;
        double top = targetRect.y;
        double bottom = targetRect.y + targetRect.height;

        // first, clip the target rect to the client area.
        // This also automatically gets rid of infinity target positions
        // (which are allowed)
        if (left > clientSize.width) {
            left = clientSize.width;
        }
        if (right > clientSize.width) {
            right = clientSize.width;
        }
        if (top > clientSize.height) {
            top = clientSize.height;
        }
        if (bottom > clientSize.height) {
            bottom = clientSize.height;
        }

        if (left < 0) {
            left = 0;
        }
        if (right < 0) {
            right = 0;
        }
        if (top < 0) {
            top = 0;
        }
        if (bottom < 0) {
            bottom = 0;
        }

        if (right < left) {
            right = left;
        }
        if (bottom < top) {
            bottom = top;
        }

        Point scrollPos = iosPointToPoint(_uiScrollView.contentOffset);

        // there is a special case if the target rect is bigger than the
        // viewport. In that case the desired end position is ambiguous: any
        // sub-rect of viewport size inside the specified target rect would
        // be "as good as possible". The documentation for
        // scrollClientRectToVisible resolves this ambiguity by requiring
        // that we scroll the minimal amount. So we want the new visible
        // rect to be as close to the old one as possible.

        // The ios scrollRectToVisible function does not behave like that.
        // It seems to give the bottom and right edges of the specified
        // target rect priority. So we have to manually fix this by
        // selecting the part of the big target rect that we actually want
        // to scroll to.
        double currVisibleLeft = scrollPos.x;
        double currVisibleRight = currVisibleLeft + viewPortSize.width;
        double currVisibleTop = scrollPos.y;
        double currVisibleBottom = currVisibleTop + viewPortSize.height;

        if (right - left > viewPortSize.width) {
            // the width of the target rect is bigger than the viewport
            // width. So not all of the target rect can be made visible.
            // UIScrollView will prioritize the right edge of the target
            // rect. We want the closest edge to the current visible rect to
            // be prioritized.

            if (currVisibleLeft >= left && currVisibleRight <= right) {
                // The current visible rect is already fully inside the
                // target rect. In this case we do not want to move the
                // scroll position at all. So set the target rect to the
                // current view port rect
                left = currVisibleLeft;
                right = currVisibleRight;
            } else {
                // shrink the target rect so that it matches the viewport
                // width. We want to shrink towards the edge that is closest
                // to the current visible rect. Note that the width of the
                // visible rect is smaller than the target width and that
                // the visible rect is not fully inside the target rect. So
                // one of the target rect edges has to be closer than the
                // other.

                double distanceLeft = fabs(left - currVisibleLeft);
                double distanceRight = fabs(right - currVisibleRight);

                if (distanceLeft < distanceRight) {
                    // the left edge of the target rect is closer to the
                    // current visible rect than the right edge. So we want
                    // to move towards the left.
                    right = left + viewPortSize.width;
                } else {
                    // move towards the right edge
                    left = right - viewPortSize.width;
                }
            }
        }

        if (bottom - top > viewPortSize.height) {
            if (currVisibleTop >= top && currVisibleBottom <= bottom) {
                top = currVisibleTop;
                bottom = currVisibleBottom;
            } else {
                double distanceTop = fabs(top - currVisibleTop);
                double distanceBottom = fabs(bottom - currVisibleBottom);

                if (distanceTop < distanceBottom) {
                    bottom = top + viewPortSize.height;
                } else {
                    top = bottom - viewPortSize.height;
                }
            }
        }

        if (left < 0) {
            left = 0;
        }
        if (right < 0) {
            right = 0;
        }
        if (top < 0) {
            top = 0;
        }
        if (bottom < 0) {
            bottom = 0;
        }

        // also, the rect must not be zero size. So we must move either the
        // left or right boundaries by one pixel (or top/bottom). We have a
        // small problem here. If the current scroll position is before the
        // target position then we actually do not want to move the
        // right/bottom boundary further away from it. Otherwise we will
        // scroll one pixel too far. So we need to look at the current
        // scroll position to decide which one we want to move. We always
        // want to enlarge the target rect TOWARDS the current scroll
        // position.

        if (right == left) {
            if (scrollPos.x < left) {
                if (right < 1) {
                    right = 1;
                }
                left = right - 1;
            } else {
                if (left + 1 > clientSize.width) {
                    left = clientSize.width - 1;
                }

                right = left + 1;
            }
        }

        if (bottom == top) {
            if (scrollPos.y < top) {
                if (bottom < 1) {
                    bottom = 1;
                }
                top = bottom - 1;
            } else {
                if (top + 1 > clientSize.height) {
                    top = clientSize.height - 1;
                }

                bottom = top + 1;
            }
        }

        Rect adaptedTargetRect(left, top, right - left, bottom - top);

        CGRect iosRect = rectToIosRect(adaptedTargetRect);

        [_uiScrollView scrollRectToVisible:iosRect animated:YES];
    }

    void ScrollViewCore::updateVisibleClientRect()
    {
        Point scrollPosition = iosPointToPoint(_uiScrollView.contentOffset);

        // Not correct if scroll view is zoomed. Zooming is not supported yet.
        Size visibleSize = iosSizeToSize(_uiScrollView.bounds.size);
        this->visibleClientRect = Rect{scrollPosition, visibleSize};
    }
}
