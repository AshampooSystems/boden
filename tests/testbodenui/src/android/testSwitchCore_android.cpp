#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/Window.h>
#include <bdn/test/TestSwitchCore.h>
#include <bdn/android/JSwitch.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;

class TestAndroidSwitchCore
    : public bdn::test::TestAndroidViewCoreMixin<bdn::test::TestSwitchCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<
            bdn::test::TestSwitchCore>::initCore();

        _jSwitch = bdn::android::JSwitch(_jView.getRef_());
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pSwitch->label();
        String label = _jSwitch.getText();
        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pSwitch->on();
        bool on = _jSwitch.isChecked();
        REQUIRE(on == expectedOn);
    }

  protected:
    bdn::android::JSwitch _jSwitch;
};

TEST_CASE("android.SwitchCore")
{
    P<TestAndroidSwitchCore> pTest = newObj<TestAndroidSwitchCore>();

    pTest->runTests();
}
