#include <bdn/init.h>
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

        _pNSButton = (NSButton *)_pNSView;
        REQUIRE(_pNSButton != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pToggle->label();

        String label = bdn::mac::macStringToString(_pNSButton.title);

        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pToggle->on();
        bool on = _pNSButton.state == NSControlStateValueOn;
        REQUIRE(on == expectedOn);
    }

  protected:
    NSButton *_pNSButton;
};

TEST_CASE("mac.ToggleCore")
{
    P<TestMacToggleCore> pTest = newObj<TestMacToggleCore>();

    pTest->runTests();
}
