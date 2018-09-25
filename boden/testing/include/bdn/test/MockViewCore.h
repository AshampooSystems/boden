#ifndef BDN_TEST_MockViewCore_H_
#define BDN_TEST_MockViewCore_H_

#include <bdn/IViewCore.h>
#include <bdn/Dip.h>
#include <bdn/test/MockUiProvider.h>
#include <bdn/round.h>

#include <bdn/test.h>

namespace bdn
{
    namespace test
    {

        /** Implementation of a "fake" view core that does not actually show
           anything visible, but behaves otherwise like a normal view core.

            See MockUiProvider.
            */
        class MockViewCore
            : public Base,
              BDN_IMPLEMENTS IViewCore,
              BDN_IMPLEMENTS LayoutCoordinator::IViewCoreExtension
        {
          public:
            explicit MockViewCore(View *pView)
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _outerViewWeak = pView;

                _visible = pView->visible();
                _padding = pView->padding();
                _margin = pView->margin();
                _horizontalAlignment = pView->horizontalAlignment();
                _verticalAlignment = pView->verticalAlignment();
                _preferredSizeHint = pView->preferredSizeHint();
                _preferredSizeMinimum = pView->preferredSizeMinimum();
                _preferredSizeMaximum = pView->preferredSizeMaximum();
                // the bounds should not be copied from the outer object - a
                // layout cycle must happen to initialize it. _bounds = Rect(
                // pView->position(), pView->size() );
                _pParentViewWeak = pView->getParentView();
            }

            ~MockViewCore()
            {
                // core objects must only be released from the main thread.
                BDN_REQUIRE_IN_MAIN_THREAD();
            }

            /** Returns the outer view object that this core is embedded in.*/
            P<View> getOuterViewIfStillAttached() const
            {
                return _outerViewWeak.toStrong();
            }

            /** Returns true if the fake view is currently marked as
             * "visible".*/
            bool getVisible() const { return _visible; }

            /** Returns the number of times the view's visibility state has
             * changed.*/
            int getVisibleChangeCount() const { return _visibleChangeCount; }

            /** Returns the padding that is currently configured.*/
            Nullable<UiMargin> getPadding() const { return _padding; }

            /** Returns the number of times the view's padding has changed.*/
            int getPaddingChangeCount() const { return _paddingChangeCount; }

            /** Returns the margin that is currently configured.*/
            UiMargin getMargin() const { return _margin; }

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
            View *getParentViewWeak() const { return _pParentViewWeak; }

            /** Returns the number of times the view's parent have changed.*/
            int getParentViewChangeCount() const
            {
                return _parentViewChangeCount;
            }

            Size _getTextSize(const String &s,
                              double wrapWidth =
                                  std::numeric_limits<double>::infinity()) const
            {
                String remaining(s);

                // normalize line breaks
                remaining.findAndReplace("\r\n", "\n");

                // our fake font has a size of 9.75 x 19 2/3 DIPs for each
                // character.
                double charWidth = 9.75;
                double charHeight = 19 + 2.0 / 3;

                int maxLineChars = -1;
                if (std::isfinite(wrapWidth)) {
                    // round the wrap width down to full mock pixels
                    wrapWidth = Dip::pixelAlign(wrapWidth, 3, RoundType::down);

                    maxLineChars =
                        static_cast<int>(std::floor(wrapWidth / charWidth));

                    if (maxLineChars <= 0)
                        maxLineChars = 1;
                }

                int lineCount = 0;
                double width = 0;
                do {
                    String line = remaining.splitOffToken("\n", true, nullptr);

                    do {
                        lineCount++;

                        size_t lineChars = line.getLength();

                        if (maxLineChars != -1 &&
                            lineChars > (size_t)maxLineChars) {
                            // find the break point.
                            // Note that if there is a whitespace just FOLLOWING
                            // the maximum number of chars then we want to break
                            // there. So we start searching from that point
                            // backwards.
                            size_t lastWhitespaceIndex =
                                line.reverseFindOneOf(" \t", maxLineChars);
                            if (lastWhitespaceIndex == String::npos) {
                                // no white space found.
                                lineChars = maxLineChars;
                            } else {
                                // break before whitespace
                                lineChars = lastWhitespaceIndex;
                            }
                        }

                        double lineWidth = Dip::pixelAlign(
                            lineChars * charWidth, 3, RoundType::up);
                        width = std::max(width, lineWidth);

                        line = line.subString(lineChars);

                        if (!line.isEmpty()) {
                            char32_t firstChar = line[0];

                            // if we auto-wrap to a new line and that line
                            // begins with a whitespace then we do not print it.
                            if (firstChar == ' ' || firstChar == '\t')
                                line = line.subString(1);
                        }
                    } while (!line.isEmpty());
                } while (!remaining.isEmpty());

                double height = lineCount * charHeight;

                return Size(width, height);
            }

            void setVisible(const bool &visible) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _visible = visible;
                _visibleChangeCount++;
            }

            void setPadding(const Nullable<UiMargin> &padding) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _padding = padding;
                _paddingChangeCount++;
            }

            void setMargin(const UiMargin &margin) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _margin = margin;
                _marginChangeCount++;
            }

            void setHorizontalAlignment(
                const View::HorizontalAlignment &align) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _horizontalAlignment = align;
                _horizontalAlignmentChangeCount++;
            }

            View::HorizontalAlignment getHorizontalAlignment() const
            {
                return _horizontalAlignment;
            }

            int getHorizontalAlignmentChangeCount() const
            {
                return _horizontalAlignmentChangeCount;
            }

            void
            setVerticalAlignment(const View::VerticalAlignment &align) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _verticalAlignment = align;
                _verticalAlignmentChangeCount++;
            }

            View::VerticalAlignment getVerticalAlignment() const
            {
                return _verticalAlignment;
            }

            int getVerticalAlignmentChangeCount() const
            {
                return _verticalAlignmentChangeCount;
            }

            void setPreferredSizeHint(const Size &hint) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _preferredSizeHint = hint;
                _preferredSizeHintChangeCount++;
            }

            Size getPreferredSizeHint() const { return _preferredSizeHint; }

            int getPreferredSizeHintChangeCount() const
            {
                return _preferredSizeHintChangeCount;
            }

            void setPreferredSizeMinimum(const Size &limit) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _preferredSizeMinimum = limit;
                _preferredSizeMinimumChangeCount++;
            }

            Size getPreferredSizeMinimum() const
            {
                return _preferredSizeMinimum;
            }

            int getPreferredSizeMinimumChangeCount() const
            {
                return _preferredSizeMinimumChangeCount;
            }

            void setPreferredSizeMaximum(const Size &limit) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _preferredSizeMaximum = limit;
                _preferredSizeMaximumChangeCount++;
            }

            Size getPreferredSizeMaximum() const
            {
                return _preferredSizeMaximum;
            }

            int getPreferredSizeMaximumChangeCount() const
            {
                return _preferredSizeMaximumChangeCount;
            }

            Rect adjustAndSetBounds(const Rect &requestedBounds) override
            {
                _bounds = adjustBounds(requestedBounds, RoundType::nearest,
                                       RoundType::nearest);

                _boundsChangeCount++;

                return _bounds;
            }

            Rect adjustBounds(const Rect &requestedBounds,
                              RoundType positionRoundType,
                              RoundType sizeRoundType) const override
            {
                // our mock UI has 3 pixels per DIP
                return Dip::pixelAlign(requestedBounds, _pixelsPerDip,
                                       positionRoundType, sizeRoundType);
            }

            double uiLengthToDips(const UiLength &uiLength) const override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                switch (uiLength.unit) {
                case UiLength::Unit::none:
                    return 0;

                case UiLength::Unit::dip:
                    return uiLength.value;

                case UiLength::Unit::em:
                    // one em = 23 mock DIPs;
                    return uiLength.value * 23;

                case UiLength::Unit::sem:
                    // one sem = 20 mock DIPs;
                    return uiLength.value * 20;

                default:
                    throw InvalidArgumentError(
                        "Invalid UiLength unit passed to "
                        "MockViewCore::uiLengthToDips: " +
                        std::to_string((int)uiLength.unit));
                }
            }

            Margin uiMarginToDipMargin(const UiMargin &margin) const override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                return Margin(
                    uiLengthToDips(margin.top), uiLengthToDips(margin.right),
                    uiLengthToDips(margin.bottom), uiLengthToDips(margin.left));
            }

            bool tryChangeParentView(View *pNewParent) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _pParentViewWeak = pNewParent;
                _parentViewChangeCount++;
                return true;
            }

            int getCalcPreferredSizeCount() const
            {
                return _calcPreferredSizeCount;
            }

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
            void setOverrideLayoutFunc(const std::function<bool()> func)
            {
                _overrideLayoutFunc = func;
            }

            int getInvalidateSizingInfoCount() const
            {
                return _invalidateSizingInfoCount;
            }

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

                P<View> pView = getOuterViewIfStillAttached();
                if (pView != nullptr)
                    cast<MockUiProvider>(pView->getUiProvider())
                        ->getLayoutCoordinator()
                        ->viewNeedsLayout(pView);
            }

            int getChildSizingInfoInvalidatedCount() const
            {
                return _childSizingInfoInvalidatedCount;
            }

            void childSizingInfoInvalidated(View *pChild) override
            {
                BDN_REQUIRE_IN_MAIN_THREAD();

                _childSizingInfoInvalidatedCount++;

                P<View> pOuter = getOuterViewIfStillAttached();
                if (pOuter != nullptr) {
                    pOuter->invalidateSizingInfo(
                        View::InvalidateReason::childSizingInfoInvalidated);
                    pOuter->needLayout(
                        View::InvalidateReason::childSizingInfoInvalidated);
                }
            }

          protected:
            bool _visible = false;
            int _visibleChangeCount = 0;

            Nullable<UiMargin> _padding;
            int _paddingChangeCount = 0;

            UiMargin _margin;
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

            View *_pParentViewWeak = nullptr;
            int _parentViewChangeCount = 0;

            int _layoutCount = 0;
            std::function<bool()> _overrideLayoutFunc;

            int _invalidateSizingInfoCount = 0;
            int _childSizingInfoInvalidatedCount = 0;
            int _needLayoutCount = 0;
            mutable int _calcPreferredSizeCount = 0;

            WeakP<View> _outerViewWeak = nullptr;

            const double _pixelsPerDip = 3; // 3 physical pixels per DIP
        };
    }
}

#endif
