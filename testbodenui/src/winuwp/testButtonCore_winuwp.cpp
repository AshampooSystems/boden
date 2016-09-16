#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/ButtonCore.h>
#include "TestWinuwpViewCoreMixin.h"

using namespace bdn;


class TestWinuwpButtonCore : public bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestButtonCore >
{
protected:

    void initCore() override
    {
        TestWinuwpViewCoreMixin< TestButtonCore >::initCore();

        _pWinButton = dynamic_cast<::Windows::UI::Xaml::Controls::Button^>( _pWinFrameworkElement );
        REQUIRE( _pWinButton!=nullptr );
    }

    UiMargin getExpectedDefaultPadding() override
    {
        return UiMargin(UiLength::sem, 0.4, 1);
    }

    void verifyCorePadding() override
    {
        verifyIsExpectedWinPadding( _pWinButton->Padding );
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _pButton->label();

        String label = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>( _pWinButton->Content )->Text->Data();                
        
        REQUIRE( label == expectedLabel );
    }


    ::Windows::UI::Xaml::Controls::Button^ _pWinButton;
};

TEST_CASE("winuwp.ButtonCore")
{
    P<TestWinuwpButtonCore> pTest = newObj<TestWinuwpButtonCore>();

    pTest->runTests();
}




