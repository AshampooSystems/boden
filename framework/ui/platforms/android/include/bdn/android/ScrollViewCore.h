#pragma once

#include <bdn/ScrollView.h>
#include <bdn/ScrollViewCore.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/NativeScrollView.h>
#include <bdn/android/wrapper/ViewGroup.h>
#include <bdn/java/wrapper/NativeWeakPointer.h>

namespace bdn::android
{

    class ScrollViewCore : public ViewCore, virtual public bdn::ScrollViewCore
    {
      public:
        ScrollViewCore(const ContextWrapper &ctxt)
            : ScrollViewCore(createAndroidViewClass<wrapper::NativeScrollView>(ctxt).cast<wrapper::NativeScrollView>())
        {}

      private:
        ScrollViewCore(wrapper::NativeScrollView nativeScrollView)
            : ViewCore(nativeScrollView.getWrapperView()), _jNativeScrollView(nativeScrollView),
              _jContentParent(_jNativeScrollView.getContentParent())
        {
            content.onChange() += [=](auto va) { updateContent(va->get()); };
        }

      public:
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

        virtual void visitInternalChildren(std::function<void(std::shared_ptr<bdn::ViewCore>)> function) override
        {
            if (content.get()) {
                function(content->viewCore());
            }
        }

        void updateContent(const std::shared_ptr<View> &content)
        {
            _childGeometry.reset();
            _jContentParent.removeAllViews();

            if (auto childCore = content->core<android::ViewCore>()) {
                _jContentParent.addView(childCore->getJView());
                _childGeometry = std::make_unique<Property<Rect>>();
                _childGeometry->bind(childCore->geometry, BindMode::unidirectional);
                _childGeometry->onChange() += [=](auto va) {
                    _jContentParent.setSize(std::lround(va->get().width * getUIScaleFactor()),
                                            std::lround(va->get().height * getUIScaleFactor()));
                };
            } else {
                throw std::runtime_error("Cannot add Child with this type of core");
            }

            updateChildren();
        }

        virtual void updateGeometry() override
        {
            ViewCore::updateGeometry();
            updateVisibleClientRect();
        }

        void _scrollChange(int scrollX, int scrollY, int oldScrollX, int oldScrollY) { updateVisibleClientRect(); }

      private:
        void updateVisibleClientRect()
        {
            double uiScaleFactor = getUIScaleFactor();

            Rect visibleRect(
                _jNativeScrollView.getScrollX() / uiScaleFactor, _jNativeScrollView.getScrollY() / uiScaleFactor,
                _jNativeScrollView.getWidth() / uiScaleFactor, _jNativeScrollView.getHeight() / uiScaleFactor);

            visibleClientRect = (visibleRect);
        }

        wrapper::NativeScrollView _jNativeScrollView;
        wrapper::NativeViewGroup _jContentParent;

        std::unique_ptr<Property<Rect>> _childGeometry;
    };
}
