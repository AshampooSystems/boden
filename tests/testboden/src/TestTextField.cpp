#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextField.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockTextFieldCore.h>

using namespace bdn;

TEST_CASE("TextField")
{
    // Test the generic view properties of TextField
    SECTION("View-base")
    bdn::test::testView<TextField>();

    SECTION("TextField-specific")
    {
        P<bdn::test::ViewTestPreparer<TextField>> preparer = newObj<bdn::test::ViewTestPreparer<TextField>>();
        P<bdn::test::ViewWithTestExtensions<TextField>> textField = preparer->createView();
        P<bdn::test::MockTextFieldCore> core = cast<bdn::test::MockTextFieldCore>(textField->getViewCore());

        REQUIRE(core != nullptr);

        SECTION("initialTextFieldState")
        {
            SECTION("text")
            {
                REQUIRE(textField->text() == "");
                REQUIRE(core->getText() == "");
                REQUIRE(core->getTextChangeCount() == 0);
            }
        }

        SECTION("changeTextFieldProperty")
        {
            SECTION("text")
            {
                bdn::test::_testViewOp(textField, preparer, [textField]() { textField->setText("hello"); },
                                       [core, textField] {
                                           REQUIRE(core->getText() == "hello");
                                           REQUIRE(core->getTextChangeCount() == 1);
                                       },
                                       0);
            }
        }

        SECTION("submit")
        {
            bool submitted = false;
            textField->onSubmit().subscribeParamless([&submitted]() { submitted = true; });

            textField->submit();

            REQUIRE(submitted == true);
        }
    }
}
