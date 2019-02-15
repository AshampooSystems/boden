
#include <bdn/test.h>

#include <bdn/TextView.h>
#include <bdn/test/MockTextViewCore.h>
#include <bdn/test/testView.h>

using namespace bdn;

TEST_CASE("TextView")
{
    // test the generic view properties of Button
    SECTION("View-base")
    bdn::test::testView<TextView>();

    SECTION("TextView-specific")
    {
        std::shared_ptr<bdn::test::ViewTestPreparer<TextView>> preparer =
            std::make_shared<bdn::test::ViewTestPreparer<TextView>>();
        std::shared_ptr<bdn::test::ViewWithTestExtensions<TextView>> textView = preparer->createView();
        std::shared_ptr<bdn::test::MockTextViewCore> core =
            std::dynamic_pointer_cast<bdn::test::MockTextViewCore>(textView->getViewCore());

        REQUIRE(core != nullptr);

        SECTION("initialState")
        {
            // the test view from the text view preparer has a text set (so that
            // the preferredsizehint will have an effect). So it is not in the
            // initial state. So we create a new one here to test the initial
            // state.
            std::shared_ptr<TextView> newView = std::make_shared<TextView>();
            preparer->getWindow()->setContentView(newView);

            std::shared_ptr<bdn::test::MockTextViewCore> newCore =
                std::dynamic_pointer_cast<bdn::test::MockTextViewCore>(newView->getViewCore());

            SECTION("text")
            {
                REQUIRE(newView->text == "");
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
                        textView, preparer, [textView, preparer]() { textView->text = ("hello"); },
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
