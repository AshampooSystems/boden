#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Toggle.h>
#include <bdn/Window.h>
#include <bdn/test/TestToggleCore.h>
#include <bdn/android/JSwitch.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;

class TestAndroidToggleCore
    : public bdn::test::TestAndroidViewCoreMixin<bdn::test::TestToggleCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<
            bdn::test::TestToggleCore>::initCore();

        _jSwitch = bdn::android::JSwitch(_jView.getRef_());
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pToggle->label();
        String label = _jSwitch.getText();
        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pToggle->on();
        bool on = _jSwitch.isChecked();
        REQUIRE(on == expectedOn);
    }

  protected:
    bdn::android::JSwitch _jSwitch;
};

TEST_CASE("android.ToggleCore")
{
    P<TestAndroidToggleCore> pTest = newObj<TestAndroidToggleCore>();

    pTest->runTests();
}
