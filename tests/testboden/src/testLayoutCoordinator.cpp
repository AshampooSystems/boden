
#include <bdn/test.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/Button.h>

#include <bdn/test/MockUIProvider.h>
#include <bdn/test/MockViewCore.h>
#include <bdn/test/MockWindowCore.h>

using namespace bdn;

class LayoutCoordinatorForTesting : public LayoutCoordinator
{
  public:
    int exceptionCount = 0;

  protected:
    void handleException(const std::exception *exceptionIfAvailable, const String &functionName) override
    {
        // we do not want exceptions to be logged. So we do not call the default
        // implementation.
        exceptionCount++;
    }
};

TEST_CASE("LayoutCoordinator")
{
    std::shared_ptr<LayoutCoordinatorForTesting> coord = std::make_shared<LayoutCoordinatorForTesting>();

    std::shared_ptr<bdn::test::MockUIProvider> uiProvider = std::make_shared<bdn::test::MockUIProvider>();

    std::shared_ptr<Window> view1 = std::make_shared<Window>(uiProvider);
    std::shared_ptr<Window> view2 = std::make_shared<Window>(uiProvider);
    std::shared_ptr<Window> view3 = std::make_shared<Window>(uiProvider);

    view1->setContentView(nullptr);
    view2->setContentView(nullptr);
    view3->setContentView(nullptr);

    // wait for the initial updates by the global layout coordinator to be done
    CONTINUE_SECTION_WHEN_IDLE(uiProvider, coord, view1, view2, view3){
        SECTION("exception"){std::shared_ptr<Window> centerWindow = std::make_shared<Window>(uiProvider);

    centerWindow->setContentView(nullptr);

    int initialCenterCount =
        std::dynamic_pointer_cast<bdn::test::MockWindowCore>(centerWindow->getViewCore())->getCenterCount();

    // centerWindow is always scheduled for centering. This is because window
    // centering is always done last, so this allows us to reasonably detect
    // when an exception causes as abort.
    coord->windowNeedsCentering(centerWindow);

    SECTION("layout throws exception")
    {
        std::dynamic_pointer_cast<bdn::test::MockViewCore>(view2->getViewCore())->setOverrideLayoutFunc([]() {
            throw std::exception();
            return false;
        });

        int initialCount1 = std::dynamic_pointer_cast<bdn::test::MockViewCore>(view1->getViewCore())->getLayoutCount();
        int initialCount2 = std::dynamic_pointer_cast<bdn::test::MockViewCore>(view2->getViewCore())->getLayoutCount();
        int initialCount3 = std::dynamic_pointer_cast<bdn::test::MockViewCore>(view3->getViewCore())->getLayoutCount();

        coord->viewNeedsLayout(view1);
        coord->viewNeedsLayout(view2);
        coord->viewNeedsLayout(view3);

        CONTINUE_SECTION_WHEN_IDLE(coord, view1, view2, view3, initialCount1, initialCount2, initialCount3,
                                   centerWindow, initialCenterCount)
        {
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(view1->getViewCore())->getLayoutCount() ==
                    initialCount1 + 1);
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(view2->getViewCore())->getLayoutCount() ==
                    initialCount2 + 1);
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(view2->getViewCore())->getLayoutCount() ==
                    initialCount3 + 1);

            REQUIRE(
                std::dynamic_pointer_cast<bdn::test::MockWindowCore>(centerWindow->getViewCore())->getCenterCount() ==
                initialCenterCount + 1);

            REQUIRE(coord->exceptionCount == 1);
        };
    }

    SECTION("autoSize throws exception")
    {
        int initialCount1 =
            std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view1->getViewCore())->getAutoSizeCount();
        int initialCount2 =
            std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view2->getViewCore())->getAutoSizeCount();
        int initialCount3 =
            std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view3->getViewCore())->getAutoSizeCount();

        int initialCenterWindowCenterCount =
            std::dynamic_pointer_cast<bdn::test::MockWindowCore>(centerWindow->getViewCore())->getCenterCount();

        std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view2->getViewCore())->setOverrideAutoSizeFunc([]() {
            throw std::exception();
            return false;
        });

        coord->windowNeedsAutoSizing(view1);
        coord->windowNeedsAutoSizing(view2);
        coord->windowNeedsAutoSizing(view3);

        CONTINUE_SECTION_WHEN_IDLE(coord, view1, view2, view3, centerWindow, initialCount1, initialCount2,
                                   initialCount3, initialCenterWindowCenterCount)
        {
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view1->getViewCore())->getAutoSizeCount() ==
                    initialCount1 + 1);
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view2->getViewCore())->getAutoSizeCount() ==
                    initialCount2 + 1);
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view3->getViewCore())->getAutoSizeCount() ==
                    initialCount3 + 1);
            REQUIRE(
                std::dynamic_pointer_cast<bdn::test::MockWindowCore>(centerWindow->getViewCore())->getCenterCount() ==
                initialCenterWindowCenterCount + 1);

            REQUIRE(coord->exceptionCount == 1);
        };
    }

    SECTION("center throws exception")
    {
        int initialCount1 =
            std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view1->getViewCore())->getCenterCount();
        int initialCount2 =
            std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view2->getViewCore())->getCenterCount();
        int initialCount3 =
            std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view3->getViewCore())->getCenterCount();

        int initialCenterWindowCenterCount =
            std::dynamic_pointer_cast<bdn::test::MockWindowCore>(centerWindow->getViewCore())->getCenterCount();

        std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view2->getViewCore())->setOverrideCenterFunc([]() {
            throw std::exception();
            return false;
        });

        coord->windowNeedsCentering(view1);
        coord->windowNeedsCentering(view2);
        coord->windowNeedsCentering(view3);

        CONTINUE_SECTION_WHEN_IDLE(coord, view1, view2, view3, centerWindow, initialCount1, initialCount2,
                                   initialCount3, initialCenterWindowCenterCount)
        {
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view1->getViewCore())->getCenterCount() ==
                    initialCount1 + 1);
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view2->getViewCore())->getCenterCount() ==
                    initialCount2 + 1);
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockWindowCore>(view3->getViewCore())->getCenterCount() ==
                    initialCount3 + 1);
            REQUIRE(
                std::dynamic_pointer_cast<bdn::test::MockWindowCore>(centerWindow->getViewCore())->getCenterCount() ==
                initialCenterWindowCenterCount + 1);

            REQUIRE(coord->exceptionCount == 1);
        };
    }
}

SECTION("Pending events are handled between layouts")
{
    // Layout operations are handled by the coordinator in "parent first" order.
    // Parents are layouted before their children, since the children's size
    // might change during the parent layout. BUT that introduces a special
    // case: when the size of a child view changes then the child view will
    // usually automatically schedule a layout in the corresponding property
    // change handler. That is also the correct behaviour. But the property
    // change handler call is posted to the event queue, so it does not happen
    // immediately during the parent layout. If the child was now already
    // scheduled for layout, then it is important that the pending change events
    // are executed before the child's layout is started. Otherwise the
    // duplicate layout will not be detected and the child is layouted twice. We
    // test here that queued up events are executed between layout operations.
    // That ensures that no such duplicate layouts will happen.

    // note that for this test we need the two layout calls to happen in a
    // defined order. The order is only defined if one is on a deeper level, so
    // we create a child view inside the (unused) view3.

    std::shared_ptr<Button> view4 = std::make_shared<Button>();

    view3->setContentView(view4);

    // wait until the changes scheduled by adding the content view have all been
    // handled.
    CONTINUE_SECTION_WHEN_IDLE(view1, view2, view3, view4, coord)
    {
        int initialLayoutCount1 =
            std::dynamic_pointer_cast<bdn::test::MockViewCore>(view1->getViewCore())->getLayoutCount();
        int initialLayoutCount4 =
            std::dynamic_pointer_cast<bdn::test::MockViewCore>(view4->getViewCore())->getLayoutCount();

        view1->needLayout(View::InvalidateReason::customDataChanged);
        view4->needLayout(View::InvalidateReason::customDataChanged);

        // layout order is parent-first, so view1 will always be layouted before
        // view4

        std::dynamic_pointer_cast<bdn::test::MockViewCore>(view1->getViewCore())
            ->setOverrideLayoutFunc([view1, view2, view3, view4, coord, initialLayoutCount1, initialLayoutCount4]() {
                // view 4 should not have been layouted yet
                REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(view4->getViewCore())->getLayoutCount() ==
                        initialLayoutCount4);

                // and view 1 only once (the current call)
                REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(view1->getViewCore())->getLayoutCount() ==
                        initialLayoutCount1 + 1);

                // now schedule something to the event queue.
                // This simulates the property change notification.
                asyncCallFromMainThread([view1, view2, view3, view4, coord, initialLayoutCount1,
                                         initialLayoutCount4]() {
                    // view4 should still not have been layouted yet
                    REQUIRE(
                        std::dynamic_pointer_cast<bdn::test::MockViewCore>(view4->getViewCore())->getLayoutCount() ==
                        initialLayoutCount4);

                    // and view1 still only once
                    REQUIRE(
                        std::dynamic_pointer_cast<bdn::test::MockViewCore>(view1->getViewCore())->getLayoutCount() ==
                        initialLayoutCount1 + 1);
                });

                // execute the normal layout implementation
                return false;
            });

        CONTINUE_SECTION_WHEN_IDLE(view1, view2, view3, view4, initialLayoutCount1, initialLayoutCount4, coord)
        {
            // verify that now all layout requests have been handled.
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(view1->getViewCore())->getLayoutCount() ==
                    initialLayoutCount1 + 1);
            REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(view4->getViewCore())->getLayoutCount() ==
                    initialLayoutCount4 + 1);
        };
    };
}
}
;
}
