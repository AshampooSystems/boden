#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#include <bdn/test/MockTextViewCore.h>
#include "TestMockViewCoreMixin.h"

using namespace bdn;

class TestMockTextViewCore : public bdn::test::TestMockViewCoreMixin<bdn::test::TestTextViewCore>
{
  protected:
    void verifyCoreText() override
    {
        String expectedText = _pTextView->text();

        String text = cast<bdn::test::MockTextViewCore>(_pMockCore)->getText();

        REQUIRE(text == expectedText);
    }

    bool wrapsAtCharacterBoundariesIfWordDoesNotFit() const override { return true; }
};

TEST_CASE("mock.TextViewCore")
{
    P<TestMockTextViewCore> pTest = newObj<TestMockTextViewCore>();

    pTest->runTests();
}
