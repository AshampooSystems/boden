#ifndef BDN_TEST_ScrollViewLayoutTesterBase_H_
#define BDN_TEST_ScrollViewLayoutTesterBase_H_

#include <bdn/Button.h>
#include <bdn/TextView.h>
#include <bdn/Dip.h>

namespace bdn
{
    namespace test
    {

        /** A base class for classes that test scroll view preferred size
           calculation and layout. Derive the actual test class from this and
           then call doPreferredSizeAndLayoutTests(). You will need to provide
            an implementation for the unimplemented virtual functions in the
           derived class.

            The template parameter BaseClass indicates the class of
           ScrollViewLayoutTesterBase. In many cases one can specify bdn::Base
           here, to use a default base without any special functionality.

            Specifying a different base class can be useful if
           ScrollViewLayoutTesterBase is to be used as a kind of mixin to add
           functionality on top of another test class.

            */
        template <class BaseClass> class ScrollViewLayoutTesterBase : public BaseClass
        {
          public:
            ScrollViewLayoutTesterBase() {}

            bool _horzScrollingEnabled = false;
            bool _vertScrollingEnabled = true;

            /** Returns the scroll view to use for the tests.*/
            virtual P<ScrollView> getScrollView() = 0;

            /** Returns the width of the vertical scroll bar in DIPs.

                If the vertical scroll bar is only shown on demand during
               scrolling as an overlay (i.e. if no space is allocated for it)
               then this should return 0.
            */
            virtual double getVertBarWidth() = 0;

            /** Returns the height of the horizontal scroll bar in DIPs.

                If the horizontal scroll bar is only shown on demand during
               scrolling as an overlay (i.e. if no space is allocated for it)
               then this should return 0.
            */
            virtual double getHorzBarHeight() = 0;

            /** Calls calcPreferredSize on the object that is tested and returns
             * the result. */
            virtual Size callCalcPreferredSize(const Size &availableSpace = Size::none()) = 0;

            /** Prepares a calcLayout call. This must be called before
               calcLayout and UI events must be handled between the two calls
               (usually by using BDN_CONTINUE_SECTION_WHEN_IDLE ).

                Returns the adjusted viewport size, that the viewport will
               actually end up having (adjusted for the current display, for
               example, pixel aligned).

                */
            virtual Size prepareCalcLayout(const Size &viewPortSize) = 0;

            /** Calculates the scrollview layout for the previously prepared
               viewport size (see prepareCalcLayout).

                This enables the layout verify functions
                (e.g. verifyScrollsHorizontally() etc.) to be used afterwards.*/
            virtual void calcLayoutAfterPreparation() = 0;

            /** Can be called after calcLayoutAfterPreparation() to verify
               whether the view scrolls horizontally.

                "Scrolls horizontally" means that the user can actually scroll
               the contents, i.e. the content does not fit into the viewport.

                Should cause a test fail if the value is not correct.
                */
            virtual void verifyScrollsHorizontally(bool expectedScrolls) = 0;

            /** Can be called after calcLayoutAfterPreparation() to verify
               whether the view scrolls vertically.

                "Scrolls vertically" means that the user can actually scroll the
               contents, i.e. the content does not fit into the viewport.

                Should cause a test fail if the visibility is not correct.
                */
            virtual void verifyScrollsVertically(bool expectedScrolls) = 0;

            /** Can be called after calcLayoutAfterPreparation() to verify the
               bounding rectangle of the content view.

               expectedBounds contains the expected value, assuming  that the
               origin of the scroll view's content coordinate system is (0,0).
               If that is not the case then the implementation of
               verifyContentViewBounds must compensate accordingly.

               The verification function should ignore insignificant
               differences. It can use bdn::Dip for that.

               Should cause a test fail if the content bounds are not correct.

               If the content view is null then the function can be implemented
               in one of these two ways: 1) It can do nothing and ignore the
               call. 2) It may also compare the specified expected values
               against the values a content view WOULD HAVE if one were there,
               if that is possible.
               */
            virtual void verifyContentViewBounds(const Rect &expectedBounds, double maxDeviation = 0) = 0;

            /** Can be called after calcLayoutAfterPreparation() to verify the
               size of the scrollable area (including the scroll view padding
               and content view margins).

                Should cause a test fail if the visibility is not correct.
                */
            virtual void verifyScrolledAreaSize(const Size &expectedSize) = 0;

            /** Can be called after calcLayoutAfterPreparation() to verify the
               size of the view port after layout. If scroll bars are shown then
               they are not part of the view port.

                Should cause a test fail if the view port size is not correct.
                */
            virtual void verifyViewPortSize(const Size &expectedSize) = 0;

            /** Returns the size of 1 pixel in DIPs.
                The default implementation infers this automatically by calling
               View::adjustBounds.*/
            virtual Size getPixelSize()
            {
                P<View> view = getScrollView();

                Rect baseBounds(0, 0, 100, 100);

                // adjust the base bounds up to the nearest pixel
                baseBounds = view->adjustBounds(baseBounds, RoundType::up, RoundType::up);

                // then add a tiny bit of size and round up again
                Rect tinyBitBiggerBounds = Rect(baseBounds.getPosition(), baseBounds.getSize() + Size(0.01, 0.01));
                tinyBitBiggerBounds = view->adjustBounds(tinyBitBiggerBounds, RoundType::up, RoundType::up);

                // the difference in size should be the size of 1 pixel.
                Size pixelSize = tinyBitBiggerBounds.getSize() - baseBounds.getSize();

                // sanity check: the pixel size must not be zero
                REQUIRE(pixelSize > Size(0, 0));

                return pixelSize;
            }

            /** Runs the tests. This calls testPreferredSize() and
             * testLayout().*/
            virtual void doPreferredSizeAndLayoutTests()
            {
                SECTION("scrollable in both directions")
                doPreferredSizeAndLayoutTests(true, true);
                SECTION("only horz scrolling")
                doPreferredSizeAndLayoutTests(true, false);
                SECTION("only vert scrolling")
                doPreferredSizeAndLayoutTests(false, true);
                SECTION("no scrolling")
                doPreferredSizeAndLayoutTests(false, false);
            }

            virtual void doPreferredSizeAndLayoutTests(bool horzScrollingEnabled, bool vertScrollingEnabled)
            {
                _horzScrollingEnabled = horzScrollingEnabled;
                _vertScrollingEnabled = vertScrollingEnabled;

                getScrollView()->setHorizontalScrollingEnabled(horzScrollingEnabled);
                getScrollView()->setVerticalScrollingEnabled(vertScrollingEnabled);

                P<ScrollViewLayoutTesterBase> self = this;

                CONTINUE_SECTION_WHEN_IDLE(self)
                {
                    SECTION("preferredSize")
                    self->testPreferredSize();

                    SECTION("layout")
                    self->testLayout();
                };
            }

            virtual void testPreferredSize()
            {
                P<ScrollViewLayoutTesterBase> self = this;

                Size pixelSize = getPixelSize();

                SECTION("contentview null")
                {
                    SECTION("no padding")
                    {
                        Size prefSize = callCalcPreferredSize(Size::none());
                        REQUIRE(prefSize == Size(0, 0));
                    }

                    SECTION("with padding")
                    {
                        getScrollView()->setPadding(UiMargin(5, 6, 7, 8));

                        Size prefSize = callCalcPreferredSize(Size::none());
                        REQUIRE(prefSize == Size(6 + 8, 5 + 7));
                    }

                    SECTION("with padding and scrollview margin")
                    {
                        // the scrollview's own margin should not matter - only
                        // the margin of the content view
                        getScrollView()->setMargin(UiMargin(1, 2, 3, 4));
                        getScrollView()->setPadding(UiMargin(5, 6, 7, 8));

                        Size prefSize = callCalcPreferredSize(Size::none());
                        REQUIRE(prefSize == Size(6 + 8, 5 + 7));
                    }
                }

                SECTION("contentview not null")
                {
                    P<Button> button = newObj<Button>();
                    getScrollView()->setContentView(button);

                    button->setMargin(UiMargin(1, 2, 3, 4));
                    getScrollView()->setPadding(UiMargin(5, 6, 7, 8));

                    CONTINUE_SECTION_WHEN_IDLE(self, button, pixelSize)
                    {
                        double horzBarHeight = self->getHorzBarHeight();
                        double vertBarWidth = self->getVertBarWidth();

                        Size optimalButtonSize = button->calcPreferredSize();

                        Size optimalSize = Size(2 + 4 + 6 + 8, 1 + 3 + 5 + 7) + optimalButtonSize;

                        SECTION("unlimited space")
                        {
                            // should request as much space as the content needs
                            // by default

                            Size prefSize = self->callCalcPreferredSize(Size::none());
                            REQUIRE(prefSize == optimalSize);
                        }

                        SECTION("more than enough space")
                        {
                            Size prefSize = self->callCalcPreferredSize(optimalSize + Size(100, 100));
                            REQUIRE(prefSize == optimalSize);
                        }

                        SECTION("exactly enough space")
                        {
                            Size prefSize = self->callCalcPreferredSize(optimalSize);
                            REQUIRE(prefSize == optimalSize);
                        }

                        SECTION("less width than needed, more than enough "
                                "height for scrollbar")
                        {
                            Size prefSize = self->callCalcPreferredSize(optimalSize + Size(-pixelSize.width, 100));

                            if (self->_horzScrollingEnabled) {
                                // preferred width should be the available
                                // width. preferred height should include the
                                // scrollbar
                                REQUIRE(Dip::equal(prefSize, optimalSize + Size(-pixelSize.width, horzBarHeight)));
                            } else {
                                // no horz scrolling => no additional scrollbar
                                // at bottom. Also, the content cannot shrink
                                // down to the available space so the returned
                                // width should exceed it and be the optimal
                                // width
                                REQUIRE(Dip::equal(prefSize, optimalSize + Size(0, 0)));
                            }
                        }

                        SECTION("less width than needed, enough height for "
                                "scrollbar")
                        {
                            Size prefSize =
                                self->callCalcPreferredSize(optimalSize + Size(-pixelSize.width, horzBarHeight));

                            if (self->_horzScrollingEnabled) {
                                // preferred width should be the available
                                // width. preferred height should include the
                                // scrollbar
                                REQUIRE(prefSize == optimalSize + Size(-pixelSize.width, horzBarHeight));
                            } else {
                                // no horz scrolling => no additional scrollbar
                                // at bottom. Also, the content cannot shrink
                                // down to the available space so the returned
                                // width should exceed it and be the optimal
                                // width
                                REQUIRE(prefSize == optimalSize + Size(0, 0));
                            }
                        }

                        if (horzBarHeight > 0) {
                            SECTION("less width than needed, not enough height "
                                    "for scrollbar")
                            {
                                Size prefSize = self->callCalcPreferredSize(
                                    optimalSize + Size(-pixelSize.width, horzBarHeight - pixelSize.height));

                                if (self->_horzScrollingEnabled && self->_vertScrollingEnabled) {
                                    // self should cause both scrollbars to be
                                    // shown. We should use all the available
                                    // space
                                    REQUIRE(prefSize ==
                                            optimalSize + Size(-pixelSize.width, horzBarHeight - pixelSize.height));
                                } else if (self->_horzScrollingEnabled) {
                                    // no vertical scrolling. We will get a
                                    // horizontal scrollbar, but no vertical
                                    // scrollbar. Note that the height will
                                    // exceed the available height, since the
                                    // content view also reports a preferred
                                    // size that exceeds the available height.
                                    REQUIRE(prefSize == optimalSize + Size(-pixelSize.width, horzBarHeight));
                                } else {
                                    // no horz scrolling => no additional
                                    // scrollbar at bottom Also, the content
                                    // cannot shrink down to the available space
                                    // so the returned width should exceed it
                                    // and be the optimal width
                                    REQUIRE(prefSize == optimalSize + Size(0, 0));
                                }
                            }
                        }

                        SECTION("less height than needed, more than enough "
                                "width for scrollbar")
                        {
                            Size prefSize = self->callCalcPreferredSize(optimalSize + Size(100, -pixelSize.height));

                            if (self->_vertScrollingEnabled) {
                                REQUIRE(prefSize == optimalSize + Size(vertBarWidth, -pixelSize.height));
                            } else {
                                // no vert scrolling => no scrollbar added. Note
                                // that the reported preferred height should
                                // exceed the available space since the content
                                // view cannot shrink further.
                                REQUIRE(prefSize == optimalSize + Size(0, 0));
                            }
                        }

                        SECTION("less height than needed, enough width for "
                                "scrollbar")
                        {
                            Size prefSize =
                                self->callCalcPreferredSize(optimalSize + Size(vertBarWidth, -pixelSize.height));

                            if (self->_vertScrollingEnabled) {
                                REQUIRE(prefSize == optimalSize + Size(vertBarWidth, -pixelSize.height));
                            } else {
                                // no vert scrolling => no scrollbar added. Note
                                // that the reported preferred height should
                                // exceed the available space since the content
                                // view cannot shrink further.
                                REQUIRE(prefSize == optimalSize + Size(0, 0));
                            }
                        }

                        if (vertBarWidth > 0) {
                            SECTION("less height than needed, not enough width "
                                    "for scrollbar")
                            {
                                Size prefSize = self->callCalcPreferredSize(
                                    optimalSize + Size(vertBarWidth - pixelSize.width, -pixelSize.height));

                                if (self->_vertScrollingEnabled && self->_horzScrollingEnabled) {
                                    // we should fill the available space
                                    REQUIRE(prefSize ==
                                            optimalSize + Size(vertBarWidth - pixelSize.width, -pixelSize.height));
                                } else if (self->_vertScrollingEnabled) {
                                    // the width should exceed the available
                                    // space, since the content cannot be shrunk
                                    // down further. The available height should
                                    // not be exceeded, since we can scroll
                                    REQUIRE(prefSize == optimalSize + Size(vertBarWidth, -pixelSize.height));
                                } else {
                                    // no vert scrolling => no scrollbar added.
                                    // Note that the reported preferred height
                                    // should exceed the available space since
                                    // the content view cannot shrink further.
                                    REQUIRE(prefSize == optimalSize + Size(0, 0));
                                }
                            }
                        }

                        SECTION("less width and less height than needed")
                        {
                            Size prefSize =
                                self->callCalcPreferredSize(optimalSize + Size(-pixelSize.width, -pixelSize.height));

                            if (self->_vertScrollingEnabled && self->_horzScrollingEnabled) {
                                // scroll view should simply use the available
                                // space
                                REQUIRE(prefSize == optimalSize + Size(-pixelSize.width, -pixelSize.height));
                            } else if (self->_vertScrollingEnabled) {
                                // preferred size should be full width plus the
                                // size of the scrollbar (since the content view
                                // cannot shrink below that). Height should be
                                // but available height
                                REQUIRE(prefSize == optimalSize + Size(vertBarWidth, -pixelSize.height));
                            } else if (self->_horzScrollingEnabled) {
                                // preferred size should be full height plus the
                                // size of the scrollbar (since the content view
                                // cannot shrink below that). Width should be
                                // but available width
                                REQUIRE(prefSize == optimalSize + Size(-pixelSize.width, horzBarHeight));
                            } else {
                                // no scrolling. Should simply be the optimal
                                // size, since the content view cannot shrink
                                // beyond that.
                                REQUIRE(prefSize == optimalSize);
                            }
                        }
                    };
                }

                SECTION("contentview calcPreferredSize usage")
                {
                    P<ScrollViewLayoutHelperTestContentView<TextView>> contentView =
                        newObj<ScrollViewLayoutHelperTestContentView<TextView>>();
                    getScrollView()->setContentView(contentView);

                    // we want a content view whose width and height depend on
                    // each other. So we use a text view with multiline text.
                    contentView->setText("Lorem ipsum dolor sit amet, consectetur adipiscing "
                                         "elit.\nPraesent ultrices, nisi quis posuere viverra, "
                                         "arcu erat auctor tellus, sit amet tincidunt magna leo "
                                         "id velit.");

                    CONTINUE_SECTION_WHEN_IDLE(self, contentView, pixelSize)
                    {
                        Size optimalSize = self->callCalcPreferredSize();

                        int initialCalcCount = contentView->getCalcPreferredSizeCallCount();

                        double horzBarHeight = self->getHorzBarHeight();

                        SECTION("available space unlimited")
                        {
                            Size prefSize = self->callCalcPreferredSize();
                            REQUIRE(prefSize == optimalSize);
                            REQUIRE(contentView->getCalcPreferredSizeCallCount() == initialCalcCount + 1);
                        }

                        SECTION("space bigger or equal to needed size")
                        {
                            Size prefSize = self->callCalcPreferredSize(optimalSize);
                            REQUIRE(prefSize == optimalSize);

                            int calcPrefSizeCount = contentView->getCalcPreferredSizeCallCount();
                            REQUIRE(calcPrefSizeCount > initialCalcCount);
                        }

                        SECTION("width less than needed")
                        {
                            Size optimalContentSize = contentView->calcPreferredSize();

                            Size prefSize = self->callCalcPreferredSize(optimalSize + Size(-pixelSize.width, 100));

                            // content view should have been asked for its
                            // preferred sizeat least once
                            REQUIRE(contentView->getCalcPreferredSizeCallCount() > initialCalcCount);

                            if (self->_horzScrollingEnabled) {
                                // space for the scrollbar should have been
                                // added at bottom. Width is the available
                                // space.
                                REQUIRE(prefSize == optimalSize + Size(-pixelSize.width, horzBarHeight));
                            } else {
                                Size contentAvailSpace = contentView->getLastCalcPreferredSizeAvailableSpace();

                                // available space should have been communicated
                                // to the content view
                                REQUIRE(contentAvailSpace.width < optimalContentSize.width);
                                if (self->_vertScrollingEnabled)
                                    REQUIRE(!std::isfinite(contentAvailSpace.height));
                                else
                                    REQUIRE(contentAvailSpace.height == optimalContentSize.height + 100);

                                // the preferred width should be less than
                                // optimal. The height should have increased
                                // (since we need more lines for the text).
                                REQUIRE(prefSize.width < optimalSize.width);
                                REQUIRE(prefSize.height > optimalSize.height);
                            }
                        }

                        SECTION("height less than needed")
                        {
                            Size optimalContentSize = contentView->calcPreferredSize();

                            Size prefSize = self->callCalcPreferredSize(optimalSize + Size(100, -pixelSize.height));

                            // content view should have been asked for its
                            // preferred sizeat least once
                            REQUIRE(contentView->getCalcPreferredSizeCallCount() > initialCalcCount);

                            if (self->_vertScrollingEnabled) {
                                // space for the scrollbar should have been
                                // added at the right side. Height is the
                                // available space.
                                REQUIRE(prefSize == optimalSize + Size(self->getVertBarWidth(), -pixelSize.height));
                            } else {
                                Size contentAvailSpace = contentView->getLastCalcPreferredSizeAvailableSpace();

                                REQUIRE(contentAvailSpace.height < optimalContentSize.height);

                                // available space should have been communicated
                                // to the content view
                                if (self->_horzScrollingEnabled)
                                    REQUIRE(!std::isfinite(contentAvailSpace.width));
                                else
                                    REQUIRE(contentAvailSpace.width == optimalContentSize.width + 100);

                                // should have reported the optimal size since
                                // text views cannot reduce their height.
                                REQUIRE(prefSize == optimalSize);
                            }
                        }
                    };
                }
            }

            void testLayout()
            {
                P<ScrollViewLayoutTesterBase> self = this;

                Size pixelSize = getPixelSize();

                SECTION("contentview null")
                {
                    SECTION("no margin, no padding")
                    {
                        Size viewPortSize = prepareCalcLayout(Size(250, 250));

                        // sanity check
                        REQUIRE(viewPortSize >= Size(245, 245));
                        REQUIRE(viewPortSize <= Size(255, 255));

                        BDN_CONTINUE_SECTION_WHEN_IDLE(self, viewPortSize)
                        {
                            self->calcLayoutAfterPreparation();

                            BDN_CONTINUE_SECTION_WHEN_IDLE(self, viewPortSize)
                            {
                                self->verifyScrollsHorizontally(false);
                                self->verifyScrollsVertically(false);
                                self->verifyContentViewBounds(Rect(Point(), viewPortSize));
                                self->verifyScrolledAreaSize(viewPortSize);
                                self->verifyViewPortSize(viewPortSize);
                            };
                        };
                    }

                    SECTION("padding")
                    {
                        Margin padding(1, 2, 3, 4);

                        // round the padding to full pixels
                        padding.top = stableScaledRound(RoundType::nearest, padding.top, 1.0 / pixelSize.height);
                        padding.bottom = stableScaledRound(RoundType::nearest, padding.bottom, 1.0 / pixelSize.height);
                        padding.left = stableScaledRound(RoundType::nearest, padding.left, 1.0 / pixelSize.width);
                        padding.right = stableScaledRound(RoundType::nearest, padding.right, 1.0 / pixelSize.width);

                        Margin margin(5, 6, 7, 8);

                        // round the margin to full pixels
                        margin.top = stableScaledRound(RoundType::nearest, margin.top, 1.0 / pixelSize.height);
                        margin.bottom = stableScaledRound(RoundType::nearest, margin.bottom, 1.0 / pixelSize.height);
                        margin.left = stableScaledRound(RoundType::nearest, margin.left, 1.0 / pixelSize.width);
                        margin.right = stableScaledRound(RoundType::nearest, margin.right, 1.0 / pixelSize.width);

                        getScrollView()->setPadding(UiMargin(padding.top, padding.right, padding.bottom, padding.left));

                        // the scrollview's margin should not influence the
                        // layout
                        getScrollView()->setMargin(UiMargin(margin.top, margin.right, margin.bottom, margin.left));

                        CONTINUE_SECTION_WHEN_IDLE(self, padding)
                        {
                            Size viewPortSize = self->prepareCalcLayout(Size(250, 250));

                            CONTINUE_SECTION_WHEN_IDLE(self, viewPortSize, padding)
                            {
                                self->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE(self, viewPortSize, padding)
                                {
                                    self->verifyScrollsHorizontally(false);
                                    self->verifyScrollsVertically(false);
                                    self->verifyContentViewBounds(Rect(
                                        padding.left, padding.top, viewPortSize.width - padding.left - padding.right,
                                        viewPortSize.height - padding.top - padding.bottom));
                                    self->verifyScrolledAreaSize(viewPortSize);
                                    self->verifyViewPortSize(viewPortSize);
                                };
                            };
                        };
                    }
                }

                SECTION("contentview not null")
                {
                    P<ScrollViewLayoutHelperTestContentView<Button>> button =
                        newObj<ScrollViewLayoutHelperTestContentView<Button>>();

                    // make sure that the button is not too small. The size of
                    // the content view determines the initial size of the
                    // scroll view that many tests use as a starting point. And
                    // we want that initial size to be bigger than the scroll
                    // bars would be, to ensure that the scroll bars are fully
                    // visible. So to achieve that we set a multiline button
                    // label, which should cause the button to be bigger than
                    // the scroll bars.
                    button->setLabel("Lorem ipsum\ndolor sit amet");

                    getScrollView()->setContentView(button);

                    Margin buttonMargin(1, 2, 3, 4);

                    // round the margin to full pixels
                    buttonMargin.top = stableScaledRound(RoundType::nearest, buttonMargin.top, 1.0 / pixelSize.height);
                    buttonMargin.bottom =
                        stableScaledRound(RoundType::nearest, buttonMargin.bottom, 1.0 / pixelSize.height);
                    buttonMargin.left = stableScaledRound(RoundType::nearest, buttonMargin.left, 1.0 / pixelSize.width);
                    buttonMargin.right =
                        stableScaledRound(RoundType::nearest, buttonMargin.right, 1.0 / pixelSize.width);

                    button->setMargin(
                        UiMargin(buttonMargin.top, buttonMargin.right, buttonMargin.bottom, buttonMargin.left));

                    Margin scrollViewPadding(35, 36, 37, 38);

                    scrollViewPadding.top =
                        stableScaledRound(RoundType::nearest, scrollViewPadding.top, 1.0 / pixelSize.height);
                    scrollViewPadding.bottom =
                        stableScaledRound(RoundType::nearest, scrollViewPadding.bottom, 1.0 / pixelSize.height);
                    scrollViewPadding.left =
                        stableScaledRound(RoundType::nearest, scrollViewPadding.left, 1.0 / pixelSize.width);
                    scrollViewPadding.right =
                        stableScaledRound(RoundType::nearest, scrollViewPadding.right, 1.0 / pixelSize.width);

                    getScrollView()->setPadding(UiMargin(scrollViewPadding.top, scrollViewPadding.right,
                                                         scrollViewPadding.bottom, scrollViewPadding.left));

                    CONTINUE_SECTION_WHEN_IDLE(self, button, pixelSize, buttonMargin, scrollViewPadding)
                    {
                        P<ScrollView> scrollView = self->getScrollView();

                        double horzBarHeight = self->getHorzBarHeight();
                        double vertBarWidth = self->getVertBarWidth();

                        Size optimalButtonSize = button->calcPreferredSize();

                        Rect unadjustedOptimalButtonBounds(
                            Point(buttonMargin.left + scrollViewPadding.left, buttonMargin.top + scrollViewPadding.top),
                            optimalButtonSize);

                        // adjust the optimal size so that it is a multiple of
                        // the physical pixels Note that we round the size up
                        // here, so that the entire button will definitely fit
                        // inside.
                        Rect optimalButtonBounds =
                            button->adjustBounds(unadjustedOptimalButtonBounds, RoundType::nearest, RoundType::up);
                        optimalButtonSize = optimalButtonBounds.getSize();

                        Size optimalSize = optimalButtonSize + buttonMargin + scrollViewPadding;

                        // calculate the adjusted optimal scrollview size, based
                        // on the optimal button bounds.
                        {
                            Rect optimalScrollViewBounds(optimalButtonBounds);
                            optimalScrollViewBounds += buttonMargin + scrollViewPadding;
                            optimalScrollViewBounds = self->getScrollView()->adjustBounds(
                                optimalScrollViewBounds, RoundType::nearest, RoundType::nearest);

                            optimalSize = Size(optimalScrollViewBounds.width, optimalScrollViewBounds.height);
                        }

                        int initialCalcPreferredSizeCallCount = button->getCalcPreferredSizeCallCount();

                        SECTION("much bigger viewport")
                        {
                            Size viewPortSize =
                                scrollView
                                    ->adjustBounds(Rect(scrollView->position(), optimalSize + Size(100, 100)),
                                                   RoundType::nearest, RoundType::nearest)
                                    .getSize();

                            self->prepareCalcLayout(viewPortSize);

                            CONTINUE_SECTION_WHEN_IDLE(self, viewPortSize, button, buttonMargin, scrollViewPadding)
                            {
                                self->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE(self, viewPortSize, button, buttonMargin, scrollViewPadding)
                                {
                                    // content view should be stretched to fill
                                    // whole viewport
                                    self->verifyScrollsHorizontally(false);
                                    self->verifyScrollsVertically(false);

                                    Rect expectedBounds = self->getScrollView()->adjustBounds(
                                        Rect(buttonMargin.left + scrollViewPadding.left,
                                             buttonMargin.top + scrollViewPadding.top,
                                             viewPortSize.width - buttonMargin.right - buttonMargin.left -
                                                 scrollViewPadding.right - scrollViewPadding.left,
                                             viewPortSize.height - buttonMargin.top - buttonMargin.bottom -
                                                 scrollViewPadding.top - scrollViewPadding.bottom),
                                        RoundType::nearest, RoundType::nearest);
                                    self->verifyContentViewBounds(expectedBounds);
                                    self->verifyScrolledAreaSize(viewPortSize);
                                    self->verifyViewPortSize(viewPortSize);

                                    // calcPreferredSize might or might not have
                                    // been called, since the available space is
                                    // big enough to fit in the preferred size.
                                    // So the view object might have optimized
                                    // the call away.
                                };
                            };
                        }

                        SECTION("viewport matches needed content size")
                        {
                            self->prepareCalcLayout(optimalSize);

                            CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds)
                            {
                                self->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds)
                                {
                                    self->verifyScrollsHorizontally(false);
                                    self->verifyScrollsVertically(false);
                                    self->verifyContentViewBounds(optimalButtonBounds);
                                    self->verifyScrolledAreaSize(optimalSize);
                                    self->verifyViewPortSize(optimalSize);

                                    // calcPreferredSize might or might not have
                                    // been called, since the available space is
                                    // big enough to fit in the preferred size.
                                    // So the view object might have optimized
                                    // the call away.
                                };
                            };
                        }

                        SECTION("less width than needed, more than enough "
                                "height for scrollbar")
                        {
                            Size viewPortSize = optimalSize + Size(-pixelSize.width, pixelSize.height * 100);
                            viewPortSize = scrollView
                                               ->adjustBounds(Rect(scrollView->position(), viewPortSize),
                                                              RoundType::nearest, RoundType::nearest)
                                               .getSize();

                            Size addedSize = viewPortSize - optimalSize;

                            self->prepareCalcLayout(viewPortSize);

                            CONTINUE_SECTION_WHEN_IDLE(self, button, initialCalcPreferredSizeCallCount, optimalSize,
                                                       optimalButtonBounds, pixelSize, horzBarHeight, viewPortSize,
                                                       optimalButtonSize, addedSize)
                            {
                                self->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE(self, button, initialCalcPreferredSizeCallCount, optimalSize,
                                                           optimalButtonBounds, pixelSize, horzBarHeight, viewPortSize,
                                                           optimalButtonSize, addedSize)
                                {
                                    self->verifyScrollsHorizontally(self->_horzScrollingEnabled);
                                    self->verifyScrollsVertically(false);

                                    Rect expectedContentViewBounds;

                                    if (self->_horzScrollingEnabled)
                                        expectedContentViewBounds = Rect(optimalButtonBounds.getPosition(),
                                                                         optimalButtonBounds.getSize() +
                                                                             Size(0, addedSize.height - horzBarHeight));
                                    else {
                                        // no scrolling >= no scrollbar. Also
                                        // width is truncated
                                        expectedContentViewBounds = Rect(optimalButtonBounds.getPosition(),
                                                                         optimalButtonBounds.getSize() +
                                                                             Size(-pixelSize.width, addedSize.height));
                                    }

                                    // we allow slight deviation due to floating
                                    // point calculations
                                    self->verifyContentViewBounds(expectedContentViewBounds, 0.0001);

                                    if (self->_horzScrollingEnabled) {
                                        // scrollable area is the optimal width.
                                        // The height is the viewport height
                                        // minus the height of the horizontal
                                        // scrollbar
                                        self->verifyScrolledAreaSize(
                                            Size(optimalSize.width, viewPortSize.height - horzBarHeight));
                                        // horizontal scroll bar is visible, so
                                        // the final viewport size should be
                                        // smaller by that amount
                                        self->verifyViewPortSize(viewPortSize - Size(0, horzBarHeight));
                                    } else {
                                        // width is truncated.
                                        self->verifyScrolledAreaSize(
                                            Size(optimalSize.width - pixelSize.width, viewPortSize.height));
                                        self->verifyViewPortSize(viewPortSize);
                                    }

                                    // calcpreferredsize should have been called
                                    // at least once
                                    REQUIRE(button->getCalcPreferredSizeCallCount() >
                                            initialCalcPreferredSizeCallCount);

                                    if (!self->_horzScrollingEnabled) {
                                        if (self->_vertScrollingEnabled) {
                                            // reported available space should
                                            // have been unlimited height (since
                                            // scrollable) and 1 DIP less width
                                            // than needed. The reported
                                            Size lastAvailSpace = button->getLastCalcPreferredSizeAvailableSpace();
                                            Size expectedLastAvailSpace(optimalButtonSize.width - pixelSize.width,
                                                                        Size::componentNone());
                                            REQUIRE(Dip::equal(lastAvailSpace, expectedLastAvailSpace));
                                        } else {
                                            // reported available space should
                                            // have been the available height
                                            // height (since not scrollable) and
                                            // 1 DIP less width than needed.
                                            Size lastAvailSpace = button->getLastCalcPreferredSizeAvailableSpace();
                                            Size expectedLastAvailSpace =
                                                optimalButtonSize + Size(-pixelSize.width, addedSize.height);
                                            REQUIRE(Dip::equal(lastAvailSpace, expectedLastAvailSpace));
                                        }
                                    }
                                };
                            };
                        }

                        SECTION("less width than needed, enough height for "
                                "scrollbar")
                        {
                            Size viewPortSize = optimalSize + Size(-pixelSize.width, horzBarHeight);
                            viewPortSize = scrollView
                                               ->adjustBounds(Rect(scrollView->position(), viewPortSize),
                                                              RoundType::nearest, RoundType::nearest)
                                               .getSize();

                            Size addedSize = viewPortSize - optimalSize;

                            self->prepareCalcLayout(viewPortSize);

                            CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                       pixelSize, addedSize)
                            {
                                self->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                           pixelSize, addedSize)
                                {
                                    self->verifyScrollsHorizontally(self->_horzScrollingEnabled);
                                    self->verifyScrollsVertically(false);

                                    if (self->_horzScrollingEnabled) {
                                        self->verifyContentViewBounds(optimalButtonBounds);

                                        // scrollable area is the optimal size
                                        self->verifyScrolledAreaSize(optimalSize);

                                        // horizontal scroll bar is visible, so
                                        // the final viewport size should be
                                        // smaller by that amount
                                        self->verifyViewPortSize(viewPortSize - Size(0, addedSize.height));
                                    } else {
                                        // content view should be expanded to
                                        // fill the available height (since
                                        // there is no scrollbar). width is
                                        // truncated
                                        self->verifyContentViewBounds(Rect(
                                            optimalButtonBounds.getPosition(),
                                            optimalButtonBounds.getSize() + Size(-pixelSize.width, addedSize.height)));
                                        self->verifyScrolledAreaSize(optimalSize +
                                                                     Size(-pixelSize.width, addedSize.height));

                                        self->verifyViewPortSize(viewPortSize);
                                    }
                                };
                            };
                        }

                        if (horzBarHeight > 0) {
                            SECTION("less width than needed, not enough height "
                                    "for scrollbar")
                            {
                                Size viewPortSize =
                                    optimalSize + Size(-pixelSize.width, horzBarHeight - pixelSize.height);
                                viewPortSize = scrollView
                                                   ->adjustBounds(Rect(scrollView->position(), viewPortSize),
                                                                  RoundType::nearest, RoundType::nearest)
                                                   .getSize();

                                self->prepareCalcLayout(viewPortSize);

                                CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                           vertBarWidth, horzBarHeight, pixelSize)
                                {
                                    self->calcLayoutAfterPreparation();

                                    CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds,
                                                               viewPortSize, vertBarWidth, horzBarHeight, pixelSize)
                                    {
                                        self->verifyScrollsHorizontally(self->_horzScrollingEnabled);
                                        self->verifyScrollsVertically(
                                            (self->_horzScrollingEnabled && self->_vertScrollingEnabled));

                                        if (self->_horzScrollingEnabled && self->_vertScrollingEnabled) {
                                            // the horizontal scroll bar does
                                            // not fit. So we should also get a
                                            // vertical scrollbar
                                            self->verifyContentViewBounds(optimalButtonBounds);

                                            // scrollable area is the optimal
                                            // size
                                            self->verifyScrolledAreaSize(optimalSize);

                                            // both scroll bars are visible, so
                                            // the final viewport size should be
                                            // smaller by that amount
                                            self->verifyViewPortSize(viewPortSize - Size(vertBarWidth, horzBarHeight));
                                        } else if (self->_horzScrollingEnabled) {
                                            // the viewport height is reduced
                                            // due to the horizontal scroll bar.
                                            // But vert scrolling is not
                                            // allowed, so we won't get a
                                            // vertical scrollbar. So the
                                            // content height will be truncated
                                            // to one less than what is needed.
                                            self->verifyContentViewBounds(
                                                Rect(optimalButtonBounds.getPosition(),
                                                     optimalButtonBounds.getSize() + Size(0, -pixelSize.height)));

                                            // scrollable area is the optimal
                                            // width, since we can scroll. The
                                            // height is optimal-pixelsize,
                                            // since we need to truncate
                                            self->verifyScrolledAreaSize(optimalSize + Size(0, -pixelSize.height));

                                            // only one scroll bar is visible
                                            self->verifyViewPortSize(viewPortSize - Size(0, horzBarHeight));
                                        } else {
                                            // horizontal scrolling is disabled.
                                            // vert scrolling is not needed,
                                            // since without the horz scrollbar
                                            // we have enough height. So there
                                            // will be no scrolling. So it does
                                            // not matter if vert scrolling is
                                            // enabled or not.

                                            // content fills the available space
                                            self->verifyContentViewBounds(
                                                Rect(optimalButtonBounds.getPosition(),
                                                     optimalButtonBounds.getSize() +
                                                         Size(-pixelSize.width, horzBarHeight - pixelSize.height)));

                                            // scrolled area is extended /
                                            // truncated to fit the available
                                            // space
                                            self->verifyScrolledAreaSize(
                                                optimalSize +
                                                Size(-pixelSize.height, horzBarHeight - pixelSize.height));

                                            // viewport stays the same
                                            self->verifyViewPortSize(viewPortSize);
                                        }
                                    };
                                };
                            }
                        }

                        SECTION("less height than needed, more than enough "
                                "width for scrollbar")
                        {
                            Size viewPortSize = optimalSize + Size(100, -pixelSize.height);
                            viewPortSize = scrollView
                                               ->adjustBounds(Rect(scrollView->position(), viewPortSize),
                                                              RoundType::nearest, RoundType::nearest)
                                               .getSize();

                            Size addedSize = viewPortSize - optimalSize;

                            self->prepareCalcLayout(viewPortSize);

                            CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                       vertBarWidth, pixelSize, addedSize)
                            {
                                self->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                           vertBarWidth, pixelSize, addedSize)
                                {
                                    self->verifyScrollsHorizontally(false);
                                    self->verifyScrollsVertically(self->_vertScrollingEnabled);

                                    if (self->_vertScrollingEnabled) {
                                        self->verifyContentViewBounds(Rect(
                                            optimalButtonBounds.getPosition(),
                                            optimalButtonBounds.getSize() + Size(addedSize.width - vertBarWidth, 0)));

                                        // scrollable area width is the viewport
                                        // width minus the scrollbar width.
                                        // Height is the optimal height
                                        self->verifyScrolledAreaSize(
                                            Size(viewPortSize.width - vertBarWidth, optimalSize.height));
                                        // vertical scroll bar is visible, so
                                        // the final viewport size should be
                                        // smaller by that amount
                                        self->verifyViewPortSize(viewPortSize - Size(vertBarWidth, 0));
                                    } else {
                                        // cannot scroll vertically => no
                                        // scrollbar shown. width fills the
                                        // viewport. height is truncated to
                                        // available space

                                        self->verifyContentViewBounds(Rect(
                                            optimalButtonBounds.getPosition(),
                                            optimalButtonBounds.getSize() + Size(addedSize.width, -pixelSize.height)));

                                        // scrollable area width is the viewport
                                        // width. Height is the optimal height,
                                        // truncated to the available space
                                        self->verifyScrolledAreaSize(optimalSize +
                                                                     Size(addedSize.width, -pixelSize.height));

                                        // no scrollbar visible => full viewport
                                        // size
                                        self->verifyViewPortSize(viewPortSize);
                                    }
                                };
                            };
                        }

                        SECTION("less height than needed, enough width for "
                                "scrollbar")
                        {
                            Size viewPortSize = optimalSize + Size(vertBarWidth, -pixelSize.height);
                            viewPortSize = scrollView
                                               ->adjustBounds(Rect(scrollView->position(), viewPortSize),
                                                              RoundType::nearest, RoundType::nearest)
                                               .getSize();

                            self->prepareCalcLayout(viewPortSize);

                            CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                       pixelSize, vertBarWidth)
                            {
                                self->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                           pixelSize, vertBarWidth)
                                {
                                    self->verifyScrollsHorizontally(false);
                                    self->verifyScrollsVertically(self->_vertScrollingEnabled);

                                    if (self->_vertScrollingEnabled) {
                                        self->verifyContentViewBounds(optimalButtonBounds);

                                        // scrollable area size is the optimal
                                        // size
                                        self->verifyScrolledAreaSize(optimalSize);
                                        // vertical scroll bar is visible, so
                                        // the final viewport size should be
                                        // smaller by that amount
                                        self->verifyViewPortSize(viewPortSize - Size(vertBarWidth, 0));
                                    } else {
                                        // cannot scroll vertically => no
                                        // scrollbar shown. width fills the
                                        // viewport. height is truncated to
                                        // available space

                                        self->verifyContentViewBounds(Rect(optimalButtonBounds.getPosition(),
                                                                           optimalButtonBounds.getSize() +
                                                                               Size(vertBarWidth, -pixelSize.height)));

                                        // scrollable area width is the viewport
                                        // width. Height is the optimal height,
                                        // truncated to the available space
                                        self->verifyScrolledAreaSize(optimalSize +
                                                                     Size(vertBarWidth, -pixelSize.height));

                                        // no scrollbar visible => full viewport
                                        // size
                                        self->verifyViewPortSize(viewPortSize);
                                    }
                                };
                            };
                        }

                        if (vertBarWidth > 0) {
                            SECTION("less height than needed, not enough width "
                                    "for scrollbar")
                            {
                                Size viewPortSize =
                                    optimalSize + Size(vertBarWidth - pixelSize.width, -pixelSize.height);
                                viewPortSize = scrollView
                                                   ->adjustBounds(Rect(scrollView->position(), viewPortSize),
                                                                  RoundType::nearest, RoundType::nearest)
                                                   .getSize();

                                self->prepareCalcLayout(viewPortSize);

                                CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                           pixelSize, vertBarWidth, horzBarHeight)
                                {
                                    self->calcLayoutAfterPreparation();

                                    CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds,
                                                               viewPortSize, pixelSize, vertBarWidth, horzBarHeight)
                                    {
                                        self->verifyScrollsHorizontally(
                                            (self->_horzScrollingEnabled && self->_vertScrollingEnabled));
                                        self->verifyScrollsVertically(self->_vertScrollingEnabled);

                                        if (self->_horzScrollingEnabled && self->_vertScrollingEnabled) {
                                            self->verifyContentViewBounds(optimalButtonBounds);

                                            // scrollable area size is the
                                            // optimal size
                                            self->verifyScrolledAreaSize(optimalSize);
                                            // both scroll bars are visible, so
                                            // the final viewport size should be
                                            // smaller by that amount
                                            self->verifyViewPortSize(viewPortSize - Size(vertBarWidth, horzBarHeight));
                                        } else if (self->_vertScrollingEnabled) {
                                            // the viewport width is reduced due
                                            // to the vertical scroll bar. But
                                            // horz scrolling is not allowed, so
                                            // we won't get a horizontal
                                            // scrollbar. So the content width
                                            // will be truncated to one less
                                            // than what is needed.
                                            self->verifyContentViewBounds(
                                                Rect(optimalButtonBounds.getPosition(),
                                                     optimalButtonBounds.getSize() + Size(-pixelSize.width, 0)));

                                            // scrollable area is the optimal
                                            // height, since we can scroll. The
                                            // width is optimal-pixelSize, since
                                            // we need to truncate
                                            self->verifyScrolledAreaSize(optimalSize + Size(-pixelSize.width, 0));

                                            // only one scroll bar is visible
                                            self->verifyViewPortSize(viewPortSize - Size(vertBarWidth, 0));
                                        } else {
                                            // vertical scrolling is disabled.
                                            // horizontal scrolling is not
                                            // needed, since without the
                                            // vertical scrollbar we have enough
                                            // width. So there will be no
                                            // scrolling. So it does not matter
                                            // if horz scrolling is enabled or
                                            // not.

                                            // content fills the available space
                                            self->verifyContentViewBounds(
                                                Rect(optimalButtonBounds.getPosition(),
                                                     optimalButtonBounds.getSize() +
                                                         Size(vertBarWidth - pixelSize.width, -pixelSize.height)));

                                            // scrolled area is extended /
                                            // truncated to fit the available
                                            // space
                                            self->verifyScrolledAreaSize(
                                                optimalSize + Size(vertBarWidth - pixelSize.width, -pixelSize.height));

                                            // viewport stays the same
                                            self->verifyViewPortSize(viewPortSize);
                                        }
                                    };
                                };
                            }
                        }

                        SECTION("less width and less height than needed")
                        {
                            Size viewPortSize = optimalSize + Size(-pixelSize.width, -pixelSize.height);
                            viewPortSize = scrollView
                                               ->adjustBounds(Rect(scrollView->position(), viewPortSize),
                                                              RoundType::nearest, RoundType::nearest)
                                               .getSize();

                            self->prepareCalcLayout(viewPortSize);

                            CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                       pixelSize, vertBarWidth, horzBarHeight)
                            {
                                self->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE(self, button, optimalSize, optimalButtonBounds, viewPortSize,
                                                           pixelSize, vertBarWidth, horzBarHeight)
                                {
                                    self->verifyScrollsHorizontally(self->_horzScrollingEnabled);
                                    self->verifyScrollsVertically(self->_vertScrollingEnabled);

                                    if (self->_horzScrollingEnabled && self->_vertScrollingEnabled) {
                                        self->verifyContentViewBounds(optimalButtonBounds);

                                        self->verifyScrolledAreaSize(optimalSize);
                                        self->verifyViewPortSize(viewPortSize - Size(vertBarWidth, horzBarHeight));
                                    } else if (self->_horzScrollingEnabled) {
                                        // we cannot scroll vertically. So
                                        // height is truncated, width is optimal
                                        self->verifyContentViewBounds(
                                            Rect(optimalButtonBounds.getPosition(),
                                                 optimalButtonBounds.getSize() +
                                                     Size(0, -pixelSize.height - horzBarHeight)));

                                        self->verifyScrolledAreaSize(optimalSize +
                                                                     Size(0, -pixelSize.height - horzBarHeight));

                                        // only one scrollbar is visible
                                        self->verifyViewPortSize(viewPortSize - Size(0, horzBarHeight));
                                    } else if (self->_vertScrollingEnabled) {
                                        // we cannot scroll horizontally. So
                                        // width is truncated, height is optimal
                                        self->verifyContentViewBounds(Rect(
                                            optimalButtonBounds.getPosition(),
                                            optimalButtonBounds.getSize() + Size(-pixelSize.width - vertBarWidth, 0)));

                                        self->verifyScrolledAreaSize(optimalSize +
                                                                     Size(-pixelSize.width - vertBarWidth, 0));

                                        // only one scrollbar is visible
                                        self->verifyViewPortSize(viewPortSize - Size(vertBarWidth, 0));
                                    } else {
                                        // no scrolling. Width and height are
                                        // truncated
                                        self->verifyContentViewBounds(Rect(
                                            optimalButtonBounds.getPosition(),
                                            optimalButtonBounds.getSize() + Size(-pixelSize.width, -pixelSize.height)));

                                        self->verifyScrolledAreaSize(optimalSize +
                                                                     Size(-pixelSize.width, -pixelSize.height));

                                        // only one scrollbar is visible
                                        self->verifyViewPortSize(viewPortSize);
                                    }
                                };
                            };
                        }
                    };
                }

                /* The following test checks if the scrollview can handle huge
                scrollable areas. We have disabled it because for the time being
                we do not support such huge scrollable areas. Note that there
                are also some platform restrictions that make this difficult
                (win32, winuwp). The problem and possible solution are tracked
                in this issue: BDN-220 if(_horzScrollingEnabled &&
                _vertScrollingEnabled)
                {
                    SECTION("huge contentview")
                    {
                        P< ScrollViewLayoutHelperTestContentView<Button> >
                button = newObj< ScrollViewLayoutHelperTestContentView<Button>
                >();

                        button->setPreferredSizeMinimum( Size(100000, 100000)
                );

                        getScrollView()->setContentView(button);

                        CONTINUE_SECTION_WHEN_IDLE(this, button, pixelSize )
                        {
                            P<ScrollView> scrollView = this->getScrollView();

                            Size viewPortSize = Size(300, 300);
                            viewPortSize = scrollView->adjustBounds(
                Rect(scrollView->position(), viewPortSize), RoundType::nearest,
                RoundType::nearest).getSize();

                            Size optimalButtonSize =
                button->calcPreferredSize();

                            Rect unadjustedOptimalButtonBounds( Point(0,0),
                optimalButtonSize);

                            // adjust the optimal size so that it is a multiple
                of the physical pixels Rect optimalButtonBounds =
                button->adjustBounds( unadjustedOptimalButtonBounds,
                RoundType::nearest, RoundType::up ); optimalButtonSize =
                optimalButtonBounds.getSize();

                            this->prepareCalcLayout( viewPortSize );

                            CONTINUE_SECTION_WHEN_IDLE( this, button,
                viewPortSize, pixelSize, optimalButtonBounds )
                            {
                                this->calcLayoutAfterPreparation();

                                CONTINUE_SECTION_WHEN_IDLE( this, button,
                viewPortSize, pixelSize, optimalButtonBounds )
                                {
                                    double horzBarHeight =
                this->getHorzBarHeight(); double vertBarWidth =
                this->getVertBarWidth();

                                    // note that this test is only executed when
                vertical and horizontal scrolling
                                    // are enabled.

                                    this->verifyScrollsHorizontally( true );
                                    this->verifyScrollsVertically( true );

                                    this->verifyContentViewBounds(
                optimalButtonBounds );

                                    this->verifyScrolledAreaSize(
                optimalButtonBounds.getSize() ); this->verifyViewPortSize(
                viewPortSize-Size(vertBarWidth, horzBarHeight) );

                                };
                            };
                        };
                    }
                }*/
            }

          private:
            template <class HelperBaseClass> class ScrollViewLayoutHelperTestContentView : public HelperBaseClass
            {
              public:
                int getCalcPreferredSizeCallCount() const { return _calcPreferredSizeCallCount; }

                Size getLastCalcPreferredSizeAvailableSpace() const { return _lastCalcPreferredSizeAvailableSpace; }

                Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
                {
                    _lastCalcPreferredSizeAvailableSpace = availableSpace;
                    _calcPreferredSizeCallCount++;

                    return HelperBaseClass::calcPreferredSize(availableSpace);
                }

              private:
                mutable int _calcPreferredSizeCallCount = 0;
                mutable Size _lastCalcPreferredSizeAvailableSpace;
            };
        };
    }
}

#endif
