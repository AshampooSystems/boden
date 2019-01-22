
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/Window.h>
#include <bdn/test/TestSwitchCore.h>
#include <bdn/ios/SwitchCore.hh>

#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosSwitchCore : public bdn::test::TestIosViewCoreMixin<bdn::test::TestSwitchCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestIosViewCoreMixin<bdn::test::TestSwitchCore>::initCore();
        _switchComposite = (BdnIosSwitchComposite *)_uIView;
        REQUIRE(_switchComposite != nullptr);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _switch->on;
        bool on = _switchComposite.uiSwitch.on;
        REQUIRE(on == expectedOn);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _switch->label;
        String label = bdn::ios::nsStringToString(_switchComposite.uiLabel.text);
        REQUIRE(label == expectedLabel);
    }

  protected:
    BdnIosSwitchComposite *_switchComposite;
};

TEST_CASE("ios.SwitchCore")
{
    std::shared_ptr<TestIosSwitchCore> test = std::make_shared<TestIosSwitchCore>();

    test->runTests();
}
