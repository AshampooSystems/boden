#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockSwitchCore.h>

using namespace bdn;

TEST_CASE("Switch")
{
    // test the generic view properties of Switch
    SECTION("View-base")
    bdn::test::testView<Switch>();

    SECTION("Switch-specific")
    {
        P<bdn::test::ViewTestPreparer<Switch>> pPreparer =
            newObj<bdn::test::ViewTestPreparer<Switch>>();
        P<bdn::test::ViewWithTestExtensions<Switch>> pSwitch =
            pPreparer->createView();
        P<bdn::test::MockSwitchCore> pCore =
            cast<bdn::test::MockSwitchCore>(pSwitch->getViewCore());

        REQUIRE(pCore != nullptr);

        SECTION("initialSwitchState")
        {
            SECTION("label")
            {
                REQUIRE(pSwitch->label() == "");
                REQUIRE(pCore->getLabel() == "");
                REQUIRE(pCore->getLabelChangeCount() == 0);
            }

            SECTION("on")
            {
                REQUIRE(pSwitch->on() == false);
                REQUIRE(pCore->getOn() == false);
                REQUIRE(pCore->getOnChangeCount() == 0);
            }
        }

        SECTION("changeSwitchProperty")
        {
            SECTION("label")
            {
                bdn::test::_testViewOp(
                    pSwitch, pPreparer,
                    [pSwitch]() { pSwitch->setLabel("hello"); },
                    [pCore, pSwitch] {
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
                    pSwitch, pPreparer, [pSwitch]() { pSwitch->setOn(true); },
                    [pCore, pSwitch] {
                        REQUIRE(pCore->getOn() == true);
                        REQUIRE(pCore->getOnChangeCount() == 1);
                    },
                    0);
            }
        }
    }
}
