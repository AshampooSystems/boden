#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>

#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosButtonCore : public bdn::test::TestIosViewCoreMixin<bdn::test::TestButtonCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestIosViewCoreMixin<bdn::test::TestButtonCore>::initCore();

        _uIButton = (UIButton *)_uIView;
        REQUIRE(_uIButton != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _button->label();

        String label = bdn::ios::iosStringToString(_uIButton.currentTitle);

        REQUIRE(label == expectedLabel);
    }

  protected:
    UIButton *_uIButton;
};

TEST_CASE("ios.ButtonCore")
{
    P<TestIosButtonCore> test = newObj<TestIosButtonCore>();

    test->runTests();
}
