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
        P<bdn::test::ViewTestPreparer<Switch>> preparer = newObj<bdn::test::ViewTestPreparer<Switch>>();
        P<bdn::test::ViewWithTestExtensions<Switch>> switchControl = preparer->createView();
        P<bdn::test::MockSwitchCore> core = cast<bdn::test::MockSwitchCore>(switchControl->getViewCore());

        REQUIRE(core != nullptr);

        SECTION("initialSwitchState")
        {
            SECTION("label")
            {
                REQUIRE(switchControl->label() == "");
                REQUIRE(core->getLabel() == "");
                REQUIRE(core->getLabelChangeCount() == 0);
            }

            SECTION("on")
            {
                REQUIRE(switchControl->on() == false);
                REQUIRE(core->getOn() == false);
                REQUIRE(core->getOnChangeCount() == 0);
            }
        }

        SECTION("changeSwitchProperty")
        {
            SECTION("label")
            {
                bdn::test::_testViewOp(
                    switchControl, preparer, [switchControl]() { switchControl->setLabel("hello"); },
                    [core, switchControl] {
                        REQUIRE(core->getLabel() == "hello");
                        REQUIRE(core->getLabelChangeCount() == 1);
                    },
                    (int)bdn::test::ExpectedSideEffect_::invalidateSizingInfo         // should have caused sizing
                                                                                      // info to be invalidated
                        | (int)bdn::test::ExpectedSideEffect_::invalidateParentLayout // should cause a parent
                                                                                      // layout update since sizing
                                                                                      // info was invalidated
                );
            }

            SECTION("on")
            {
                bdn::test::_testViewOp(switchControl, preparer, [switchControl]() { switchControl->setOn(true); },
                                       [core, switchControl] {
                                           REQUIRE(core->getOn() == true);
                                           REQUIRE(core->getOnChangeCount() == 1);
                                       },
                                       0);
            }
        }
    }
}
