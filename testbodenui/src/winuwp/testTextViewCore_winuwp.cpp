#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextView.h>
#include <bdn/Window.h>
#include <bdn/test/TestTextViewCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/TextViewCore.h>
#include "TestWinuwpViewCoreMixin.h"

using namespace bdn;

class TestWinuwpTextViewCore : public bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestTextViewCore >
{
protected:

    void initCore() override
    {
        bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestTextViewCore >::initCore();

        _pWinTextBlock = dynamic_cast<::Windows::UI::Xaml::Controls::TextBlock^>( _pWinFrameworkElement );
        REQUIRE( _pWinTextBlock!=nullptr );
    }

    void verifyCorePadding() override
    {
        verifyIsExpectedWinPadding( _pWinTextBlock->Padding );
    }

    void verifyCoreText() override
    {
        String expectedText = _pTextView->text();

        String text = _pWinTextBlock->Text->Data();                
        
        REQUIRE( text == expectedText );
    }


    ::Windows::UI::Xaml::Controls::TextBlock^ _pWinTextBlock;
};

TEST_CASE("winuwp.TextViewCore")
{
    P<TestWinuwpTextViewCore> pTest = newObj<TestWinuwpTextViewCore>();

    pTest->runTests();
}






