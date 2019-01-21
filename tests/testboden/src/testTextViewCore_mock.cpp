
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
        String expectedText = _textView->text;

        String text = std::dynamic_pointer_cast<bdn::test::MockTextViewCore>(_mockCore)->getText();

        REQUIRE(text == expectedText);
    }

    bool wrapsAtCharacterBoundariesIfWordDoesNotFit() const override { return true; }
};

TEST_CASE("mock.TextViewCore")
{
    std::shared_ptr<TestMockTextViewCore> test = std::make_shared<TestMockTextViewCore>();

    test->runTests();
}
