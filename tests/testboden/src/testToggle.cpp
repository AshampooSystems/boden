
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
        std::shared_ptr<bdn::test::ViewTestPreparer<Toggle>> preparer =
            std::make_shared<bdn::test::ViewTestPreparer<Toggle>>();
        std::shared_ptr<bdn::test::ViewWithTestExtensions<Toggle>> toggle = preparer->createView();
        std::shared_ptr<bdn::test::MockToggleCore> core =
            std::dynamic_pointer_cast<bdn::test::MockToggleCore>(toggle->getViewCore());

        REQUIRE(core != nullptr);

        SECTION("initialToggleState")
        {
            SECTION("label")
            {
                REQUIRE(toggle->label == "");
                REQUIRE(core->getLabel() == "");
                REQUIRE(core->getLabelChangeCount() == 0);
            }

            SECTION("on")
            {
                REQUIRE(toggle->on == false);
                REQUIRE(core->getOn() == false);
                REQUIRE(core->getOnChangeCount() == 0);
            }
        }

        SECTION("changeToggleProperty")
        {
            SECTION("label")
            {
                bdn::test::_testViewOp(
                    toggle, preparer, [toggle]() { toggle->label = ("hello"); },
                    [core, toggle] {
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
                bdn::test::_testViewOp(toggle, preparer, [toggle]() { toggle->on = (true); },
                                       [core, toggle] {
                                           REQUIRE(core->getOn() == true);
                                           REQUIRE(core->getOnChangeCount() == 1);
                                       },
                                       0);
            }
        }
    }
}
