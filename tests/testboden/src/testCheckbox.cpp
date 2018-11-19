#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Checkbox.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockCheckboxCore.h>

using namespace bdn;

TEST_CASE("Checkbox")
{
    // test the generic view properties of Checkbox
    SECTION("View-base")
    bdn::test::testView<Checkbox>();

    SECTION("Checkbox-specific")
    {
        P<bdn::test::ViewTestPreparer<Checkbox>> preparer = newObj<bdn::test::ViewTestPreparer<Checkbox>>();
        P<bdn::test::ViewWithTestExtensions<Checkbox>> checkbox = preparer->createView();
        P<bdn::test::MockCheckboxCore> core = cast<bdn::test::MockCheckboxCore>(checkbox->getViewCore());

        REQUIRE(core != nullptr);

        SECTION("initialCheckboxState")
        {
            SECTION("label")
            {
                REQUIRE(checkbox->label() == "");
                REQUIRE(core->getLabel() == "");
                REQUIRE(core->getLabelChangeCount() == 0);
            }

            SECTION("state")
            {
                REQUIRE(checkbox->state() == TriState::off);
                REQUIRE(core->getState() == TriState::off);
                REQUIRE(core->getStateChangeCount() == 0);
            }
        }

        SECTION("changeCheckboxProperty")
        {
            SECTION("label")
            {
                bdn::test::_testViewOp(
                    checkbox, preparer, [checkbox]() { checkbox->setLabel("hello"); },
                    [core, checkbox] {
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

            SECTION("state")
            {
                bdn::test::_testViewOp(checkbox, preparer, [checkbox]() { checkbox->setState(TriState::on); },
                                       [core, checkbox] {
                                           REQUIRE(core->getState() == TriState::on);
                                           REQUIRE(core->getStateChangeCount() == 1);
                                       },
                                       0);
            }
        }
    }
}
