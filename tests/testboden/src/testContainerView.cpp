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

    Size calcContainerPreferredSize(
        const Size &availableSpace = Size::none()) const override
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
        P<bdn::test::ViewTestPreparer<DummyContainerViewForTests>> pPreparer =
            newObj<bdn::test::ViewTestPreparer<DummyContainerViewForTests>>();
        P<bdn::test::ViewWithTestExtensions<DummyContainerViewForTests>>
            pContainerView = pPreparer->createView();
        P<bdn::test::MockContainerViewCore> pCore =
            cast<bdn::test::MockContainerViewCore>(
                pContainerView->getViewCore());

        REQUIRE(pCore != nullptr);

        P<Button> pButton = newObj<Button>();

        pButton->adjustAndSetBounds(Rect(10, 10, 10, 10));

        auto pOtherButton = newObj<Button>();
        auto pOtherButton2 = newObj<Button>();

        auto pNonChild = newObj<Button>();

        SECTION("no child view")
        {
            SECTION("getChildList")
            {
                List<P<View>> childList;
                pContainerView->getChildViews(childList);

                REQUIRE(childList.empty());
            }

            SECTION("removeAllChildViews")
            {
                pContainerView->removeAllChildViews();

                List<P<View>> childList;
                pContainerView->getChildViews(childList);

                REQUIRE(childList.empty());
            }

            SECTION("addChildView")
            {
                CONTINUE_SECTION_WHEN_IDLE(pPreparer, pContainerView, pButton,
                                           pCore)
                {
                    int layoutCountBefore = cast<bdn::test::MockViewCore>(
                                                pContainerView->getViewCore())
                                                ->getLayoutCount();

                    pContainerView->addChildView(pButton);

                    // let scheduled property updates propagate
                    CONTINUE_SECTION_WHEN_IDLE(pPreparer, pContainerView,
                                               pButton, pCore,
                                               layoutCountBefore)
                    {
                        // should cause a layout update.
                        REQUIRE(cast<bdn::test::MockViewCore>(
                                    pContainerView->getViewCore())
                                    ->getLayoutCount() ==
                                layoutCountBefore + 1);
                    };
                };
            }
        }

        SECTION("addChildView")
        {
            SECTION("empty")
            {
                pContainerView->addChildView(pButton);

                REQUIRE(pButton->getParentView() == cast<View>(pContainerView));

                List<P<View>> childList;
                pContainerView->getChildViews(childList);

                REQUIRE(childList == List<P<View>>{pButton});
            }

            SECTION("not empty")
            {
                pContainerView->addChildView(pOtherButton);

                pContainerView->addChildView(pButton);

                REQUIRE(pButton->getParentView() == cast<View>(pContainerView));

                List<P<View>> childList;
                pContainerView->getChildViews(childList);

                REQUIRE(childList == (List<P<View>>{pOtherButton, pButton}));
            }

            SECTION("already in view")
            {
                SECTION("only child")
                {
                    pContainerView->addChildView(pButton);

                    pContainerView->addChildView(pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == List<P<View>>{pButton});
                }

                SECTION("first")
                {
                    pContainerView->addChildView(pButton);
                    pContainerView->addChildView(pOtherButton);

                    pContainerView->addChildView(pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    // should have moved to the end
                    REQUIRE(childList ==
                            (List<P<View>>{pOtherButton, pButton}));
                }

                SECTION("last")
                {
                    pContainerView->addChildView(pOtherButton);
                    pContainerView->addChildView(pButton);

                    pContainerView->addChildView(pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    // should still be at the end
                    REQUIRE(childList ==
                            (List<P<View>>{pOtherButton, pButton}));
                }
            }
        }

        SECTION("insertChildView")
        {
            SECTION("empty")
            {
                SECTION("insert at end")
                {
                    pContainerView->insertChildView(nullptr, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == List<P<View>>{pButton});
                }

                SECTION("insert before non-child")
                {
                    pContainerView->insertChildView(pNonChild, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == List<P<View>>{pButton});
                }
            }

            SECTION("one other")
            {
                auto pOtherButton = newObj<Button>();

                pContainerView->addChildView(pOtherButton);

                SECTION("insert at end")
                {
                    pContainerView->insertChildView(nullptr, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pOtherButton, pButton}));
                }

                SECTION("insert at start")
                {
                    pContainerView->insertChildView(pOtherButton, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pButton, pOtherButton}));
                }

                SECTION("insert before non-child")
                {
                    pContainerView->insertChildView(pNonChild, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pOtherButton, pButton}));
                }
            }

            SECTION("two others")
            {
                auto pOtherButton = newObj<Button>();

                pContainerView->addChildView(pOtherButton);
                pContainerView->addChildView(pOtherButton2);

                SECTION("insert at end")
                {
                    pContainerView->insertChildView(nullptr, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(
                        childList ==
                        (List<P<View>>{pOtherButton, pOtherButton2, pButton}));
                }

                SECTION("insert at start")
                {
                    pContainerView->insertChildView(pOtherButton, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pButton, pOtherButton,
                                                        pOtherButton2}));
                }

                SECTION("insert in middle start")
                {
                    pContainerView->insertChildView(pOtherButton2, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pOtherButton, pButton,
                                                        pOtherButton2}));
                }

                SECTION("insert before non-child")
                {
                    pContainerView->insertChildView(pNonChild, pButton);

                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(
                        childList ==
                        (List<P<View>>{pOtherButton, pOtherButton2, pButton}));
                }
            }
        }

        SECTION("removeChildView")
        {
            SECTION("empty")
            {
                SECTION("not a child")
                {
                    pContainerView->removeChildView(pNonChild);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{}));
                }
            }

            SECTION("one")
            {
                pContainerView->addChildView(pButton);

                SECTION("remove first")
                {
                    pContainerView->removeChildView(pButton);

                    REQUIRE(pButton->getParentView() == nullptr);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{}));
                }

                SECTION("not a child")
                {
                    pContainerView->removeChildView(pNonChild);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pButton}));
                }
            }

            SECTION("two")
            {
                pContainerView->addChildView(pButton);
                pContainerView->addChildView(pOtherButton);

                SECTION("remove first")
                {
                    pContainerView->removeChildView(pButton);

                    REQUIRE(pButton->getParentView() == nullptr);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pOtherButton}));
                }

                SECTION("remove second")
                {
                    pContainerView->removeChildView(pOtherButton);

                    REQUIRE(pOtherButton->getParentView() == nullptr);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pButton}));
                }

                SECTION("not a child")
                {
                    pContainerView->removeChildView(pNonChild);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pButton, pOtherButton}));
                }
            }

            SECTION("three")
            {
                pContainerView->addChildView(pButton);
                pContainerView->addChildView(pOtherButton);
                pContainerView->addChildView(pOtherButton2);

                SECTION("remove first")
                {
                    pContainerView->removeChildView(pButton);

                    REQUIRE(pButton->getParentView() == nullptr);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pOtherButton, pOtherButton2}));
                }

                SECTION("remove second")
                {
                    pContainerView->removeChildView(pOtherButton);

                    REQUIRE(pOtherButton->getParentView() == nullptr);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pButton, pOtherButton2}));
                }

                SECTION("remove third")
                {
                    pContainerView->removeChildView(pOtherButton2);

                    REQUIRE(pOtherButton2->getParentView() == nullptr);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pButton, pOtherButton}));
                }

                SECTION("not a child")
                {
                    pContainerView->removeChildView(pNonChild);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pButton, pOtherButton,
                                                        pOtherButton2}));
                }
            }
        }

        SECTION("_childViewStolen")
        {
            SECTION("empty")
            {
                SECTION("not a child")
                {
                    pContainerView->_childViewStolen(pNonChild);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{}));
                }
            }

            SECTION("one")
            {
                pContainerView->addChildView(pButton);

                SECTION("stole first")
                {
                    pContainerView->_childViewStolen(pButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{}));
                }

                SECTION("not a child")
                {
                    pContainerView->_childViewStolen(pNonChild);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pButton}));
                }
            }

            SECTION("two")
            {
                pContainerView->addChildView(pButton);
                pContainerView->addChildView(pOtherButton);

                SECTION("stole first")
                {
                    pContainerView->_childViewStolen(pButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pOtherButton}));
                }

                SECTION("stole second")
                {
                    pContainerView->_childViewStolen(pOtherButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(pOtherButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pButton}));
                }

                SECTION("not a child")
                {
                    pContainerView->_childViewStolen(pNonChild);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pButton, pOtherButton}));
                }
            }

            SECTION("three")
            {
                pContainerView->addChildView(pButton);
                pContainerView->addChildView(pOtherButton);
                pContainerView->addChildView(pOtherButton2);

                SECTION("stole first")
                {
                    pContainerView->_childViewStolen(pButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(pButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pOtherButton, pOtherButton2}));
                }

                SECTION("stole second")
                {
                    pContainerView->_childViewStolen(pOtherButton);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(pOtherButton->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pButton, pOtherButton2}));
                }

                SECTION("stole third")
                {
                    pContainerView->_childViewStolen(pOtherButton2);

                    // stolen view should not be accessed, so parent should
                    // still be set there
                    REQUIRE(pOtherButton2->getParentView() ==
                            cast<View>(pContainerView));

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList ==
                            (List<P<View>>{pButton, pOtherButton}));
                }

                SECTION("not a child")
                {
                    pContainerView->removeChildView(pNonChild);

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);

                    REQUIRE(childList == (List<P<View>>{pButton, pOtherButton,
                                                        pOtherButton2}));
                }
            }
        }

        SECTION("removeAllChildViews")
        {
            SECTION("empty") {}

            SECTION("one") { pContainerView->addChildView(pButton); }

            SECTION("two")
            {
                pContainerView->addChildView(pButton);
                pContainerView->addChildView(pOtherButton);
            }

            pContainerView->removeAllChildViews();

            List<P<View>> childList;
            pContainerView->getChildViews(childList);

            REQUIRE(childList == (List<P<View>>{}));

            REQUIRE(pButton->getParentView() == nullptr);
            REQUIRE(pOtherButton->getParentView() == nullptr);
        }

        SECTION("findPreviousChildView")
        {
            SECTION("empty")
            {
                SECTION("not a child")
                {
                    REQUIRE(pContainerView->findPreviousChildView(pNonChild) ==
                            nullptr);
                }
            }

            SECTION("one")
            {
                pContainerView->addChildView(pButton);

                SECTION("first")
                {
                    REQUIRE(pContainerView->findPreviousChildView(pButton) ==
                            nullptr);
                }

                SECTION("not a child")
                {
                    REQUIRE(pContainerView->findPreviousChildView(pNonChild) ==
                            nullptr);
                }
            }

            SECTION("two")
            {
                pContainerView->addChildView(pButton);
                pContainerView->addChildView(pOtherButton);

                SECTION("first")
                {
                    REQUIRE(pContainerView->findPreviousChildView(pButton) ==
                            nullptr);
                }

                SECTION("second")
                {
                    REQUIRE(pContainerView->findPreviousChildView(
                                pOtherButton) == cast<View>(pButton));
                }

                SECTION("not a child")
                {
                    REQUIRE(pContainerView->findPreviousChildView(pNonChild) ==
                            nullptr);
                }
            }

            SECTION("three")
            {
                pContainerView->addChildView(pButton);
                pContainerView->addChildView(pOtherButton);
                pContainerView->addChildView(pOtherButton2);

                SECTION("first")
                {
                    REQUIRE(pContainerView->findPreviousChildView(pButton) ==
                            nullptr);
                }

                SECTION("second")
                {
                    REQUIRE(pContainerView->findPreviousChildView(
                                pOtherButton) == cast<View>(pButton));
                }

                SECTION("third")
                {
                    REQUIRE(pContainerView->findPreviousChildView(
                                pOtherButton2) == cast<View>(pOtherButton));
                }

                SECTION("not a child")
                {
                    REQUIRE(pContainerView->findPreviousChildView(pNonChild) ==
                            nullptr);
                }
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

            P<LocalTestData_> pData = newObj<LocalTestData_>();

            pContainerView->setDestructFunc(
                [pData, pButton](bdn::test::ViewWithTestExtensions<
                                 DummyContainerViewForTests> *pContainerView) {
                    pData->destructorRun = true;
                    pData->childParentStillSet =
                        (pButton->getParentView() != nullptr) ? 1 : 0;

                    List<P<View>> childList;
                    pContainerView->getChildViews(childList);
                    pData->childListEmpty = (childList.empty() ? 1 : 0);
                });

            BDN_CONTINUE_SECTION_WHEN_IDLE(pData, pButton)
            {
                // All test objects should have been destroyed by now.
                // First verify that the destructor was even called.
                REQUIRE(pData->destructorRun);

                // now verify what we actually want to test: that the
                // content view's parent was set to null before the destructor
                // of the parent was called.
                REQUIRE(pData->childParentStillSet == 0);

                // the child should also not be a child of the parent
                // from the parent's perspective anymore.
                REQUIRE(pData->childListEmpty == 1);
            };
        }
    }
}
