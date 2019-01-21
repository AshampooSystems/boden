
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>

#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;

class TestMacButtonCore : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestButtonCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin<bdn::test::TestButtonCore>::initCore();

        _nSButton = (NSButton *)_nSView;
        REQUIRE(_nSButton != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _button->label;

        String label = bdn::mac::macStringToString(_nSButton.title);

        REQUIRE(label == expectedLabel);
    }

  protected:
    NSButton *_nSButton;
};

TEST_CASE("mac.ButtonCore")
{
    std::shared_ptr<TestMacButtonCore> test = std::make_shared<TestMacButtonCore>();

    test->runTests();
}
