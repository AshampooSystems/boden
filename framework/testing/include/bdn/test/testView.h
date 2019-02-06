#pragma once

#include <bdn/test/MockUIProvider.h>
#include <bdn/test/ViewWithTestExtensions.h>
#include <bdn/test/testCalcPreferredSize.h>
#include <bdn/test/MockViewCore.h>

namespace bdn
{
    namespace test
    {

        template <class ViewType> inline void _initViewTestPreparerTestView(std::shared_ptr<ViewType> view)
        {
            // do nothing by default
        }

        template <> inline void _initViewTestPreparerTestView<TextView>(std::shared_ptr<TextView> view)
        {
            // must have a text set so that the preferred size hint will have a
            // measurable effect
            view->text = ("hello world");
        }

        template <class ViewType> class ViewTestPreparer : public Base
        {
          public:
            ViewTestPreparer()
            {
                _uiProvider = std::make_shared<MockUIProvider>();
                _window = std::make_shared<Window>(_uiProvider);
            }

            std::shared_ptr<MockUIProvider> getUIProvider() { return _uiProvider; }

            std::shared_ptr<Window> getWindow() { return _window; }

            std::shared_ptr<ViewWithTestExtensions<ViewType>> createView()
            {
                std::shared_ptr<ViewWithTestExtensions<ViewType>> view = createViewWithoutParent();

                _window->setContentView(view);

                return view;
            }

            std::shared_ptr<ViewWithTestExtensions<ViewType>> createViewWithoutParent()
            {
                std::shared_ptr<ViewWithTestExtensions<ViewType>> view =
                    std::make_shared<ViewWithTestExtensions<ViewType>>();

                auto window = std::dynamic_pointer_cast<Window>(view);

                if (window) {
                    window->setContentView(nullptr);
                }

                _initViewTestPreparerTestView<ViewType>(view);

                return view;
            }

            void createLocalView() { ViewType view; }

          protected:
            std::shared_ptr<MockUIProvider> _uiProvider;
            std::shared_ptr<Window> _window;
        };

        template <> class ViewTestPreparer<Window> : public Base
        {
          public:
            ViewTestPreparer() { _uiProvider = std::make_shared<MockUIProvider>(); }

            std::shared_ptr<MockUIProvider> getUIProvider() { return _uiProvider; }

            std::shared_ptr<Window> getWindow() { return _window; }

            std::shared_ptr<ViewWithTestExtensions<Window>> createView() { return createViewWithoutParent(); }

            std::shared_ptr<ViewWithTestExtensions<Window>> createViewWithoutParent()
            {
                std::shared_ptr<ViewWithTestExtensions<Window>> window =
                    std::make_shared<ViewWithTestExtensions<Window>>(_uiProvider);

                _initViewTestPreparerTestView<Window>(window);

                _window = window;

                window->setContentView(nullptr);

                return window;
            }

            void createLocalView() { Window window(_uiProvider); }

          protected:
            std::shared_ptr<MockUIProvider> _uiProvider;
            std::shared_ptr<Window> _window;
        };

        template <class ViewType> inline bool shouldViewBeInitiallyVisible() { return true; }

        template <> inline bool shouldViewBeInitiallyVisible<Window>() { return false; }

        template <class ViewType> inline bool shouldViewHaveParent() { return true; }

        template <> inline bool shouldViewHaveParent<Window>() { return false; }

        /** Used internally - do not use outside the Boden framework.*/
        enum class ExpectedSideEffect_
        {
            invalidateSizingInfo = 1 << 0,
            invalidateLayout = 1 << 1,
            invalidateParentLayout = 1 << 2
        };

        inline int operator|(ExpectedSideEffect_ left, ExpectedSideEffect_ right)
        {
            return static_cast<int>(left) | static_cast<int>(right);
        }

        inline int operator|(int left, ExpectedSideEffect_ right) { return left | static_cast<int>(right); }

        inline int operator&(int left, ExpectedSideEffect_ right) { return left & static_cast<int>(right); }

        /** Used internally. Do not use outside the Boden framework.

            Helper function that performs an operation on a view object and
           verifies the result afterwards.

            _testViewOp may schedule parts of the test to be continued later
            after pending UI events have been handled (see
           CONTINUE_SECTION_WHEN_IDLE). If this happens then the _testViewOp
           function returns before the test has actually finished. The Boden
           test system will automatically manage the postponed continuation of
           the test, but the caller must be aware that local objects that are
           used by the opFunc and verifyFunc might not be there anymore at that
           point in time. To solve this problem the caller can pass an opaque
           object in keepAliveDuringContinuations that is kept alive until all
           test continuations have finished. If only one object is needed by
           opFunc and verifyFunc then it can be passed in
           keepAliveDuringContinuations directly. If multiple objects need to
            be kept alive then one should add them to a container and then pass
           the container in keepAliveDuringContinuations. If no objects need to
           be kept alive artificially then the caller can simply pass null for
           keepAliveDuringContinuations.

            Note that view is automatically kept alive during the test. It is
           not necessary to pass it in keepAliveDuringContinuations.

            @param view the view to perform the operation on
            @param keepAliveDuringContinuations an arbitrary object that is
           kept alive during the test. This can be used to ensure that resources
           needed by opFunc and verifyFunc remain alive, even if the test is
           continued asynchronously (see above for more information).
                keepAliveDuringContinuations can be null if it is not needed.
            @param opFunc a function object that performs the action on the view
            @param verifyFunc a function object that verifies that the view is
           in the expected state after the action.
            @param expectedSideEffects a combination of flags from the
           bdn::ExpectedSideEffect_ enumeration. The flags indicate what kind of
           side effect the operation should have on the layout system.
        */
        template <class ViewType>
        inline void _testViewOp(std::shared_ptr<ViewWithTestExtensions<ViewType>> view,
                                std::shared_ptr<Base> keepAliveDuringContinuations, std::function<void()> opFunc,
                                std::function<void()> verifyFunc, int expectedSideEffects)
        {
            // schedule the test asynchronously, so that the initial sizing
            // info update from the view construction is already done.

            ASYNC_SECTION("mainThread", view, opFunc, verifyFunc, keepAliveDuringContinuations, expectedSideEffects)
            {
                int initialNeedLayoutCount =
                    std::dynamic_pointer_cast<MockViewCore>(view->getViewCore())->getNeedLayoutCount();
                int initialSizingInvalidatedCount =
                    std::dynamic_pointer_cast<MockViewCore>(view->getViewCore())->getInvalidateSizingInfoCount();

                std::shared_ptr<View> parent = view->getParentView();
                int initialParentLayoutInvalidatedCount = 0;
                int initialParentLayoutCount = 0;
                if (parent != nullptr) {
                    initialParentLayoutInvalidatedCount =
                        std::dynamic_pointer_cast<MockViewCore>(parent->getViewCore())->getNeedLayoutCount();
                    initialParentLayoutCount =
                        std::dynamic_pointer_cast<MockViewCore>(parent->getViewCore())->getLayoutCount();
                }

                opFunc();

                // the results of the change may depend on notification calls.
                // Those are posted to the main event queue. So we need to
                // process those now before we verify.
                BDN_CONTINUE_SECTION_WHEN_IDLE(view, parent, keepAliveDuringContinuations, initialNeedLayoutCount,
                                               initialSizingInvalidatedCount, initialParentLayoutInvalidatedCount,
                                               initialParentLayoutCount, expectedSideEffects, verifyFunc)
                {
                    verifyFunc();

                    bool expectSizingInfoInvalidation =
                        (expectedSideEffects & ExpectedSideEffect_::invalidateSizingInfo) != 0;
                    bool expectParentLayoutInvalidation =
                        (expectedSideEffects & ExpectedSideEffect_::invalidateParentLayout) != 0;
                    bool expectLayoutInvalidation = (expectedSideEffects & ExpectedSideEffect_::invalidateLayout) != 0;

                    // if the parent layout is being invalidated then that can
                    // cause the view's size to change, which can also trigger
                    // a child layout invalidation. So if the parent layout is
                    // invalidated then we allow (but do not require) the child
                    // layout to be invalidated as wenn.
                    bool mightInvalidateLayout = expectParentLayoutInvalidation || expectLayoutInvalidation;

                    BDN_REQUIRE(
                        (std::dynamic_pointer_cast<MockViewCore>(view->getViewCore())->getInvalidateSizingInfoCount() >
                         initialSizingInvalidatedCount) == expectSizingInfoInvalidation);

                    if (expectLayoutInvalidation)
                        BDN_REQUIRE(std::dynamic_pointer_cast<MockViewCore>(view->getViewCore())->getNeedLayoutCount() >
                                    initialNeedLayoutCount);
                    else if (!mightInvalidateLayout)
                        BDN_REQUIRE(
                            std::dynamic_pointer_cast<MockViewCore>(view->getViewCore())->getNeedLayoutCount() ==
                            initialNeedLayoutCount);

                    // if the parent should be invalidated and the view is not a
                    // top level window then it MUST have a parent.
                    if (expectParentLayoutInvalidation && std::dynamic_pointer_cast<Window>(view) == nullptr)
                        REQUIRE(parent != nullptr);

                    if (parent != nullptr) {
                        BDN_REQUIRE(
                            (std::dynamic_pointer_cast<MockViewCore>(parent->getViewCore())->getNeedLayoutCount() >
                             initialParentLayoutInvalidatedCount) == expectParentLayoutInvalidation);

                        // verify that the layout was only updates once in total
                        // (if an update was expected)
                        BDN_REQUIRE(std::dynamic_pointer_cast<MockViewCore>(parent->getViewCore())->getLayoutCount() ==
                                    initialParentLayoutCount + (expectParentLayoutInvalidation ? 1 : 0));
                    }
                };
            };
        }

        template <class ViewType>
        inline void testView_Continue(std::shared_ptr<ViewTestPreparer<ViewType>> preparer, int initialCoresCreated,
                                      std::shared_ptr<Window> window,
                                      std::shared_ptr<ViewWithTestExtensions<ViewType>> view,
                                      std::shared_ptr<bdn::test::MockViewCore> core);

        template <class ViewType> inline void testView()
        {
            std::shared_ptr<ViewTestPreparer<ViewType>> preparer = std::make_shared<ViewTestPreparer<ViewType>>();

            int initialCoresCreated = preparer->getUIProvider()->getCoresCreated();

            SECTION("onlyNewAllocAllowed")
            {
                BDN_REQUIRE(preparer->getUIProvider()->getCoresCreated() == initialCoresCreated);
            }

            SECTION("without parent")
            {
                // we test the view before the core is created (i.e. before it
                // has a parent).
                std::shared_ptr<ViewType> view = preparer->createViewWithoutParent();

                SECTION("parent null")
                REQUIRE(view->getParentView() == nullptr);

                if (std::dynamic_pointer_cast<Window>(view) == nullptr) {
                    SECTION("core null")
                    REQUIRE(view->getViewCore() == nullptr);

                    SECTION("calcPreferredSize")
                    REQUIRE(view->calcPreferredSize() == Size(0, 0));

                    SECTION("adjustBounds")
                    REQUIRE(view->adjustBounds(Rect(1, 2, 3, 4), RoundType::nearest, RoundType::nearest) ==
                            Rect(1, 2, 3, 4));

                    SECTION("adjustAndSetBounds")
                    {
                        REQUIRE(view->adjustAndSetBounds(Rect(1, 2, 3, 4)) == Rect(1, 2, 3, 4));
                        REQUIRE(view->position == Point(1, 2));
                        REQUIRE(view->size == Size(3, 4));
                    }
                }
            }

            SECTION("with parent")
            {
                std::shared_ptr<ViewWithTestExtensions<ViewType>> view = preparer->createView();
                // BDN_REQUIRE(preparer->getUIProvider()->getCoresCreated() == initialCoresCreated + 1);

                std::shared_ptr<bdn::test::MockViewCore> core =
                    std::dynamic_pointer_cast<bdn::test::MockViewCore>(view->getViewCore());
                BDN_REQUIRE(core != nullptr);

                std::shared_ptr<Window> window = preparer->getWindow();

                BDN_CONTINUE_SECTION_WHEN_IDLE(preparer, window, view, core)
                {
                    SECTION("initialViewState")
                    {
                        // the core should initialize its properties from the
                        // outer window when it is created. The outer window
                        // should not set them manually after construction.
                        BDN_REQUIRE(core->getPaddingChangeCount() == 0);
                        BDN_REQUIRE(core->getParentViewChangeCount() == 0);

                        BDN_REQUIRE(core->getVisibleChangeCount() == 0);

                        BDN_REQUIRE(view->visible == shouldViewBeInitiallyVisible<ViewType>());

                        BDN_REQUIRE(view->margin == UIMargin(UILength()));
                        BDN_REQUIRE(!view->padding.get());

                        BDN_REQUIRE(view->horizontalAlignment == View::HorizontalAlignment::left);
                        BDN_REQUIRE(view->verticalAlignment == View::VerticalAlignment::top);

                        BDN_REQUIRE(view->preferredSizeHint == Size::none());
                        BDN_REQUIRE(view->preferredSizeMinimum == Size::none());
                        BDN_REQUIRE(view->preferredSizeMaximum == Size::none());

                        BDN_REQUIRE(view->getUIProvider() == preparer->getUIProvider());

                        if (shouldViewHaveParent<ViewType>())
                            BDN_REQUIRE(view->getParentView() ==
                                        std::dynamic_pointer_cast<View>(preparer->getWindow()));
                        else
                            BDN_REQUIRE(view->getParentView() == nullptr);

                        BDN_REQUIRE(view->getViewCore() == core);

                        // the view should not have any child views
                        std::list<std::shared_ptr<View>> childViews = view->getChildViews();
                        BDN_REQUIRE(childViews.empty());
                    }

                    SECTION("invalidateSizingInfo calls core")
                    {
                        int callCountBefore = core->getInvalidateSizingInfoCount();

                        view->invalidateSizingInfo(View::InvalidateReason::customDataChanged);

                        REQUIRE(core->getInvalidateSizingInfoCount() == callCountBefore + 1);
                    }

                    SECTION("invalidateSizingInfo notifies parent")
                    {
                        std::shared_ptr<View> parent = view->getParentView();
                        if (parent != nullptr) {
                            std::shared_ptr<MockViewCore> parentCore =
                                std::dynamic_pointer_cast<MockViewCore>(parent->getViewCore());

                            int childSizingInfoInvalidatedCount = parentCore->getChildSizingInfoInvalidatedCount();
                            int parentSizingInvalidatedCount = parentCore->getInvalidateSizingInfoCount();
                            int parentNeedLayoutCount = parentCore->getNeedLayoutCount();

                            view->invalidateSizingInfo(View::InvalidateReason::customDataChanged);

                            // should have invalidated the parent sizing info
                            // and layout via a childSizingInfoInvalidated call.
                            REQUIRE(parentCore->getChildSizingInfoInvalidatedCount() ==
                                    childSizingInfoInvalidatedCount + 1);
                            REQUIRE(parentCore->getInvalidateSizingInfoCount() == parentSizingInvalidatedCount + 1);
                            REQUIRE(parentCore->getNeedLayoutCount() == parentNeedLayoutCount + 1);
                        }
                    }

                    SECTION("preferred size caching (finite space)")
                    {
                        // fill cache
                        Size prefSize = view->calcPreferredSize(Size(1000, 2000));

                        int calcCountBefore = core->getCalcPreferredSizeCount();

                        // sanity check - should not call view's
                        // calcPreferredSize
                        Size prefSize2 = view->calcPreferredSize(Size(1000, 2000));
                        REQUIRE(core->getCalcPreferredSizeCount() == calcCountBefore);
                        REQUIRE(prefSize2 == prefSize);

                        SECTION("invalidateSizingInfo")
                        {
                            view->invalidateSizingInfo(View::InvalidateReason::customDataChanged);

                            // now call calc again. This time the size should be
                            // freshly calculated again
                            Size prefSize3 = view->calcPreferredSize(Size(1000, 2000));
                            REQUIRE(core->getCalcPreferredSizeCount() == calcCountBefore + 1);
                            REQUIRE(prefSize3 == prefSize);
                        }

                        SECTION("different available space")
                        {
                            view->calcPreferredSize(Size(1001, 2000));
                            REQUIRE(core->getCalcPreferredSizeCount() == calcCountBefore + 1);
                            view->calcPreferredSize(Size(1000, 2001));
                            REQUIRE(core->getCalcPreferredSizeCount() == calcCountBefore + 2);
                        }
                    }

                    SECTION("preferred size caching (infinite space)")
                    {
                        // fill cache
                        Size prefSize = view->calcPreferredSize(Size::none());

                        int calcCountBefore = core->getCalcPreferredSizeCount();

                        SECTION("same infinite space")
                        {
                            Size prefSize2 = view->calcPreferredSize(Size::none());
                            REQUIRE(core->getCalcPreferredSizeCount() == calcCountBefore);
                            REQUIRE(prefSize2 == prefSize);
                        }

                        SECTION("prefSize")
                        {
                            // specifying an available space > prefSize should
                            // return prefSize from the cache
                            Size prefSize2 = view->calcPreferredSize(prefSize);
                            REQUIRE(core->getCalcPreferredSizeCount() == calcCountBefore);
                            REQUIRE(prefSize2 == prefSize);
                        }

                        SECTION("width smaller than prefSize")
                        {
                            view->calcPreferredSize(prefSize - Size(1, 0));
                            // should calculate a new preferred size
                            REQUIRE(core->getCalcPreferredSizeCount() == calcCountBefore + 1);
                        }

                        SECTION("height smaller than prefSize")
                        {
                            view->calcPreferredSize(prefSize - Size(0, 1));
                            // should calculate a new preferred size
                            REQUIRE(core->getCalcPreferredSizeCount() == calcCountBefore + 1);
                        }
                    }

                    SECTION("invalidateSizingInfo")
                    {
                        _testViewOp(view, preparer,
                                    [view]() { view->invalidateSizingInfo(View::InvalidateReason::customDataChanged); },
                                    []() {},
                                    ExpectedSideEffect_::invalidateSizingInfo |
                                        ExpectedSideEffect_::invalidateParentLayout // parent layout is
                                                                                    // invalidated since
                                                                                    // sizing info
                                                                                    // changed
                        );
                    }

                    SECTION("needLayout")
                    {
                        int callCountBefore = core->getNeedLayoutCount();

                        _testViewOp(view, preparer,
                                    [view]() { view->needLayout(View::InvalidateReason::customDataChanged); }, []() {},
                                    0 | ExpectedSideEffect_::invalidateLayout);
                    }

                    SECTION("parentViewNullAfterParentDestroyed")
                    {
                        std::shared_ptr<ViewType> view;

                        {
                            ViewTestPreparer<ViewType> preparer2;

                            view = preparer2.createView();

                            if (shouldViewHaveParent<ViewType>())
                                BDN_REQUIRE(view->getParentView() != nullptr);
                            else
                                BDN_REQUIRE(view->getParentView() == nullptr);
                        }

                        // preparer2 is now gone, so the parent view is not
                        // referenced there anymore. But there may still be a
                        // scheduled sizing info update pending that holds a
                        // reference to the window or child view.
                        // Since we want the window to be destroyed, we do the
                        // remaining test asynchronously after all pending
                        // operations are done.

                        CONTINUE_SECTION_WHEN_IDLE(view, preparer) { BDN_REQUIRE(view->getParentView() == nullptr); };
                    }

                    SECTION("changeViewProperty")
                    {
                        SECTION("visible")
                        {
                            _testViewOp<ViewType>(
                                view, preparer,
                                [view, window]() { view->visible = (!shouldViewBeInitiallyVisible<ViewType>()); },
                                [core, view, window]() {
                                    BDN_REQUIRE(core->getVisibleChangeCount() == 1);
                                    BDN_REQUIRE(core->getVisible() == !shouldViewBeInitiallyVisible<ViewType>());
                                },
                                0 // no layout invalidations
                            );
                        }

                        SECTION("margin")
                        {
                            UIMargin m(UILength::sem(1), UILength::sem(2), UILength::sem(3), UILength::sem(4));

                            _testViewOp<ViewType>(view, preparer, [view, m, window]() { view->margin = (m); },
                                                  [core, m, view, window]() {
                                                      BDN_REQUIRE(core->getMarginChangeCount() == 1);
                                                      BDN_REQUIRE(core->getMargin() == m);

                                                      // margin property should still have the
                                                      // value we set
                                                      REQUIRE(view->margin == m);
                                                  },
                                                  0 | ExpectedSideEffect_::invalidateParentLayout
                                                  // should NOT have caused a sizing info update,
                                                  // since the view's preferred size does not
                                                  // depend on its margin
                            );
                        }

                        SECTION("padding")
                        {
                            UIMargin m(UILength::sem(1), UILength::sem(2), UILength::sem(3), UILength::sem(4));

                            _testViewOp<ViewType>(view, preparer, [view, m, window]() { view->padding = (m); },
                                                  [core, m, view, window]() {
                                                      BDN_REQUIRE(core->getPaddingChangeCount() == 1);
                                                      BDN_REQUIRE(core->getPadding() == m);
                                                  },
                                                  ExpectedSideEffect_::invalidateSizingInfo         // should have
                                                                                                    // caused sizing
                                                                                                    // info to be
                                                                                                    // invalidated
                                                      | ExpectedSideEffect_::invalidateParentLayout // should cause a
                                                                                                    // parent layout
                                                                                                    // update since
                                                                                                    // sizing info
                                                                                                    // was
                                                                                                    // invalidated
                                                      | ExpectedSideEffect_::invalidateLayout // should have caused
                                                                                              // layout to be
                                                                                              // invalidated
                            );
                        }

                        SECTION("horizontalAlignment")
                        {
                            _testViewOp<ViewType>(
                                view, preparer,
                                [view, window]() { view->horizontalAlignment = (View::HorizontalAlignment::center); },
                                [core, view, window]() {
                                    BDN_REQUIRE(core->getHorizontalAlignmentChangeCount() == 1);
                                    BDN_REQUIRE(core->getHorizontalAlignment() == View::HorizontalAlignment::center);
                                },
                                0 | ExpectedSideEffect_::invalidateParentLayout
                                // should not have caused sizing info to be
                                // invalidated but should have invalidated the
                                // parent layout
                            );
                        }

                        SECTION("verticalAlignment")
                        {
                            _testViewOp<ViewType>(
                                view, preparer,
                                [view, window]() { view->verticalAlignment = (View::VerticalAlignment::bottom); },
                                [core, view, window]() {
                                    BDN_REQUIRE(core->getVerticalAlignmentChangeCount() == 1);
                                    BDN_REQUIRE(core->getVerticalAlignment() == View::VerticalAlignment::bottom);
                                },
                                0 | ExpectedSideEffect_::invalidateParentLayout
                                // should not have caused sizing info to be
                                // invalidated but should have invalidated the
                                // parent layout
                            );
                        }

                        SECTION("preferredSizeMinimum")
                        {
                            _testViewOp<ViewType>(view, preparer,
                                                  [view, window]() { view->preferredSizeMinimum = (Size(10, 20)); },
                                                  [core, view, window]() {
                                                      BDN_REQUIRE(core->getPreferredSizeMinimumChangeCount() == 1);
                                                      BDN_REQUIRE(core->getPreferredSizeMinimum() == Size(10, 20));
                                                  },
                                                  ExpectedSideEffect_::invalidateSizingInfo         // should have
                                                                                                    // caused sizing
                                                                                                    // info to be
                                                                                                    // invalidated
                                                      | ExpectedSideEffect_::invalidateParentLayout // should cause a
                                                                                                    // parent layout
                                                                                                    // update since
                                                                                                    // sizing info
                                                                                                    // was
                                                                                                    // invalidated
                            );
                        }

                        SECTION("preferredSizeMaximum")
                        {
                            _testViewOp<ViewType>(view, preparer,
                                                  [view, window]() { view->preferredSizeMaximum = (Size(10, 20)); },
                                                  [core, view, window]() {
                                                      BDN_REQUIRE(core->getPreferredSizeMaximumChangeCount() == 1);
                                                      BDN_REQUIRE(core->getPreferredSizeMaximum() == Size(10, 20));
                                                  },
                                                  ExpectedSideEffect_::invalidateSizingInfo         // should have
                                                                                                    // caused sizing
                                                                                                    // info to be
                                                                                                    // invalidated
                                                      | ExpectedSideEffect_::invalidateParentLayout // should cause a
                                                                                                    // parent layout
                                                                                                    // update since
                                                                                                    // sizing info
                                                                                                    // was
                                                                                                    // invalidated
                            );
                        }

                        SECTION("preferredSizeHint")
                        {
                            _testViewOp<ViewType>(view, preparer,
                                                  [view, window]() { view->preferredSizeHint = (Size(10, 20)); },
                                                  [core, view, window]() {
                                                      BDN_REQUIRE(core->getPreferredSizeHintChangeCount() == 1);
                                                      BDN_REQUIRE(core->getPreferredSizeHint() == Size(10, 20));
                                                  },
                                                  ExpectedSideEffect_::invalidateSizingInfo         // should have
                                                                                                    // caused sizing
                                                                                                    // info to be
                                                                                                    // invalidated
                                                      | ExpectedSideEffect_::invalidateParentLayout // should cause a
                                                                                                    // parent layout
                                                                                                    // update since
                                                                                                    // sizing info
                                                                                                    // was
                                                                                                    // invalidated
                            );
                        }

                        SECTION("adjustAndSetBounds")
                        {
                            SECTION("no need to adjust")
                            {
                                Rect bounds(1, 2, 3, 4);

                                int boundsChangeCountBefore = core->getBoundsChangeCount();

                                _testViewOp<ViewType>(
                                    view, preparer,
                                    [view, bounds, window]() {
                                        Rect adjustedBounds = view->adjustAndSetBounds(bounds);
                                        REQUIRE(adjustedBounds == bounds);
                                    },
                                    [core, bounds, view, window, boundsChangeCountBefore]() {
                                        BDN_REQUIRE(core->getBoundsChangeCount() == boundsChangeCountBefore + 1);
                                        BDN_REQUIRE(core->getBounds() == bounds);

                                        // the view's position and size
                                        // properties should reflect the new
                                        // bounds
                                        BDN_REQUIRE(view->position == bounds.getPosition());
                                        BDN_REQUIRE(view->size == bounds.getSize());
                                    },
                                    0 | ExpectedSideEffect_::invalidateLayout
                                    // should NOT have caused a sizing info
                                    // update should not cause a parent layout
                                    // update should cause a layout update
                                    // (since size was changed)
                                );
                            }

                            SECTION("need adjustment")
                            {
                                Rect bounds(1.3, 2.4, 3.1, 4.9);

                                // the mock view uses 3 pixels per DIP.
                                // Coordinates should be rounded to the NEAREST
                                // value
                                Rect expectedAdjustedBounds(1 + 1.0 / 3, 2 + 1.0 / 3, 3, 5);

                                int boundsChangeCountBefore = core->getBoundsChangeCount();

                                _testViewOp<ViewType>(
                                    view, preparer,
                                    [view, bounds, expectedAdjustedBounds, window]() {
                                        Rect adjustedBounds = view->adjustAndSetBounds(bounds);
                                        REQUIRE(adjustedBounds == expectedAdjustedBounds);
                                    },
                                    [core, expectedAdjustedBounds, view, window, boundsChangeCountBefore]() {
                                        BDN_REQUIRE(core->getBoundsChangeCount() == boundsChangeCountBefore + 1);
                                        BDN_REQUIRE(core->getBounds() == expectedAdjustedBounds);

                                        // the view's position and size
                                        // properties should reflect the new,
                                        // adjusted bounds
                                        BDN_REQUIRE(view->position == expectedAdjustedBounds.getPosition());
                                        BDN_REQUIRE(view->size == expectedAdjustedBounds.getSize());
                                    },
                                    0 | ExpectedSideEffect_::invalidateLayout
                                    // should NOT have caused a sizing info
                                    // update should not a parent layout update
                                    // should cause a layout update (since size
                                    // was changed)
                                );
                            }
                        }

                        SECTION("adjustBounds")
                        {
                            SECTION("no need to adjust")
                            {
                                Rect bounds(1, 2, 3, 4);
                                Rect origBounds = core->getBounds();

                                std::list<RoundType> roundTypes{RoundType::nearest, RoundType::up, RoundType::down};

                                for (RoundType positionRoundType : roundTypes) {
                                    for (RoundType sizeRoundType : roundTypes) {
                                        SECTION("positionRoundType: " + std::to_string((int)positionRoundType) + ", " +
                                                std::to_string((int)sizeRoundType))
                                        {
                                            Rect adjustedBounds =
                                                view->adjustBounds(bounds, positionRoundType, sizeRoundType);

                                            // no adjustments are necessary. So
                                            // we should always get out the same
                                            // that we put in
                                            REQUIRE(adjustedBounds == bounds);

                                            // view properties should not have
                                            // changed
                                            REQUIRE(view->position == origBounds.getPosition());
                                            REQUIRE(view->size == origBounds.getSize());

                                            // the core bounds should not have
                                            // been updated
                                            REQUIRE(core->getBounds() == origBounds);
                                        }
                                    }
                                }
                            }

                            SECTION("need adjustments")
                            {
                                Rect bounds(1.3, 2.4, 3.1, 4.9);
                                Rect origBounds = core->getBounds();

                                std::list<RoundType> roundTypes{RoundType::nearest, RoundType::up, RoundType::down};

                                for (RoundType positionRoundType : roundTypes) {
                                    for (RoundType sizeRoundType : roundTypes) {
                                        SECTION("positionRoundType: " + std::to_string((int)positionRoundType) + ", " +
                                                std::to_string((int)sizeRoundType))
                                        {
                                            Rect adjustedBounds =
                                                view->adjustBounds(bounds, positionRoundType, sizeRoundType);

                                            Point expectedPos;
                                            if (positionRoundType == RoundType::down)
                                                expectedPos = Point(1, 2 + 1.0 / 3);
                                            else if (positionRoundType == RoundType::up)
                                                expectedPos = Point(1 + 1.0 / 3, 2 + 2.0 / 3);
                                            else
                                                expectedPos = Point(1 + 1.0 / 3, 2 + 1.0 / 3);

                                            Size expectedSize;
                                            if (sizeRoundType == RoundType::down)
                                                expectedSize = Size(3, 4 + 2.0 / 3);
                                            else if (sizeRoundType == RoundType::up)
                                                expectedSize = Size(3 + 1.0 / 3, 5);
                                            else
                                                expectedSize = Size(3, 5);

                                            // note that small floating point
                                            // rounding differences are allowed
                                            REQUIRE_ALMOST_EQUAL(adjustedBounds.x, expectedPos.x, 0.0001);
                                            REQUIRE_ALMOST_EQUAL(adjustedBounds.y, expectedPos.y, 0.0001);
                                            REQUIRE_ALMOST_EQUAL(adjustedBounds.width, expectedSize.width, 0.0001);
                                            REQUIRE_ALMOST_EQUAL(adjustedBounds.height, expectedSize.height, 0.0001);

                                            // view properties should not have
                                            // changed
                                            REQUIRE(view->position == origBounds.getPosition());
                                            REQUIRE(view->size == origBounds.getSize());

                                            // the core bounds should not have
                                            // been updated
                                            REQUIRE(core->getBounds() == origBounds);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    SECTION("preferredSize")
                    bdn::test::_testCalcPreferredSize<ViewType, View>(view, view, preparer);

                    SECTION("multiplePropertyChangesThatInfluenceSizing")
                    {
                        _testViewOp<ViewType>(
                            view, preparer,
                            [view, window]() {
                                view->padding =
                                    (UIMargin(UILength::sem(7), UILength::sem(8), UILength::sem(9), UILength::sem(10)));
                                view->padding =
                                    (UIMargin(UILength::sem(6), UILength::sem(7), UILength::sem(8), UILength::sem(9)));
                            },

                            [core, view, window]() {
                                // padding changed twice
                                BDN_REQUIRE(core->getPaddingChangeCount() == 2);
                                BDN_REQUIRE(core->getPadding() == UIMargin(UILength::sem(6), UILength::sem(7),
                                                                           UILength::sem(8), UILength::sem(9)));
                            },

                            ExpectedSideEffect_::invalidateSizingInfo         // should have caused
                                                                              // sizing info to be
                                                                              // invalidated
                                | ExpectedSideEffect_::invalidateParentLayout // should cause a
                                                                              // parent layout
                                                                              // update since
                                                                              // sizing info was
                                                                              // invalidated
                                | ExpectedSideEffect_::invalidateLayout       // should cause layout
                                                                              // update
                        );
                    }
                };
            }
        }
    }
}
