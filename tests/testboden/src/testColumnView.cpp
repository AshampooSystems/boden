
#include <bdn/test.h>

#include <bdn/ColumnView.h>
#include <bdn/Button.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockViewCore.h>
#include <bdn/test/MockContainerViewCore.h>

using namespace bdn;

static void testChildAlignment(std::shared_ptr<bdn::test::ViewTestPreparer<ColumnView>> preparer,
                               std::shared_ptr<bdn::test::ViewWithTestExtensions<ColumnView>> columnView,
                               std::shared_ptr<Button> button, View::HorizontalAlignment horzAlign,
                               View::VerticalAlignment vertAlign)
{
    // add a second button that is considerably bigger.
    // That will cause the column view to become bigger.
    std::shared_ptr<Button> button2 = std::make_shared<Button>();
    button2->padding = (UIMargin(500, 500));

    columnView->addChildView(button2);

    if (button->horizontalAlignment == horzAlign) {
        // change to another horizontal alignment, so that the setting
        // of the requested alignment is registered as a change
        button->horizontalAlignment =
            ((horzAlign == View::HorizontalAlignment::left) ? View::HorizontalAlignment::right
                                                            : View::HorizontalAlignment::left);
    }

    if (button->verticalAlignment == vertAlign) {
        // change to another vertical alignment, so that the setting
        // of the requested alignment is registered as a change
        button->verticalAlignment = ((vertAlign == View::VerticalAlignment::top) ? View::VerticalAlignment::bottom
                                                                                 : View::VerticalAlignment::top);
    }

    CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, horzAlign, vertAlign)
    {
        int layoutCountBefore =
            std::dynamic_pointer_cast<bdn::test::MockViewCore>(columnView->getViewCore())->getLayoutCount();

        Rect buttonBoundsBefore = Rect(button->position, button->size);

        SECTION("horizontal")
        {
            button->horizontalAlignment = (horzAlign);

            CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, horzAlign, layoutCountBefore)
            {
                // but layout should have happened
                REQUIRE(
                    std::dynamic_pointer_cast<bdn::test::MockViewCore>(columnView->getViewCore())->getLayoutCount() ==
                    layoutCountBefore + 1);

                Margin margin = button->uiMarginToDipMargin(button->margin);

                Rect bounds = Rect(button->position, button->size);
                Rect containerBounds = Rect(columnView->position, columnView->size);

                // sanity check: the button should be smaller than the
                // columnview unless the alignment is "expand"
                if (horzAlign != View::HorizontalAlignment::expand)
                    REQUIRE(bounds.width < containerBounds.width);

                // and the view should now be aligned accordingly.
                if (horzAlign == View::HorizontalAlignment::left) {
                    REQUIRE_ALMOST_EQUAL(bounds.x, margin.left, 0.0000001);
                } else if (horzAlign == View::HorizontalAlignment::center) {
                    REQUIRE_ALMOST_EQUAL(bounds.x,
                                         margin.left +
                                             (containerBounds.width - (bounds.width + margin.left + margin.right)) / 2,
                                         0.0000001);
                } else if (horzAlign == View::HorizontalAlignment::right) {
                    REQUIRE_ALMOST_EQUAL(bounds.x, containerBounds.width - margin.right - bounds.width, 0.0000001);
                } else if (horzAlign == View::HorizontalAlignment::expand) {
                    REQUIRE_ALMOST_EQUAL(bounds.x, margin.left, 0.0000001);
                    REQUIRE_ALMOST_EQUAL(bounds.width, containerBounds.width - margin.left - margin.right, 0.0000001);
                }
            };
        }

        SECTION("vertical")
        {
            button->verticalAlignment = (vertAlign);

            CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, layoutCountBefore, buttonBoundsBefore)
            {
                // layout should have been invalidated
                REQUIRE(
                    std::dynamic_pointer_cast<bdn::test::MockViewCore>(columnView->getViewCore())->getLayoutCount() ==
                    layoutCountBefore + 1);

                // vertical alignment should have NO effect in a column view.
                Rect bounds = Rect(button->position, button->size);

                REQUIRE(bounds == buttonBoundsBefore);
            };
        }
    };
}

static void verifyPixelMultiple(double val)
{
    // the mock view we use simulates 3 physical pixels per DIP.
    double physicalPixels = val * 3;

    // so the value should be reasonably close to an integer value
    REQUIRE_ALMOST_EQUAL(physicalPixels, std::round(physicalPixels), 0.0000001);
}

static void verifyPixelMultiple(Size size)
{
    verifyPixelMultiple(size.width);
    verifyPixelMultiple(size.height);
}

static void verifyPixelMultiple(Point point)
{
    verifyPixelMultiple(point.x);
    verifyPixelMultiple(point.y);
}

TEST_CASE("ColumnView")
{
    // test the generic view properties of Button
    SECTION("View-base")
    bdn::test::testView<ColumnView>();

    SECTION("ColumnView-specific")
    {
        std::shared_ptr<bdn::test::ViewTestPreparer<ColumnView>> preparer =
            std::make_shared<bdn::test::ViewTestPreparer<ColumnView>>();
        std::shared_ptr<bdn::test::ViewWithTestExtensions<ColumnView>> columnView = preparer->createView();
        std::shared_ptr<bdn::test::MockContainerViewCore> core =
            std::dynamic_pointer_cast<bdn::test::MockContainerViewCore>(columnView->getViewCore());

        REQUIRE(core != nullptr);

        std::shared_ptr<Button> button = std::make_shared<Button>();

        button->adjustAndSetBounds(Rect(10, 10, 10, 10));

        SECTION("no child view")
        {
            SECTION("getChildList")
            {
                std::list<std::shared_ptr<View>> childList = columnView->getChildViews();

                REQUIRE(childList.empty());
            }

            SECTION("removeAllChildViews")
            {
                columnView->removeAllChildViews();

                std::list<std::shared_ptr<View>> childList = columnView->getChildViews();

                REQUIRE(childList.empty());
            }

            SECTION("addChildView")
            {
                CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, core)
                {
                    int layoutCountBefore =
                        std::dynamic_pointer_cast<bdn::test::MockViewCore>(columnView->getViewCore())->getLayoutCount();

                    columnView->addChildView(button);

                    // let scheduled property updates propagate
                    CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, core, layoutCountBefore)
                    {
                        // should cause a layout update.
                        REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(columnView->getViewCore())
                                    ->getLayoutCount() == layoutCountBefore + 1);

                        Size preferredSize = columnView->calcPreferredSize();

                        Size buttonPreferredSize = button->calcPreferredSize();

                        REQUIRE(preferredSize != Size(0, 0));

                        // the column view must ensure that the button gets a
                        // valid size for our mock display. So the button's
                        // preferred size must be rounded up to full mock
                        // pixels. We have 3 mock pixels per DIP, so that is
                        // what we should get
                        Rect buttonBounds(Point(), buttonPreferredSize);
                        Rect adjustedButtonBounds = core->adjustBounds(buttonBounds, RoundType::nearest, RoundType::up);

                        REQUIRE(preferredSize == adjustedButtonBounds.getSize());
                    };
                };
            }
        }

        SECTION("with child view")
        {
            columnView->addChildView(button);

            preparer->getWindow()->requestAutoSize();

            CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, core){
                SECTION("child margins"){Size preferredSizeBefore = columnView->calcPreferredSize();
            int layoutCountBefore =
                std::dynamic_pointer_cast<bdn::test::MockViewCore>(columnView->getViewCore())->getLayoutCount();

            button->margin = (UIMargin(1, 2, 3, 4));

            CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, core, preferredSizeBefore, layoutCountBefore)
            {
                // should cause a layout update for the column view
                REQUIRE(
                    std::dynamic_pointer_cast<bdn::test::MockViewCore>(columnView->getViewCore())->getLayoutCount() ==
                    layoutCountBefore + 1);

                Size preferredSize = columnView->calcPreferredSize();

                REQUIRE(preferredSize == preferredSizeBefore + Margin(1, 2, 3, 4));
            };
        }

        SECTION("child alignment")
        {
            for (int horzAlign = (int)View::HorizontalAlignment::left;
                 horzAlign <= (int)View::HorizontalAlignment::expand; horzAlign++) {
                for (int vertAlign = (int)View::VerticalAlignment::top;
                     vertAlign <= (int)View::VerticalAlignment::expand; vertAlign++) {
                    SECTION(std::to_string(horzAlign) + ", " + std::to_string(vertAlign))
                    {
                        SECTION("no margin")
                        testChildAlignment(preparer, columnView, button, (View::HorizontalAlignment)horzAlign,
                                           (View::VerticalAlignment)vertAlign);

                        SECTION("with margin")
                        {
                            button->margin = (UIMargin(10, 20, 30, 40));

                            testChildAlignment(preparer, columnView, button, (View::HorizontalAlignment)horzAlign,
                                               (View::VerticalAlignment)vertAlign);
                        }
                    }
                }
            }
        }

        SECTION("position and size pixel aligned")
        {
            // add a weird margin to the button to bring everything out of pixel
            // alignment
            button->margin = (UIMargin(0.1234567));

            CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, core)
            {
                verifyPixelMultiple(button->position);
                verifyPixelMultiple(button->size);
            };
        }

        SECTION("calcContainerPreferredSize")
        {
            SECTION("availableWidth bigger than needed")
            {
                Size unrestrictedSize = columnView->calcContainerPreferredSize();

                Size size =
                    columnView->calcContainerPreferredSize(Size(unrestrictedSize.width + 1, Size::componentNone()));

                // should be the same as the unresctricted size
                REQUIRE_ALMOST_EQUAL(size, unrestrictedSize, Size(0.0000001, 0.0000001));
            }

            SECTION("availableWidth exactly same as needed")
            {
                Size unrestrictedSize = columnView->calcContainerPreferredSize();

                Size size = columnView->calcContainerPreferredSize(Size(unrestrictedSize.width, Size::componentNone()));

                // should be the same as the unresctricted size
                REQUIRE_ALMOST_EQUAL(size, unrestrictedSize, Size(0.0000001, 0.0000001));
            }

            SECTION("availableWidth smaller than needed")
            {
                Size unrestrictedSize = columnView->calcContainerPreferredSize();
                Size size =
                    columnView->calcContainerPreferredSize(Size(unrestrictedSize.width / 2, Size::componentNone()));

                // should still report almost the unrestricted size since none
                // of the child views can be shrunk.
                REQUIRE_ALMOST_EQUAL(size, unrestrictedSize, Size(0.0000001, 0.0000001));
            }
        }

        SECTION("getChildList")
        {
            std::list<std::shared_ptr<View>> childList = columnView->getChildViews();

            REQUIRE(childList.size() == 1);
            REQUIRE(childList.front() == std::dynamic_pointer_cast<View>(button));
        }

        SECTION("removeAllChildViews")
        {
            columnView->removeAllChildViews();

            std::list<std::shared_ptr<View>> childList = columnView->getChildViews();

            REQUIRE(childList.empty());

            REQUIRE(button->getParentView() == nullptr);
        }

        SECTION("child views detached before destruction begins")
        {
            struct LocalTestData_ : public Base
            {
                bool destructorRun = false;
                int childParentStillSet = -1;
                int childListEmpty = -1;
            };

            std::shared_ptr<LocalTestData_> data = std::make_shared<LocalTestData_>();

            columnView->setDestructFunc([data, button](bdn::test::ViewWithTestExtensions<ColumnView> *colView) {
                data->destructorRun = true;
                data->childParentStillSet = (button->getParentView() != nullptr) ? 1 : 0;

                std::list<std::shared_ptr<View>> childList = colView->getChildViews();
                data->childListEmpty = (childList.empty() ? 1 : 0);
            });

            BDN_CONTINUE_SECTION_WHEN_IDLE(data, button)
            {
                // All test objects should have been destroyed by now.
                // First verify that the destructor was even called.
                REQUIRE(data->destructorRun);

                // now verify what we actually want to test: that the
                // content view's parent was set to null before the destructor
                // of the parent was called.
                REQUIRE(data->childParentStillSet == 0);

                /* TODO: How did this work?
                List<std::shared_ptr<View>> childList =
                    pColView->getChildViews();
                pData->childListEmpty = (childList.empty() ? 1 : 0);
                */
            };
        }
    };
}

SECTION("multiple child views properly arranged")
{
    columnView->addChildView(button);

    std::shared_ptr<Button> button2 = std::make_shared<Button>();
    columnView->addChildView(button2);

    preparer->getWindow()->requestAutoSize();

    Margin m;
    Margin m2;

    SECTION("no margins")
    {
        m = Margin(0);
        m2 = Margin(0);
    }

    SECTION("with margins")
    {
        m = Margin(10, 20, 30, 40);
        m2 = Margin(11, 22, 33, 44);
    }

    button->margin = (UIMargin(m.top, m.right, m.bottom, m.left));
    button2->margin = (UIMargin(m2.top, m2.right, m2.bottom, m2.left));

    CONTINUE_SECTION_WHEN_IDLE(preparer, columnView, button, button2, core, m, m2)
    {
        Rect bounds = Rect(button->position, button->size);
        Rect bounds2 = Rect(button2->position, button2->size);

        REQUIRE(bounds.x == m.left);
        REQUIRE(bounds.y == m.top);
        // width and height should have been rounded up to full pixels.
        // Since our mock view has 3 pixels per DIP, we need to round up
        // accordingly.
        REQUIRE(bounds.width == stableScaledRoundUp(button->calcPreferredSize().width, 3));
        REQUIRE(bounds.height == stableScaledRoundUp(button->calcPreferredSize().height, 3));

        REQUIRE(bounds2.x == m2.left);
        REQUIRE(bounds2.y == bounds.y + bounds.height + m.bottom + m2.top);
        REQUIRE(bounds2.width == stableScaledRoundUp(button2->calcPreferredSize().width, 3));
        REQUIRE(bounds2.height == stableScaledRoundUp(button2->calcPreferredSize().height, 3));
    };
}
}
}
