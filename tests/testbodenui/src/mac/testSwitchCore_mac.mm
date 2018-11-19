#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/Window.h>
#include <bdn/test/TestSwitchCore.h>

#import "TestMacChildViewCoreMixin.hh"
#import <bdn/mac/SwitchCore.hh>

using namespace bdn;

class TestMacSwitchCore : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestSwitchCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin<bdn::test::TestSwitchCore>::initCore();

        _switchComposite = (BdnMacSwitchComposite *)_nSView;
        REQUIRE(_switchComposite != nullptr);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _switch->label();

        String label = bdn::mac::macStringToString(_switchComposite.label.stringValue);

        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _switch->on();
        bool on = _switchComposite.bdnSwitch.on;
        REQUIRE(on == expectedOn);
    }

  protected:
    BdnMacSwitchComposite *_switchComposite;
};

TEST_CASE("mac.SwitchCore")
{
    P<TestMacSwitchCore> test = newObj<TestMacSwitchCore>();

    test->runTests();
}
