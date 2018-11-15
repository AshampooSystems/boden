#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Checkbox.h>
#include <bdn/Window.h>
#include <bdn/test/TestCheckboxCore.h>
#include <bdn/android/CheckboxCore.h>
#include <bdn/android/JCheckBox.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;

class TestAndroidCheckboxCore : public bdn::test::TestAndroidViewCoreMixin<bdn::test::TestCheckboxCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<bdn::test::TestCheckboxCore>::initCore();

        _jCheckBox = bdn::android::JCheckBox(_jView.getRef_());
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pCheckbox->label();
        String label = _jCheckBox.getText();
        REQUIRE(label == expectedLabel);
    }

    void verifyCoreState() override
    {
        TriState expectedState = _pCheckbox->state();
        TriState state = cast<bdn::android::CheckboxCore<Checkbox>>(_pAndroidViewCore)->getState();
        REQUIRE(state == expectedState);
    }

  protected:
    bdn::android::JCheckBox _jCheckBox;
};

TEST_CASE("android.CheckboxCore")
{
    P<TestAndroidCheckboxCore> pTest = newObj<TestAndroidCheckboxCore>();

    pTest->runTests();
}
