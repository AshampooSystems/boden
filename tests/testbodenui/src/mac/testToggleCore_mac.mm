
#include <bdn/test.h>

#include <bdn/Toggle.h>
#include <bdn/Window.h>
#include <bdn/test/TestToggleCore.h>

#import "TestMacChildViewCoreMixin.hh"

using namespace bdn;

class TestMacToggleCore : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestToggleCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin<bdn::test::TestToggleCore>::initCore();

        _nSButton = (NSButton *)_nSView;
        REQUIRE(_nSButton != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _toggle->label;

        String label = bdn::mac::macStringToString(_nSButton.title);

        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _toggle->on;
        bool on = _nSButton.state == NSControlStateValueOn;
        REQUIRE(on == expectedOn);
    }

  protected:
    NSButton *_nSButton;
};

TEST_CASE("mac.ToggleCore")
{
    std::shared_ptr<TestMacToggleCore> test = std::make_shared<TestMacToggleCore>();

    test->runTests();
}
