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

        _pUIButton = (UIButton *)_pUIView;
        REQUIRE(_pUIButton != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pButton->label();

        String label = bdn::ios::iosStringToString(_pUIButton.currentTitle);

        REQUIRE(label == expectedLabel);
    }

  protected:
    UIButton *_pUIButton;
};

TEST_CASE("ios.ButtonCore")
{
    P<TestIosButtonCore> pTest = newObj<TestIosButtonCore>();

    pTest->runTests();
}
