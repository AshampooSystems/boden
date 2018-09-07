#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Toggle.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockToggleCore.h>

using namespace bdn;

TEST_CASE("Toggle")
{
    // test the generic view properties of Toggle
    SECTION("View-base")
    bdn::test::testView<Toggle>();

    SECTION("Toggle-specific")
    {
        P<bdn::test::ViewTestPreparer<Toggle>> pPreparer =
            newObj<bdn::test::ViewTestPreparer<Toggle>>();
        P<bdn::test::ViewWithTestExtensions<Toggle>> pToggle =
            pPreparer->createView();
        P<bdn::test::MockToggleCore> pCore =
            cast<bdn::test::MockToggleCore>(pToggle->getViewCore());

        REQUIRE(pCore != nullptr);

        SECTION("initialToggleState")
        {
            SECTION("label")
            {
                REQUIRE(pToggle->label() == "");
                REQUIRE(pCore->getLabel() == "");
                REQUIRE(pCore->getLabelChangeCount() == 0);
            }

            SECTION("on")
            {
                REQUIRE(pToggle->on() == false);
                REQUIRE(pCore->getOn() == false);
                REQUIRE(pCore->getOnChangeCount() == 0);
            }
        }

        SECTION("changeToggleProperty")
        {
            SECTION("label")
            {
                bdn::test::_testViewOp(
                    pToggle, pPreparer,
                    [pToggle]() { pToggle->setLabel("hello"); },
                    [pCore, pToggle] {
                        REQUIRE(pCore->getLabel() == "hello");
                        REQUIRE(pCore->getLabelChangeCount() == 1);
                    },
                    (int)bdn::test::ExpectedSideEffect_::
                            invalidateSizingInfo // should have caused sizing
                                                 // info to be invalidated
                        |
                        (int)bdn::test::ExpectedSideEffect_::
                            invalidateParentLayout // should cause a parent
                                                   // layout update since sizing
                                                   // info was invalidated
                );
            }

            SECTION("on")
            {
                bdn::test::_testViewOp(
                    pToggle, pPreparer, [pToggle]() { pToggle->setOn(true); },
                    [pCore, pToggle] {
                        REQUIRE(pCore->getOn() == true);
                        REQUIRE(pCore->getOnChangeCount() == 1);
                    },
                    0);
            }
        }
    }
}
