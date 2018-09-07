#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/WindowCore.h>
#include "TestWinuwpViewCoreMixin.h"

using namespace bdn;

class TestWinuwpWindowCore
    : public bdn::test::TestWinuwpViewCoreMixin<bdn::test::TestWindowCore>
{
  protected:
    bool canManuallyChangePosition() const override { return false; }

    bool canManuallyChangeSize() const override { return false; }

    void initCore() override
    {
        bdn::test::TestWinuwpViewCoreMixin<
            bdn::test::TestWindowCore>::initCore();
    }

    void verifyCorePadding() override
    {
        // the padding is not reflected in the core UI element. The outer view
        // object handles it. So, nothing to test here.
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _pWindow->title();

        // the title is not reflected by the core UI element. So, nothing to
        // test here.
    }

    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(::Windows::UI::Xaml::FrameworkElement ^ pEl)
        {
            this->pEl = pEl;
        }

        ::Windows::UI::Xaml::FrameworkElement ^ pEl;
    };

    P<IBase> createInfoToVerifyCoreUiElementDestruction() override
    {
        // sanity check. If Parent is null then initialization has not yet
        // completed. Note that the initialization happens with low priority, so
        // it will not happen when there are events in the queue.
        REQUIRE(_pWinFrameworkElement->Parent != nullptr);

        return newObj<DestructVerificationInfo>(_pWinFrameworkElement);
    }

    void verifyCoreUiElementDestruction(IBase *pVerificationInfo) override
    {
        ::Windows::UI::Xaml::FrameworkElement ^ pEl =
            cast<DestructVerificationInfo>(pVerificationInfo)->pEl;

        // should have been removed from its parent.
        REQUIRE(_pWinFrameworkElement->Parent == nullptr);
    }
};

TEST_CASE("winuwp.WindowCore")
{
    P<TestWinuwpWindowCore> pTest = newObj<TestWinuwpWindowCore>();

    pTest->runTests();
}
