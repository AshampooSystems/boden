#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Toggle.h>
#include <bdn/Window.h>
#include <bdn/test/TestToggleCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/CheckboxCore.h>
#include "TestWinuwpViewCoreMixin.h"

using namespace bdn;


class TestWinuwpToggleCore : public bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestToggleCore >
{
protected:

    void initCore() override
    {
        TestWinuwpViewCoreMixin< TestToggleCore >::initCore();

        _pWinToggle = dynamic_cast<::Windows::UI::Xaml::Controls::StackPanel^>( _pWinFrameworkElement );
        REQUIRE( _pWinToggle!=nullptr );
    }

    UiMargin getExpectedDefaultPadding() override
    {
        return UiMargin(UiLength::sem(0.4), UiLength::sem(1) );
    }

    void verifyCorePadding() override
    {
        verifyIsExpectedWinPadding( _pWinToggle->Padding );
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pToggle->label();

        String label = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>( dynamic_cast<::Windows::UI::Xaml::Controls::CheckBox^>(_pWinToggle->Children->GetAt(0))->Content )->Text->Data();
        
        REQUIRE( label == expectedLabel );
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _pToggle->on();
		bool on = dynamic_cast<::Windows::UI::Xaml::Controls::CheckBox^>(_pWinToggle->Children->GetAt(0))->IsChecked->Value;
        REQUIRE( on == expectedOn );
    }

    ::Windows::UI::Xaml::Controls::StackPanel^ _pWinToggle;
};

TEST_CASE("winuwp.ToggleCore")
{
    P<TestWinuwpToggleCore> pTest = newObj<TestWinuwpToggleCore>();

    pTest->runTests();
}




