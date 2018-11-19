#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockButtonCore.h>

using namespace bdn;

TEST_CASE("Button")
{
    // test the generic view properties of Button
    SECTION("View-base")
    bdn::test::testView<Button>();

    SECTION("Button-specific")
    {
        P<bdn::test::ViewTestPreparer<Button>> preparer = newObj<bdn::test::ViewTestPreparer<Button>>();
        P<bdn::test::ViewWithTestExtensions<Button>> button = preparer->createView();
        P<bdn::test::MockButtonCore> core = cast<bdn::test::MockButtonCore>(button->getViewCore());

        REQUIRE(core != nullptr);

        SECTION("initialButtonState")
        {
            SECTION("label")
            {
                REQUIRE(button->label() == "");
                REQUIRE(core->getLabel() == "");
                REQUIRE(core->getLabelChangeCount() == 0);
            }
        }

        SECTION("changeButtonProperty")
        {
            SECTION("label")
            {
                bdn::test::_testViewOp(
                    button, preparer, [button]() { button->setLabel("hello"); },
                    [core, button] {
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
        }
    }
}
