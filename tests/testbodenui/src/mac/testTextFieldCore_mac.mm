
#include <bdn/test.h>

#include <bdn/TextField.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextFieldCore.h>

#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;

class TestMacTextFieldCore : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestTextFieldCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin<bdn::test::TestTextFieldCore>::initCore();

        _nsTextField = (NSTextField *)_nsView;
        REQUIRE(_nsTextField != nullptr);
    }

    void verifyCoreText() override
    {
        String expectedText = _textField->text;
        String text = bdn::mac::nsStringToString(_nsTextField.stringValue);
        REQUIRE(text == expectedText);
    }

  protected:
    NSTextField *_nsTextField;
};

TEST_CASE("mac.TextFieldCore")
{
    std::shared_ptr<TestMacTextFieldCore> test = std::make_shared<TestMacTextFieldCore>();

    test->runTests();
}
