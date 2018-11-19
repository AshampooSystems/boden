#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ContainerView.h>
#include <bdn/Button.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockViewCore.h>
#include <bdn/test/MockContainerViewCore.h>

using namespace bdn;

class DummyContainerViewForTests : public ContainerView
{
  public:
    P<ViewLayout> calcContainerLayout(const Size &containerSize) const override
    {
        // we do not care about this for these tests
        return newObj<ViewLayout>();
    }

    Size calcContainerPreferredSize(const Size &availableSpace = Size::none()) const override
    {
        // we do not care about this for these tests. But we have to do a little
        // bit so that the standard view tests succeed.

        Size size;
        auto pad = padding();
        if (!pad.isNull()) {
            Margin p = uiMarginToDipMargin(pad.get());
            size += Size(p.left + p.right, p.top + p.bottom);
        }

        size.applyMinimum(preferredSizeMinimum());
        size.applyMaximum(preferredSizeMaximum());

        return size;
    }
};

TEST_CASE("ContainerView")
{
    // test the generic view properties of Button
    SECTION("View-base")
    bdn::test::testView<DummyContainerViewForTests>();

    SECTION("ContainerView-specific")
    {
        P<bdn::test::ViewTestPreparer<DummyContainerViewForTests>> preparer =
            newObj<bdn::test::ViewTestPreparer<DummyContainerViewForTests>>();
        P<bdn::test::ViewWithTestExtensions<DummyContainerViewForTests>> containerView = preparer->createView();
        P<bdn::test::MockContainerViewCore> core = cast<bdn::test::MockContainerViewCore>(containerView->getViewCore());

        REQUIRE(core != nullptr);

        P<Button> button = newObj<Button>();

        button->adjustAndSetBounds(Rect(10, 10, 10, 10));

        auto otherButton = newObj<Button>();
        auto otherButton2 = newObj<Button>();

        auto nonChild = newObj<Button>();

        SECTION("no child view")
        {
            SECTION("getChildList")
            {
                List<P<View>> childList;
                containerView->getChildViews(childList);

                REQUIRE(childList.empty());
            }

            SECTION("removeAllChildViews")
            {
                containerView->removeAllChildViews();

                List<P<View>> childList;
                containerView->getChildViews(childList);

                REQUIRE(childList.empty());
            }

            SECTION("addChildView")
            {
                CONTINUE_SECTION_WHEN_IDLE(preparer, containerView, button, core)
                {
                    int layoutCountBefore =
                        cast<bdn::test::MockViewCore>(containerView->getViewCore())->getLayoutCount();

                    containerView->addChildView(button);

                    // let scheduled property updates propagate
                    CONTINUE_SECTION_WHEN_IDLE(preparer, containerView, button, core, layoutCountBefore)
                    {
                        // should cause a layout update.
                        REQUIRE(cast<bdn::test::MockViewCore>(containerView->getViewCore())->getLayoutCount() ==
                                layoutCountBefore + 1);
                    };
                };
            }
        }

        SECTION("addChildView")
        {
            SECTION("empty")
            {
                containerView->addChildView(button);

                REQUIRE(button->getParentView() == cast<View>(containerView));

                List<P<View>> childList;
                containerView->getChildViews(childList);

                REQUIRE(childList == List<P<View>>{button});
            }

            SECTION("not empty")
            {
                containerView->addChildView(otherButton);

                containerView->addChildView(button);

                REQUIRE(button->getParentView() == cast<View>(containerView));

                List<P<View>> childList;
                containerView->getChildViews(childList);

                REQUIRE(childList == (List<P<View>>{otherButton, button}));
            }

            SECTION("already in view")
            {
                SECTION("only child")
                {
                    containerView->addChildView(button);

                    containerView->addChildView(button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == List<P<View>>{button});
                }

                SECTION("first")
                {
                    containerView->addChildView(button);
                    containerView->addChildView(otherButton);

                    containerView->addChildView(button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    // should have moved to the end
                    REQUIRE(childList == (List<P<View>>{otherButton, button}));
                }

                SECTION("last")
                {
                    containerView->addChildView(otherButton);
                    containerView->addChildView(button);

                    containerView->addChildView(button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    // should still be at the end
                    REQUIRE(childList == (List<P<View>>{otherButton, button}));
                }
            }
        }

        SECTION("insertChildView")
        {
            SECTION("empty")
            {
                SECTION("insert at end")
                {
                    containerView->insertChildView(nullptr, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == List<P<View>>{button});
                }

                SECTION("insert before non-child")
                {
                    containerView->insertChildView(nonChild, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == List<P<View>>{button});
                }
            }

            SECTION("one other")
            {
                auto otherButton = newObj<Button>();

                containerView->addChildView(otherButton);

                SECTION("insert at end")
                {
                    containerView->insertChildView(nullptr, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton, button}));
                }

                SECTION("insert at start")
                {
                    containerView->insertChildView(otherButton, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton}));
                }

                SECTION("insert before non-child")
                {
                    containerView->insertChildView(nonChild, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton, button}));
                }
            }

            SECTION("two others")
            {
                auto otherButton = newObj<Button>();

                containerView->addChildView(otherButton);
                containerView->addChildView(otherButton2);

                SECTION("insert at end")
                {
                    containerView->insertChildView(nullptr, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton, otherButton2, button}));
                }

                SECTION("insert at start")
                {
                    containerView->insertChildView(otherButton, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton, otherButton2}));
                }

                SECTION("insert in middle start")
                {
                    containerView->insertChildView(otherButton2, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton, button, otherButton2}));
                }

                SECTION("insert before non-child")
                {
                    containerView->insertChildView(nonChild, button);

                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton, otherButton2, button}));
                }
            }
        }

        SECTION("removeChildView")
        {
            SECTION("empty")
            {
                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{}));
                }
            }

            SECTION("one")
            {
                containerView->addChildView(button);

                SECTION("remove first")
                {
                    containerView->removeChildView(button);

                    REQUIRE(button->getParentView() == nullptr);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{}));
                }

                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button}));
                }
            }

            SECTION("two")
            {
                containerView->addChildView(button);
                containerView->addChildView(otherButton);

                SECTION("remove first")
                {
                    containerView->removeChildView(button);

                    REQUIRE(button->getParentView() == nullptr);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton}));
                }

                SECTION("remove second")
                {
                    containerView->removeChildView(otherButton);

                    REQUIRE(otherButton->getParentView() == nullptr);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button}));
                }

                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton}));
                }
            }

            SECTION("three")
            {
                containerView->addChildView(button);
                containerView->addChildView(otherButton);
                containerView->addChildView(otherButton2);

                SECTION("remove first")
                {
                    containerView->removeChildView(button);

                    REQUIRE(button->getParentView() == nullptr);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton, otherButton2}));
                }

                SECTION("remove second")
                {
                    containerView->removeChildView(otherButton);

                    REQUIRE(otherButton->getParentView() == nullptr);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton2}));
                }

                SECTION("remove third")
                {
                    containerView->removeChildView(otherButton2);

                    REQUIRE(otherButton2->getParentView() == nullptr);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton}));
                }

                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton, otherButton2}));
                }
            }
        }

        SECTION("_childViewStolen")
        {
            SECTION("empty")
            {
                SECTION("not a child")
                {
                    containerView->_childViewStolen(nonChild);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{}));
                }
            }

            SECTION("one")
            {
                containerView->addChildView(button);

                SECTION("stole first")
                {
                    containerView->_childViewStolen(button);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{}));
                }

                SECTION("not a child")
                {
                    containerView->_childViewStolen(nonChild);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button}));
                }
            }

            SECTION("two")
            {
                containerView->addChildView(button);
                containerView->addChildView(otherButton);

                SECTION("stole first")
                {
                    containerView->_childViewStolen(button);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton}));
                }

                SECTION("stole second")
                {
                    containerView->_childViewStolen(otherButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(otherButton->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button}));
                }

                SECTION("not a child")
                {
                    containerView->_childViewStolen(nonChild);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton}));
                }
            }

            SECTION("three")
            {
                containerView->addChildView(button);
                containerView->addChildView(otherButton);
                containerView->addChildView(otherButton2);

                SECTION("stole first")
                {
                    containerView->_childViewStolen(button);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(button->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{otherButton, otherButton2}));
                }

                SECTION("stole second")
                {
                    containerView->_childViewStolen(otherButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(otherButton->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton2}));
                }

                SECTION("stole third")
                {
                    containerView->_childViewStolen(otherButton2);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(otherButton2->getParentView() == cast<View>(containerView));

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton}));
                }

                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    List<P<View>> childList;
                    containerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{button, otherButton, otherButton2}));
                }
            }
        }

        SECTION("removeAllChildViews")
        {
            SECTION("empty") {}

            SECTION("one") { containerView->addChildView(button); }

            SECTION("two")
            {
                containerView->addChildView(button);
                containerView->addChildView(otherButton);
            }

            containerView->removeAllChildViews();

            List<P<View>> childList;
            containerView->getChildViews(childList);

            REQUIRE(childList == (List<P<View>>{}));

            REQUIRE(button->getParentView() == nullptr);
            REQUIRE(otherButton->getParentView() == nullptr);
        }

        SECTION("findPreviousChildView")
        {
            SECTION("empty")
            {
                SECTION("not a child") { REQUIRE(containerView->findPreviousChildView(nonChild) == nullptr); }
            }

            SECTION("one")
            {
                containerView->addChildView(button);

                SECTION("first") { REQUIRE(containerView->findPreviousChildView(button) == nullptr); }

                SECTION("not a child") { REQUIRE(containerView->findPreviousChildView(nonChild) == nullptr); }
            }

            SECTION("two")
            {
                containerView->addChildView(button);
                containerView->addChildView(otherButton);

                SECTION("first") { REQUIRE(containerView->findPreviousChildView(button) == nullptr); }

                SECTION("second") { REQUIRE(containerView->findPreviousChildView(otherButton) == cast<View>(button)); }

                SECTION("not a child") { REQUIRE(containerView->findPreviousChildView(nonChild) == nullptr); }
            }

            SECTION("three")
            {
                containerView->addChildView(button);
                containerView->addChildView(otherButton);
                containerView->addChildView(otherButton2);

                SECTION("first") { REQUIRE(containerView->findPreviousChildView(button) == nullptr); }

                SECTION("second") { REQUIRE(containerView->findPreviousChildView(otherButton) == cast<View>(button)); }

                SECTION("third")
                {
                    REQUIRE(containerView->findPreviousChildView(otherButton2) == cast<View>(otherButton));
                }

                SECTION("not a child") { REQUIRE(containerView->findPreviousChildView(nonChild) == nullptr); }
            }
        }

        SECTION("child views detached before destruction begins")
        {
            struct LocalTestData_ : public Base
            {
                bool destructorRun = false;
                int childParentStillSet = -1;
                int childListEmpty = -1;
            };

            P<LocalTestData_> data = newObj<LocalTestData_>();

            containerView->setDestructFunc(
                [data, button](bdn::test::ViewWithTestExtensions<DummyContainerViewForTests> *containerView) {
                    data->destructorRun = true;
                    data->childParentStillSet = (button->getParentView() != nullptr) ? 1 : 0;

                    List<P<View>> childList;
                    containerView->getChildViews(childList);
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

                // the child should also not be a child of the parent
                // from the parent's perspective anymore.
                REQUIRE(data->childListEmpty == 1);
            };
        }
    }
}
