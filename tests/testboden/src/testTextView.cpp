#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextView.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockTextViewCore.h>

using namespace bdn;

TEST_CASE("TextView")
{
    // test the generic view properties of Button
    SECTION("View-base")
    bdn::test::testView<TextView>();

    SECTION("TextView-specific")
    {
        P<bdn::test::ViewTestPreparer<TextView>> preparer = newObj<bdn::test::ViewTestPreparer<TextView>>();
        P<bdn::test::ViewWithTestExtensions<TextView>> textView = preparer->createView();
        P<bdn::test::MockTextViewCore> core = cast<bdn::test::MockTextViewCore>(textView->getViewCore());

        REQUIRE(core != nullptr);

        SECTION("initialState")
        {
            // the test view from the text view preparer has a text set (so that
            // the preferredsizehint will have an effect). So it is not in the
            // initial state. So we create a new one here to test the initial
            // state.
            P<TextView> newView = newObj<TextView>();
            preparer->getWindow()->setContentView(newView);

            P<bdn::test::MockTextViewCore> newCore = cast<bdn::test::MockTextViewCore>(newView->getViewCore());

            SECTION("text")
            {
                REQUIRE(newView->text() == "");
                REQUIRE(newCore->getText() == "");
                REQUIRE(newCore->getTextChangeCount() == 0);
            }
        }

        SECTION("changeProperty")
        {
            SECTION("text")
            {
                CONTINUE_SECTION_WHEN_IDLE(textView, preparer, core)
                {
                    int initialChangeCount = core->getTextChangeCount();

                    bdn::test::_testViewOp(
                        textView, preparer, [textView, preparer]() { textView->setText("hello"); },
                        [core, textView, preparer, initialChangeCount] {
                            REQUIRE(core->getText() == "hello");
                            REQUIRE(core->getTextChangeCount() == initialChangeCount + 1);
                        },
                        bdn::test::ExpectedSideEffect_::invalidateSizingInfo         // should have caused
                                                                                     // sizing info to be
                                                                                     // invalidated
                            | bdn::test::ExpectedSideEffect_::invalidateParentLayout // should cause a
                                                                                     // parent layout update
                                                                                     // since sizing info
                                                                                     // was invalidated
                    );
                };
            }
        }
    }
}
