
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
        String expectedLabel = _checkbox->label;
        String label = _jCheckBox.getText();
        REQUIRE(label == expectedLabel);
    }

    void verifyCoreState() override
    {
        TriState expectedState = _checkbox->state;
        TriState state = std::dynamic_pointer_cast<bdn::android::CheckboxCore<Checkbox>>(_androidViewCore)->getState();
        REQUIRE(state == expectedState);
    }

  protected:
    bdn::android::JCheckBox _jCheckBox;
};

TEST_CASE("android.CheckboxCore")
{
    std::shared_ptr<TestAndroidCheckboxCore> test = std::make_shared<TestAndroidCheckboxCore>();

    test->runTests();
}
