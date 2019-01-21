
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
    std::shared_ptr<ViewLayout> calcContainerLayout(const Size &containerSize) const override
    {
        // we do not care about this for these tests
        return std::make_shared<ViewLayout>();
    }

    Size calcContainerPreferredSize(const Size &availableSpace = Size::none()) const override
    {
        // we do not care about this for these tests. But we have to do a little
        // bit so that the standard view tests succeed.

        Size size;
        auto pad = padding.get();
        if (pad) {
            Margin p = uiMarginToDipMargin(*pad);
            size += Size(p.left + p.right, p.top + p.bottom);
        }

        size.applyMinimum(preferredSizeMinimum);
        size.applyMaximum(preferredSizeMaximum);

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
        std::shared_ptr<bdn::test::ViewTestPreparer<DummyContainerViewForTests>> preparer =
            std::make_shared<bdn::test::ViewTestPreparer<DummyContainerViewForTests>>();
        std::shared_ptr<bdn::test::ViewWithTestExtensions<DummyContainerViewForTests>> containerView =
            preparer->createView();
        std::shared_ptr<bdn::test::MockContainerViewCore> core =
            std::dynamic_pointer_cast<bdn::test::MockContainerViewCore>(containerView->getViewCore());

        REQUIRE(core != nullptr);

        std::shared_ptr<Button> button = std::make_shared<Button>();

        button->adjustAndSetBounds(Rect(10, 10, 10, 10));

        auto otherButton = std::make_shared<Button>();
        auto otherButton2 = std::make_shared<Button>();

        auto nonChild = std::make_shared<Button>();

        SECTION("no child view")
        {
            SECTION("getChildList")
            {
                std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                REQUIRE(childList.empty());
            }

            SECTION("removeAllChildViews")
            {
                containerView->removeAllChildViews();

                std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                REQUIRE(childList.empty());
            }

            SECTION("addChildView")
            {
                CONTINUE_SECTION_WHEN_IDLE(preparer, containerView, button, core)
                {
                    int layoutCountBefore =
                        std::dynamic_pointer_cast<bdn::test::MockViewCore>(containerView->getViewCore())
                            ->getLayoutCount();

                    containerView->addChildView(button);

                    // let scheduled property updates propagate
                    CONTINUE_SECTION_WHEN_IDLE(preparer, containerView, button, core, layoutCountBefore)
                    {
                        // should cause a layout update.
                        REQUIRE(std::dynamic_pointer_cast<bdn::test::MockViewCore>(containerView->getViewCore())
                                    ->getLayoutCount() == layoutCountBefore + 1);
                    };
                };
            }
        }

        SECTION("addChildView")
        {
            SECTION("empty")
            {
                containerView->addChildView(button);

                REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                REQUIRE(childList == std::list<std::shared_ptr<View>>{button});
            }

            SECTION("not empty")
            {
                containerView->addChildView(otherButton);

                containerView->addChildView(button);

                REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, button}));
            }

            SECTION("already in view")
            {
                SECTION("only child")
                {
                    containerView->addChildView(button);

                    containerView->addChildView(button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == std::list<std::shared_ptr<View>>{button});
                }

                SECTION("first")
                {
                    containerView->addChildView(button);
                    containerView->addChildView(otherButton);

                    containerView->addChildView(button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    // should have moved to the end
                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, button}));
                }

                SECTION("last")
                {
                    containerView->addChildView(otherButton);
                    containerView->addChildView(button);

                    containerView->addChildView(button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    // should still be at the end
                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, button}));
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

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == std::list<std::shared_ptr<View>>{button});
                }

                SECTION("insert before non-child")
                {
                    containerView->insertChildView(nonChild, button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == std::list<std::shared_ptr<View>>{button});
                }
            }

            SECTION("one other")
            {
                auto otherButton = std::make_shared<Button>();

                containerView->addChildView(otherButton);

                SECTION("insert at end")
                {
                    containerView->insertChildView(nullptr, button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, button}));
                }

                SECTION("insert at start")
                {
                    containerView->insertChildView(otherButton, button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton}));
                }

                SECTION("insert before non-child")
                {
                    containerView->insertChildView(nonChild, button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, button}));
                }
            }

            SECTION("two others")
            {
                auto otherButton = std::make_shared<Button>();

                containerView->addChildView(otherButton);
                containerView->addChildView(otherButton2);

                SECTION("insert at end")
                {
                    containerView->insertChildView(nullptr, button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, otherButton2, button}));
                }

                SECTION("insert at start")
                {
                    containerView->insertChildView(otherButton, button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton, otherButton2}));
                }

                SECTION("insert in middle start")
                {
                    containerView->insertChildView(otherButton2, button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, button, otherButton2}));
                }

                SECTION("insert before non-child")
                {
                    containerView->insertChildView(nonChild, button);

                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, otherButton2, button}));
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

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{}));
                }
            }

            SECTION("one")
            {
                containerView->addChildView(button);

                SECTION("remove first")
                {
                    containerView->removeChildView(button);

                    REQUIRE(button->getParentView() == nullptr);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{}));
                }

                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button}));
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

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton}));
                }

                SECTION("remove second")
                {
                    containerView->removeChildView(otherButton);

                    REQUIRE(otherButton->getParentView() == nullptr);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button}));
                }

                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton}));
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

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, otherButton2}));
                }

                SECTION("remove second")
                {
                    containerView->removeChildView(otherButton);

                    REQUIRE(otherButton->getParentView() == nullptr);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton2}));
                }

                SECTION("remove third")
                {
                    containerView->removeChildView(otherButton2);

                    REQUIRE(otherButton2->getParentView() == nullptr);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton}));
                }

                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton, otherButton2}));
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

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{}));
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
                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{}));
                }

                SECTION("not a child")
                {
                    containerView->_childViewStolen(nonChild);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button}));
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
                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton}));
                }

                SECTION("stole second")
                {
                    containerView->_childViewStolen(otherButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(otherButton->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button}));
                }

                SECTION("not a child")
                {
                    containerView->_childViewStolen(nonChild);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton}));
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
                    REQUIRE(button->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{otherButton, otherButton2}));
                }

                SECTION("stole second")
                {
                    containerView->_childViewStolen(otherButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(otherButton->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton2}));
                }

                SECTION("stole third")
                {
                    containerView->_childViewStolen(otherButton2);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(otherButton2->getParentView() == std::dynamic_pointer_cast<View>(containerView));

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton}));
                }

                SECTION("not a child")
                {
                    containerView->removeChildView(nonChild);

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

                    REQUIRE(childList == (std::list<std::shared_ptr<View>>{button, otherButton, otherButton2}));
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

            std::list<std::shared_ptr<View>> childList = containerView->getChildViews();

            REQUIRE(childList == (std::list<std::shared_ptr<View>>{}));

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

                SECTION("second")
                {
                    REQUIRE(containerView->findPreviousChildView(otherButton) ==
                            std::dynamic_pointer_cast<View>(button));
                }

                SECTION("not a child") { REQUIRE(containerView->findPreviousChildView(nonChild) == nullptr); }
            }

            SECTION("three")
            {
                containerView->addChildView(button);
                containerView->addChildView(otherButton);
                containerView->addChildView(otherButton2);

                SECTION("first") { REQUIRE(containerView->findPreviousChildView(button) == nullptr); }

                SECTION("second")
                {
                    REQUIRE(containerView->findPreviousChildView(otherButton) ==
                            std::dynamic_pointer_cast<View>(button));
                }

                SECTION("third")
                {
                    REQUIRE(containerView->findPreviousChildView(otherButton2) ==
                            std::dynamic_pointer_cast<View>(otherButton));
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

            std::shared_ptr<LocalTestData_> data = std::make_shared<LocalTestData_>();

            containerView->setDestructFunc(
                [data, button](bdn::test::ViewWithTestExtensions<DummyContainerViewForTests> *containerView) {
                    data->destructorRun = true;
                    data->childParentStillSet = (button->getParentView() != nullptr) ? 1 : 0;

                    std::list<std::shared_ptr<View>> childList = containerView->getChildViews();
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
