#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextField.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextFieldCore.h>

#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosTextFieldCore
    : public bdn::test::TestIosViewCoreMixin<bdn::test::TestTextFieldCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestIosViewCoreMixin<
            bdn::test::TestTextFieldCore>::initCore();

        _pUITextField = (UITextField *)_pUIView;
        REQUIRE(_pUITextField != nullptr);
    }

    void verifyCoreText() override
    {
        String expectedText = _pTextField->text();
        String text = bdn::ios::iosStringToString(_pUITextField.text);
        REQUIRE(text == expectedText);
    }

  protected:
    UITextField *_pUITextField;
};

TEST_CASE("ios.TextFieldCore")
{
    P<TestIosTextFieldCore> pTest = newObj<TestIosTextFieldCore>();
    pTest->runTests();
}
