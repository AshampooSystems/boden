#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Switch.h>
#include <bdn/Window.h>
#include <bdn/test/TestSwitchCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/CheckboxCore.h>
#include "TestWinuwpViewCoreMixin.h"

using namespace bdn;

class TestWinuwpSwitchCore
    : public bdn::test::TestWinuwpViewCoreMixin<bdn::test::TestSwitchCore>
{
  protected:
    void initCore() override
    {
        TestWinuwpViewCoreMixin<TestSwitchCore>::initCore();

        _pWinSwitch = dynamic_cast<::Windows::UI::Xaml::Controls::StackPanel ^>(
            _pWinFrameworkElement);
        REQUIRE(_pWinSwitch != nullptr);
    }

    UiMargin getExpectedDefaultPadding() override
    {
        return UiMargin(UiLength::sem(0.4), UiLength::sem(1));
    }

    void verifyCorePadding() override
    {
        verifyIsExpectedWinPadding(_pWinSwitch->Padding);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pSwitch->label();

        String label =
            dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock ^>(
                dynamic_cast<::Windows::UI::Xaml::Controls::ToggleSwitch ^>(
                    _pWinSwitch->Children->GetAt(0))
                    ->Header)
                ->Text->Data();

        REQUIRE(label == expectedLabel);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pSwitch->on();
        bool on = dynamic_cast<::Windows::UI::Xaml::Controls::ToggleSwitch ^>(
                      _pWinSwitch->Children->GetAt(0))
                      ->IsOn;
        REQUIRE(on == expectedOn);
    }

    ::Windows::UI::Xaml::Controls::StackPanel ^ _pWinSwitch;
};

TEST_CASE("winuwp.SwitchCore")
{
    P<TestWinuwpSwitchCore> pTest = newObj<TestWinuwpSwitchCore>();

    pTest->runTests();
}
