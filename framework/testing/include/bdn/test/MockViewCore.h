#pragma once

#include <bdn/IViewCore.h>
#include <bdn/Dip.h>
#include <bdn/test/MockUIProvider.h>
#include <bdn/round.h>

#include <bdn/test.h>

#include <regex>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" view core that does not actually show
           anything visible, but behaves otherwise like a normal view core.

            See MockUIProvider.
            */
        class MockViewCore : public Base, virtual public IViewCore, virtual public LayoutCoordinator::IViewCoreExtension
        {
          public:
            explicit MockViewCore(std::shared_ptr<View> view)
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _outerViewWeak = view;

                _visible = view->visible;
                _padding = view->padding;
                _margin = view->margin;
                _horizontalAlignment = view->horizontalAlignment;
                _verticalAlignment = view->verticalAlignment;
                _preferredSizeHint = view->preferredSizeHint;
                _preferredSizeMinimum = view->preferredSizeMinimum;
                _preferredSizeMaximum = view->preferredSizeMaximum;
                // the bounds should not be copied from the outer object - a
                // layout cycle must happen to initialize it. _bounds = Rect(
                // view->position, view->size );
                _parentViewWeak = view->getParentView();
            }

            ~MockViewCore()
            {
                // core objects must only be released from the main thread.
                BDN_REQUIRE_IN_MAIN_THREAD();
            }

            /** Returns the outer view object that this core is embedded in.*/
            std::shared_ptr<View> getOuterViewIfStillAttached() const { return _outerViewWeak.lock(); }

            /** Returns true if the fake view is currently marked as
             * "visible".*/
            bool getVisible() const { return _visible; }

            /** Returns the number of times the view's visibility state has
             * changed.*/
            int getVisibleChangeCount() const { return _visibleChangeCount; }

            /** Returns the padding that is currently configured.*/
            std::optional<UIMargin> getPadding() const { return _padding; }

            /** Returns the number of times the view's padding has changed.*/
            int getPaddingChangeCount() const { return _paddingChangeCount; }

            /** Returns the margin that is currently configured.*/
            UIMargin getMargin() const { return _margin; }

            /** Returns the number of times the view's margin has changed.*/
            int getMarginChangeCount() const { return _marginChangeCount; }

            /** Returns the current view bounds.*/
            Rect getBounds() const { return _bounds; }

            /** Returns the number of times the view's bounds have changed.*/
            int getBoundsChangeCount() const { return _boundsChangeCount; }

            /** Returns the view's current parent view.
                Note that the MockViewCore does not hold a reference to it, so
               it will not keep the parent view alive. You have to ensure that
               the parent still exists when you access the returned pointer.*/
            std::weak_ptr<View> getParentViewWeak() const { return _parentViewWeak; }

            /** Returns the number of times the view's parent have changed.*/
            int getParentViewChangeCount() const { return _parentViewChangeCount; }

            Size _getTextSize(const String &s, double wrapWidth = std::numeric_limits<double>::infinity()) const
            {
                String remaining = std::regex_replace(s, std::regex("\r\n"), "\n");

                // our fake font has a size of 9.75 x 19 2/3 DIPs for each
                // character.
                double charWidth = 9.75;
                double charHeight = 19 + 2.0 / 3;

                int maxLineChars = -1;
                if (std::isfinite(wrapWidth)) {
                    // round the wrap width down to full mock pixels
                    wrapWidth = Dip::pixelAlign(wrapWidth, 3, RoundType::down);

                    maxLineChars = static_cast<int>(std::floor(wrapWidth / charWidth));

                    if (maxLineChars <= 0)
                        maxLineChars = 1;
                }

                int lineCount = 0;
                double width = 0;
                do {
                    auto [line, remainder] = split(remaining, '\n');

                    remaining = remainder;

                    do {
                        lineCount++;

                        size_t lineChars = line.size();

                        if (maxLineChars != -1 && lineChars > (size_t)maxLineChars) {
                            // find the break point.
                            // Note that if there is a whitespace just FOLLOWING
                            // the maximum number of chars then we want to break
                            // there. So we start searching from that point
                            // backwards.
                            size_t lastWhitespaceIndex = line.find_last_of(" \t", maxLineChars);
                            if (lastWhitespaceIndex == String::npos) {
                                // no white space found.
                                lineChars = maxLineChars;
                            } else {
                                // break before whitespace
                                lineChars = lastWhitespaceIndex;
                            }
                        }

                        double lineWidth = Dip::pixelAlign(lineChars * charWidth, 3, RoundType::up);
                        width = std::max(width, lineWidth);

                        line = line.substr(lineChars);

                        if (!line.empty()) {
                            char32_t firstChar = line[0];

                            // if we auto-wrap to a new line and that line
                            // begins with a whitespace then we do not print it.
                            if (firstChar == ' ' || firstChar == '\t')
                                line = line.substr(1);
                        }
                    } while (!line.empty());
                } while (!remaining.empty());

                double height = lineCount * charHeight;

                return Size(width, height);
            }

            void setVisible(const bool &visible) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _visible = visible;
                _visibleChangeCount++;
            }

            void setPadding(const std::optional<UIMargin> &padding) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _padding = padding;
                _paddingChangeCount++;
            }

            void setMargin(const UIMargin &margin) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _margin = margin;
                _marginChangeCount++;
            }

            void setHorizontalAlignment(const View::HorizontalAlignment &align) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _horizontalAlignment = align;
                _horizontalAlignmentChangeCount++;
            }

            View::HorizontalAlignment getHorizontalAlignment() const { return _horizontalAlignment; }

            int getHorizontalAlignmentChangeCount() const { return _horizontalAlignmentChangeCount; }

            void setVerticalAlignment(const View::VerticalAlignment &align) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _verticalAlignment = align;
                _verticalAlignmentChangeCount++;
            }

            View::VerticalAlignment getVerticalAlignment() const { return _verticalAlignment; }

            int getVerticalAlignmentChangeCount() const { return _verticalAlignmentChangeCount; }

            void setPreferredSizeHint(const Size &hint) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _preferredSizeHint = hint;
                _preferredSizeHintChangeCount++;
            }

            Size getPreferredSizeHint() const { return _preferredSizeHint; }

            int getPreferredSizeHintChangeCount() const { return _preferredSizeHintChangeCount; }

            void setPreferredSizeMinimum(const Size &limit) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _preferredSizeMinimum = limit;
                _preferredSizeMinimumChangeCount++;
            }

            Size getPreferredSizeMinimum() const { return _preferredSizeMinimum; }

            int getPreferredSizeMinimumChangeCount() const { return _preferredSizeMinimumChangeCount; }

            void setPreferredSizeMaximum(const Size &limit) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _preferredSizeMaximum = limit;
                _preferredSizeMaximumChangeCount++;
            }

            Size getPreferredSizeMaximum() const { return _preferredSizeMaximum; }

            int getPreferredSizeMaximumChangeCount() const { return _preferredSizeMaximumChangeCount; }

            Rect adjustAndSetBounds(const Rect &requestedBounds) override
            {
                _bounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);

                _boundsChangeCount++;

                return _bounds;
            }

            Rect adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                              RoundType sizeRoundType) const override
            {
                // our mock UI has 3 pixels per DIP
                return Dip::pixelAlign(requestedBounds, _pixelsPerDip, positionRoundType, sizeRoundType);
            }

            double uiLengthToDips(const UILength &uiLength) const override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                switch (uiLength.unit) {
                case UILength::Unit::none:
                    return 0;

                case UILength::Unit::dip:
                    return uiLength.value;

                case UILength::Unit::em:
                    // one em = 23 mock DIPs;
                    return uiLength.value * 23;

                case UILength::Unit::sem:
                    // one sem = 20 mock DIPs;
                    return uiLength.value * 20;

                default:
                    throw InvalidArgumentError("Invalid UILength unit passed to "
                                               "MockViewCore::uiLengthToDips: " +
                                               std::to_string((int)uiLength.unit));
                }
            }

            Margin uiMarginToDipMargin(const UIMargin &margin) const override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                return Margin(uiLengthToDips(margin.top), uiLengthToDips(margin.right), uiLengthToDips(margin.bottom),
                              uiLengthToDips(margin.left));
            }

            bool canMoveToParentView(std::shared_ptr<View> newParentView) const override { return true; }

            void moveToParentView(std::shared_ptr<View> newParentView) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _parentViewWeak = newParentView;
                _parentViewChangeCount++;
            }

            void dispose() override
            {
                // do nothing
            }

            int getCalcPreferredSizeCount() const { return _calcPreferredSizeCount; }

            Size calcPreferredSize(const Size &availableSpace) const override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _calcPreferredSizeCount++;

                // return a dummy size here. Derived classes need to override
                // this
                return Size(0, 0);
            }

            /** Returns the number of times that the view's layout was
             * updated.*/
            int getLayoutCount() const { return _layoutCount; }

            void layout() override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _layoutCount++;

                if (_overrideLayoutFunc)
                    _overrideLayoutFunc();
            }

            /** Sets a function that is called instead of the normal layout
               function.

                If the return value of the override function is false then the
               normal layout function implementation runs after the override
               function. If the return value is true then the normal
               implementation is not run.
            */
            void setOverrideLayoutFunc(const std::function<bool()> func) { _overrideLayoutFunc = func; }

            int getInvalidateSizingInfoCount() const { return _invalidateSizingInfoCount; }

            void invalidateSizingInfo(View::InvalidateReason reason) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _invalidateSizingInfoCount++;
            }

            int getNeedLayoutCount() const { return _needLayoutCount; }

            void needLayout(View::InvalidateReason reason) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _needLayoutCount++;

                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                if (view != nullptr)
                    std::dynamic_pointer_cast<MockUIProvider>(view->getUIProvider())
                        ->getLayoutCoordinator()
                        ->viewNeedsLayout(view);
            }

            int getChildSizingInfoInvalidatedCount() const { return _childSizingInfoInvalidatedCount; }

            void childSizingInfoInvalidated(std::shared_ptr<View> child) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _childSizingInfoInvalidatedCount++;

                std::shared_ptr<View> outer = getOuterViewIfStillAttached();
                if (outer != nullptr) {
                    outer->invalidateSizingInfo(View::InvalidateReason::childSizingInfoInvalidated);
                    outer->needLayout(View::InvalidateReason::childSizingInfoInvalidated);
                }
            }

          protected:
            bool _visible = false;
            int _visibleChangeCount = 0;

            std::optional<UIMargin> _padding;
            int _paddingChangeCount = 0;

            UIMargin _margin;
            int _marginChangeCount = 0;

            View::HorizontalAlignment _horizontalAlignment;
            int _horizontalAlignmentChangeCount = 0;

            View::VerticalAlignment _verticalAlignment;
            int _verticalAlignmentChangeCount = 0;

            Size _preferredSizeHint;
            int _preferredSizeHintChangeCount = 0;

            Size _preferredSizeMinimum;
            int _preferredSizeMinimumChangeCount = 0;

            Size _preferredSizeMaximum;
            int _preferredSizeMaximumChangeCount = 0;

            Rect _bounds;
            int _boundsChangeCount = 0;

            std::weak_ptr<View> _parentViewWeak;
            int _parentViewChangeCount = 0;

            int _layoutCount = 0;
            std::function<bool()> _overrideLayoutFunc;

            int _invalidateSizingInfoCount = 0;
            int _childSizingInfoInvalidatedCount = 0;
            int _needLayoutCount = 0;
            mutable int _calcPreferredSizeCount = 0;

            std::weak_ptr<View> _outerViewWeak;

            const double _pixelsPerDip = 3; // 3 physical pixels per DIP
        };
    }
}
