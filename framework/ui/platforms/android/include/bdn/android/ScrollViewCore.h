#pragma once

#include <bdn/IScrollViewCore.h>
#include <bdn/ScrollView.h>
#include <bdn/android/IParentViewCore.h>
#include <bdn/android/JNativeScrollView.h>
#include <bdn/android/JViewGroup.h>
#include <bdn/android/ViewCore.h>
#include <bdn/java/NativeWeakPointer.h>

namespace bdn
{
    namespace android
    {

        class ScrollViewCore : public ViewCore, virtual public IScrollViewCore, virtual public IParentViewCore
        {
          public:
            ScrollViewCore(std::shared_ptr<ScrollView> outer)
                : ScrollViewCore(outer, createAndroidViewClass<JNativeScrollView>(outer).cast<JNativeScrollView>())
            {}

          private:
            ScrollViewCore(std::shared_ptr<ScrollView> outer, JNativeScrollView nativeScrollView)
                : ViewCore(outer, nativeScrollView.getWrapperView()), _jNativeScrollView(nativeScrollView),
                  _jContentParent(_jNativeScrollView.getContentParent())
            {
                // inside the scroll view we have a NativeViewGroup object as
                // the glue between our layout system and that of android. That
                // allows us to position the content view manually. It also
                // ensures that the parent of the content view is a
                // NativeViewGroup, which is important because we assume that
                // that is the case in some places.

                setVerticalScrollingEnabled(outer->verticalScrollingEnabled);
                setHorizontalScrollingEnabled(outer->horizontalScrollingEnabled);
            }

          public:
            void setHorizontalScrollingEnabled(const bool &enabled) override
            {
                // nothing to do - we get this directly from the outer window
            }

            void setVerticalScrollingEnabled(const bool &enabled) override
            {
                // nothing to do - we get this directly from the outer window
            }

            void scrollClientRectToVisible(const Rect &clientRect) override
            {
                int visibleLeft = _jNativeScrollView.getScrollX();
                int visibleTop = _jNativeScrollView.getScrollY();
                int visibleWidth = _jNativeScrollView.getWidth();
                int visibleHeight = _jNativeScrollView.getHeight();
                int visibleRight = visibleLeft + visibleWidth;
                int visibleBottom = visibleTop + visibleHeight;

                int clientWidth = _jContentParent.getWidth();
                int clientHeight = _jContentParent.getHeight();

                double uiScaleFactor = getUIScaleFactor();

                int targetLeft;
                int targetRight;
                if (std::isfinite(clientRect.x)) {
                    targetLeft = std::lround(clientRect.x * uiScaleFactor);
                    targetRight = targetLeft + std::lround(clientRect.width * uiScaleFactor);
                } else {
                    if (clientRect.x > 0)
                        targetLeft = clientWidth;
                    else
                        targetLeft = 0;

                    targetRight = targetLeft;
                }

                int targetTop;
                int targetBottom;
                if (std::isfinite(clientRect.y)) {
                    targetTop = std::lround(clientRect.y * uiScaleFactor);
                    targetBottom = targetTop + std::lround(clientRect.height * uiScaleFactor);
                } else {
                    if (clientRect.y > 0)
                        targetTop = clientHeight;
                    else
                        targetTop = 0;

                    targetBottom = targetTop;
                }

                // first, clip the target rect to the client area.
                // This also automatically gets rid of infinity target positions
                // (which are allowed)
                if (targetLeft > clientWidth)
                    targetLeft = clientWidth;
                if (targetRight > clientWidth)
                    targetRight = clientWidth;
                if (targetTop > clientHeight)
                    targetTop = clientHeight;
                if (targetBottom > clientHeight)
                    targetBottom = clientHeight;

                if (targetLeft < 0)
                    targetLeft = 0;
                if (targetRight < 0)
                    targetRight = 0;
                if (targetTop < 0)
                    targetTop = 0;
                if (targetBottom < 0)
                    targetBottom = 0;

                // there is a special case if the target rect is bigger than the
                // viewport. In that case the desired end position is ambiguous:
                // any sub-rect of viewport size inside the specified target
                // rect would be "as good as possible". The documentation for
                // scrollClientRectToVisible resolves this ambiguity by
                // requiring that we scroll the minimal amount. So we want the
                // new visible rect to be as close to the old one as possible.

                // Since we specify the scroll position directly, we need to
                // handle this case on our side.

                if (targetRight - targetLeft > visibleWidth) {
                    if (visibleLeft >= targetLeft && visibleRight <= targetRight) {
                        // The current visible rect is already fully inside the
                        // target rect. In this case we do not want to move the
                        // scroll position at all. So set the target rect to the
                        // current view port rect
                        targetLeft = visibleLeft;
                        targetRight = visibleRight;
                    } else {
                        // shrink the target rect so that it matches the
                        // viewport width. We want to shrink towards the edge
                        // that is closest to the current visible rect. Note
                        // that the width of the visible rect is smaller than
                        // the target width and that the visible rect is not
                        // fully inside the target rect. So one of the target
                        // rect edges has to be closer than the other.

                        int distanceLeft = std::abs(targetLeft - visibleLeft);
                        int distanceRight = std::abs(targetRight - visibleRight);

                        if (distanceLeft < distanceRight) {
                            // the left edge of the target rect is closer to the
                            // current visible rect than the right edge. So we
                            // want to move towards the left.
                            targetRight = targetLeft + visibleWidth;
                        } else {
                            // move towards the right edge
                            targetLeft = targetRight - visibleWidth;
                        }
                    }
                }

                if (targetBottom - targetTop > visibleHeight) {
                    if (visibleTop >= targetTop && visibleBottom <= targetBottom) {
                        targetTop = visibleTop;
                        targetBottom = visibleBottom;
                    } else {
                        int distanceTop = std::abs(targetTop - visibleTop);
                        int distanceBottom = std::abs(targetBottom - visibleBottom);

                        if (distanceTop < distanceBottom)
                            targetBottom = targetTop + visibleHeight;
                        else
                            targetTop = targetBottom - visibleHeight;
                    }
                }

                if (targetLeft < 0)
                    targetLeft = 0;
                if (targetRight < 0)
                    targetRight = 0;
                if (targetTop < 0)
                    targetTop = 0;
                if (targetBottom < 0)
                    targetBottom = 0;

                int scrollX = visibleLeft;
                int scrollY = visibleTop;

                if (targetRight > visibleRight)
                    scrollX = targetRight - visibleWidth;
                if (targetLeft < visibleLeft)
                    scrollX = targetLeft;

                if (targetBottom > visibleBottom)
                    scrollY = targetBottom - visibleHeight;
                if (targetTop < visibleTop)
                    scrollY = targetTop;

                _jNativeScrollView.smoothScrollTo(scrollX, scrollY);
            }

            double getUIScaleFactor() const override { return ViewCore::getUIScaleFactor(); }

            void addChildJView(JView childJView) override
            {
                _childGeometry.reset();
                _jContentParent.removeAllViews();

                if (auto viewCore = bdn::android::getViewCoreFromJavaViewRef(childJView.getRef_())) {
                    _jContentParent.addView(childJView);
                    _childGeometry = std::make_unique<Property<Rect>>();
                    _childGeometry->bind(viewCore->geometry, BindMode::unidirectional);
                    _childGeometry->onChange() += [=](auto va) {
                        _jContentParent.setSize(std::lround(va->get().width * getUIScaleFactor()),
                                                std::lround(va->get().height * getUIScaleFactor()));
                    };
                }
            }

            void removeChildJView(JView childJView) override
            {
                _childGeometry.reset();
                _jContentParent.removeView(childJView);
            }

            void _scrollChange(int scrollX, int scrollY, int oldScrollX, int oldScrollY) { updateVisibleClientRect(); }

          private:
            void updateVisibleClientRect()
            {
                std::shared_ptr<ScrollView> outer =
                    std::dynamic_pointer_cast<ScrollView>(getOuterViewIfStillAttached());
                if (outer != nullptr) {
                    double uiScaleFactor = getUIScaleFactor();

                    Rect visibleRect(_jNativeScrollView.getScrollX() / uiScaleFactor,
                                     _jNativeScrollView.getScrollY() / uiScaleFactor,
                                     _jNativeScrollView.getWidth() / uiScaleFactor,
                                     _jNativeScrollView.getHeight() / uiScaleFactor);

                    outer->visibleClientRect = (visibleRect);
                }
            }

            JNativeScrollView _jNativeScrollView;
            JNativeViewGroup _jContentParent;

            std::unique_ptr<Property<Rect>> _childGeometry;
        };
    }
}
