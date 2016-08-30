#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/WindowCore.h>
#include "TestWinuwpViewCoreMixin.h"

using namespace bdn;




class TestWinuwpWindowCore : public bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestWindowCore >
{
protected:

    bool canManuallyChangeBounds() const override
    {
        return false;
    }

    void initCore() override
    {
        bdn::test::TestWinuwpViewCoreMixin< bdn::test::TestWindowCore >::initCore();

        _pWindow->visible() = true;
    }

    void verifyCorePadding() override
    {
        // the padding is not reflected in the core UI element. The outer view object handles it.
        // So, nothing to test here.
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _pWindow->title();

        // the title is not reflected by the core UI element. So, nothing to test here.
    }


    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(::Windows::UI::Xaml::FrameworkElement^ pEl)
        {
            this->pEl = pEl;
        }
        
        ::Windows::UI::Xaml::FrameworkElement^ pEl;
    };
    
    P<IBase> createInfoToVerifyCoreUiElementDestruction() override
    {
        // XXX test disabled. For some reason the Parent property is always null,
        // so we cannot test for the destruction.
        return nullptr;

        /*

        // sanity check
        REQUIRE( _pWinFrameworkElement->Parent!=nullptr );

        return newObj<DestructVerificationInfo>( _pWinFrameworkElement );*/
    }


    void verifyCoreUiElementDestruction(IBase* pVerificationInfo) override
    {
        // XXX test disabled. For some reason the Parent property is always null,
        // so we cannot test for the destruction.

        /*

        ::Windows::UI::Xaml::FrameworkElement^ pEl = cast<DestructVerificationInfo>( pVerificationInfo )->pEl;
        
        // should have been removed from its parent.
        REQUIRE( _pWinFrameworkElement->Parent==nullptr );

        */
    }
};

TEST_CASE("winuwp.WindowCore")
{
    P<TestWinuwpWindowCore> pTest = newObj<TestWinuwpWindowCore>();

    pTest->runTests();
}






