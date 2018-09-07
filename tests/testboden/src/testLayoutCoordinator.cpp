#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/Button.h>

#include <bdn/test/MockUiProvider.h>
#include <bdn/test/MockViewCore.h>
#include <bdn/test/MockWindowCore.h>

using namespace bdn;

class LayoutCoordinatorForTesting : public LayoutCoordinator
{
  public:
    int exceptionCount = 0;

  protected:
    void handleException(const std::exception *pExceptionIfAvailable,
                         const String &functionName) override
    {
        // we do not want exceptions to be logged. So we do not call the default
        // implementation.
        exceptionCount++;
    }
};

TEST_CASE("LayoutCoordinator")
{
    P<LayoutCoordinatorForTesting> pCoord =
        newObj<LayoutCoordinatorForTesting>();

    P<bdn::test::MockUiProvider> pUiProvider =
        newObj<bdn::test::MockUiProvider>();

    P<Window> pView1 = newObj<Window>(pUiProvider);
    P<Window> pView2 = newObj<Window>(pUiProvider);
    P<Window> pView3 = newObj<Window>(pUiProvider);

    // wait for the initial updates by the global layout coordinator to be done
    CONTINUE_SECTION_WHEN_IDLE(pUiProvider, pCoord, pView1, pView2,
                               pView3){SECTION("exception"){
        P<Window> pCenterWindow = newObj<Window>(pUiProvider);

    int initialCenterCount =
        cast<bdn::test::MockWindowCore>(pCenterWindow->getViewCore())
            ->getCenterCount();

    // pCenterWindow is always scheduled for centering. This is because window
    // centering is always done last, so this allows us to reasonably detect
    // when an exception causes as abort.
    pCoord->windowNeedsCentering(pCenterWindow);

    SECTION("layout throws exception")
    {
        cast<bdn::test::MockViewCore>(pView2->getViewCore())
            ->setOverrideLayoutFunc([]() {
                throw std::exception();
                return false;
            });

        int initialCount1 = cast<bdn::test::MockViewCore>(pView1->getViewCore())
                                ->getLayoutCount();
        int initialCount2 = cast<bdn::test::MockViewCore>(pView2->getViewCore())
                                ->getLayoutCount();
        int initialCount3 = cast<bdn::test::MockViewCore>(pView3->getViewCore())
                                ->getLayoutCount();

        pCoord->viewNeedsLayout(pView1);
        pCoord->viewNeedsLayout(pView2);
        pCoord->viewNeedsLayout(pView3);

        CONTINUE_SECTION_WHEN_IDLE(pCoord, pView1, pView2, pView3,
                                   initialCount1, initialCount2, initialCount3,
                                   pCenterWindow, initialCenterCount)
        {
            REQUIRE(cast<bdn::test::MockViewCore>(pView1->getViewCore())
                        ->getLayoutCount() == initialCount1 + 1);
            REQUIRE(cast<bdn::test::MockViewCore>(pView2->getViewCore())
                        ->getLayoutCount() == initialCount2 + 1);
            REQUIRE(cast<bdn::test::MockViewCore>(pView2->getViewCore())
                        ->getLayoutCount() == initialCount3 + 1);

            REQUIRE(
                cast<bdn::test::MockWindowCore>(pCenterWindow->getViewCore())
                    ->getCenterCount() == initialCenterCount + 1);

            REQUIRE(pCoord->exceptionCount == 1);
        };
    }

    SECTION("autoSize throws exception")
    {
        int initialCount1 =
            cast<bdn::test::MockWindowCore>(pView1->getViewCore())
                ->getAutoSizeCount();
        int initialCount2 =
            cast<bdn::test::MockWindowCore>(pView2->getViewCore())
                ->getAutoSizeCount();
        int initialCount3 =
            cast<bdn::test::MockWindowCore>(pView3->getViewCore())
                ->getAutoSizeCount();

        int initialCenterWindowCenterCount =
            cast<bdn::test::MockWindowCore>(pCenterWindow->getViewCore())
                ->getCenterCount();

        cast<bdn::test::MockWindowCore>(pView2->getViewCore())
            ->setOverrideAutoSizeFunc([]() {
                throw std::exception();
                return false;
            });

        pCoord->windowNeedsAutoSizing(pView1);
        pCoord->windowNeedsAutoSizing(pView2);
        pCoord->windowNeedsAutoSizing(pView3);

        CONTINUE_SECTION_WHEN_IDLE(
            pCoord, pView1, pView2, pView3, pCenterWindow, initialCount1,
            initialCount2, initialCount3, initialCenterWindowCenterCount)
        {
            REQUIRE(cast<bdn::test::MockWindowCore>(pView1->getViewCore())
                        ->getAutoSizeCount() == initialCount1 + 1);
            REQUIRE(cast<bdn::test::MockWindowCore>(pView2->getViewCore())
                        ->getAutoSizeCount() == initialCount2 + 1);
            REQUIRE(cast<bdn::test::MockWindowCore>(pView3->getViewCore())
                        ->getAutoSizeCount() == initialCount3 + 1);
            REQUIRE(
                cast<bdn::test::MockWindowCore>(pCenterWindow->getViewCore())
                    ->getCenterCount() == initialCenterWindowCenterCount + 1);

            REQUIRE(pCoord->exceptionCount == 1);
        };
    }

    SECTION("center throws exception")
    {
        int initialCount1 =
            cast<bdn::test::MockWindowCore>(pView1->getViewCore())
                ->getCenterCount();
        int initialCount2 =
            cast<bdn::test::MockWindowCore>(pView2->getViewCore())
                ->getCenterCount();
        int initialCount3 =
            cast<bdn::test::MockWindowCore>(pView3->getViewCore())
                ->getCenterCount();

        int initialCenterWindowCenterCount =
            cast<bdn::test::MockWindowCore>(pCenterWindow->getViewCore())
                ->getCenterCount();

        cast<bdn::test::MockWindowCore>(pView2->getViewCore())
            ->setOverrideCenterFunc([]() {
                throw std::exception();
                return false;
            });

        pCoord->windowNeedsCentering(pView1);
        pCoord->windowNeedsCentering(pView2);
        pCoord->windowNeedsCentering(pView3);

        CONTINUE_SECTION_WHEN_IDLE(
            pCoord, pView1, pView2, pView3, pCenterWindow, initialCount1,
            initialCount2, initialCount3, initialCenterWindowCenterCount)
        {
            REQUIRE(cast<bdn::test::MockWindowCore>(pView1->getViewCore())
                        ->getCenterCount() == initialCount1 + 1);
            REQUIRE(cast<bdn::test::MockWindowCore>(pView2->getViewCore())
                        ->getCenterCount() == initialCount2 + 1);
            REQUIRE(cast<bdn::test::MockWindowCore>(pView3->getViewCore())
                        ->getCenterCount() == initialCount3 + 1);
            REQUIRE(
                cast<bdn::test::MockWindowCore>(pCenterWindow->getViewCore())
                    ->getCenterCount() == initialCenterWindowCenterCount + 1);

            REQUIRE(pCoord->exceptionCount == 1);
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

    P<Button> pView4 = newObj<Button>();

    pView3->setContentView(pView4);

    // wait until the changes scheduled by adding the content view have all been
    // handled.
    CONTINUE_SECTION_WHEN_IDLE(pView1, pView2, pView3, pView4, pCoord)
    {
        int initialLayoutCount1 =
            cast<bdn::test::MockViewCore>(pView1->getViewCore())
                ->getLayoutCount();
        int initialLayoutCount4 =
            cast<bdn::test::MockViewCore>(pView4->getViewCore())
                ->getLayoutCount();

        pView1->needLayout(View::InvalidateReason::customDataChanged);
        pView4->needLayout(View::InvalidateReason::customDataChanged);

        // layout order is parent-first, so view1 will always be layouted before
        // view4

        cast<bdn::test::MockViewCore>(pView1->getViewCore())
            ->setOverrideLayoutFunc([pView1, pView2, pView3, pView4, pCoord,
                                     initialLayoutCount1,
                                     initialLayoutCount4]() {
                // view 4 should not have been layouted yet
                REQUIRE(cast<bdn::test::MockViewCore>(pView4->getViewCore())
                            ->getLayoutCount() == initialLayoutCount4);

                // and view 1 only once (the current call)
                REQUIRE(cast<bdn::test::MockViewCore>(pView1->getViewCore())
                            ->getLayoutCount() == initialLayoutCount1 + 1);

                // now schedule something to the event queue.
                // This simulates the property change notification.
                asyncCallFromMainThread([pView1, pView2, pView3, pView4, pCoord,
                                         initialLayoutCount1,
                                         initialLayoutCount4]() {
                    // view4 should still not have been layouted yet
                    REQUIRE(cast<bdn::test::MockViewCore>(pView4->getViewCore())
                                ->getLayoutCount() == initialLayoutCount4);

                    // and view1 still only once
                    REQUIRE(cast<bdn::test::MockViewCore>(pView1->getViewCore())
                                ->getLayoutCount() == initialLayoutCount1 + 1);
                });

                // execute the normal layout implementation
                return false;
            });

        CONTINUE_SECTION_WHEN_IDLE(pView1, pView2, pView3, pView4,
                                   initialLayoutCount1, initialLayoutCount4,
                                   pCoord)
        {
            // verify that now all layout requests have been handled.
            REQUIRE(cast<bdn::test::MockViewCore>(pView1->getViewCore())
                        ->getLayoutCount() == initialLayoutCount1 + 1);
            REQUIRE(cast<bdn::test::MockViewCore>(pView4->getViewCore())
                        ->getLayoutCount() == initialLayoutCount4 + 1);
        };
    };
}
}
;
}
