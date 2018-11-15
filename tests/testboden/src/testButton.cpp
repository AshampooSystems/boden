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
        P<bdn::test::ViewTestPreparer<Button>> pPreparer = newObj<bdn::test::ViewTestPreparer<Button>>();
        P<bdn::test::ViewWithTestExtensions<Button>> pButton = pPreparer->createView();
        P<bdn::test::MockButtonCore> pCore = cast<bdn::test::MockButtonCore>(pButton->getViewCore());

        REQUIRE(pCore != nullptr);

        SECTION("initialButtonState")
        {
            SECTION("label")
            {
                REQUIRE(pButton->label() == "");
                REQUIRE(pCore->getLabel() == "");
                REQUIRE(pCore->getLabelChangeCount() == 0);
            }
        }

        SECTION("changeButtonProperty")
        {
            SECTION("label")
            {
                bdn::test::_testViewOp(
                    pButton, pPreparer, [pButton]() { pButton->setLabel("hello"); },
                    [pCore, pButton] {
                        REQUIRE(pCore->getLabel() == "hello");
                        REQUIRE(pCore->getLabelChangeCount() == 1);
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
