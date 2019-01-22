
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

        _nSTextField = (NSTextField *)_nSView;
        REQUIRE(_nSTextField != nullptr);
    }

    void verifyCoreText() override
    {
        String expectedText = _textField->text;
        String text = bdn::mac::nsStringToString(_nSTextField.stringValue);
        REQUIRE(text == expectedText);
    }

  protected:
    NSTextField *_nSTextField;
};

TEST_CASE("mac.TextFieldCore")
{
    std::shared_ptr<TestMacTextFieldCore> test = std::make_shared<TestMacTextFieldCore>();

    test->runTests();
}
