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
        P<bdn::test::ViewTestPreparer<TextField>> pPreparer = newObj<bdn::test::ViewTestPreparer<TextField>>();
        P<bdn::test::ViewWithTestExtensions<TextField>> pTextField = pPreparer->createView();
        P<bdn::test::MockTextFieldCore> pCore = cast<bdn::test::MockTextFieldCore>(pTextField->getViewCore());

        REQUIRE(pCore != nullptr);

        SECTION("initialTextFieldState")
        {
            SECTION("text")
            {
                REQUIRE(pTextField->text() == "");
                REQUIRE(pCore->getText() == "");
                REQUIRE(pCore->getTextChangeCount() == 0);
            }
        }

        SECTION("changeTextFieldProperty")
        {
            SECTION("text")
            {
                bdn::test::_testViewOp(pTextField, pPreparer, [pTextField]() { pTextField->setText("hello"); },
                                       [pCore, pTextField] {
                                           REQUIRE(pCore->getText() == "hello");
                                           REQUIRE(pCore->getTextChangeCount() == 1);
                                       },
                                       0);
            }
        }

        SECTION("submit")
        {
            bool submitted = false;
            pTextField->onSubmit().subscribeParamless([&submitted]() { submitted = true; });

            pTextField->submit();

            REQUIRE(submitted == true);
        }
    }
}
