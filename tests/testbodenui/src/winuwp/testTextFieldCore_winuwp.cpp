#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextField.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextFieldCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/TextFieldCore.h>
#include "TestWinuwpViewCoreMixin.h"

using namespace bdn;

class TestWinuwpTextFieldCore
    : public bdn::test::TestWinuwpViewCoreMixin<bdn::test::TestTextFieldCore>
{
  protected:
    void initCore() override
    {
        TestWinuwpViewCoreMixin<TestTextFieldCore>::initCore();

        _pWinTextBox = dynamic_cast<::Windows::UI::Xaml::Controls::TextBox ^>(
            _pWinFrameworkElement);
        REQUIRE(_pWinTextBox != nullptr);
    }

    UiMargin getExpectedDefaultPadding() override
    {
        return UiMargin(UiLength::sem(0.4), UiLength::sem(0.4));
    }

    void verifyCorePadding() override
    {
        verifyIsExpectedWinPadding((_pWinTextBox)->Padding);
    }

    void verifyCoreText() override
    {
        String expectedText = _pTextField->text();
        String text = _pWinTextBox->Text->Data();
        REQUIRE(text == expectedText);
    }

    ::Windows::UI::Xaml::Controls::TextBox ^ _pWinTextBox;
};

TEST_CASE("winuwp.TextFieldCore")
{
    P<TestWinuwpTextFieldCore> pTest = newObj<TestWinuwpTextFieldCore>();
    pTest->runTests();
}
