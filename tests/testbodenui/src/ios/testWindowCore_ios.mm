#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/WindowCore.hh>
#import <bdn/ios/ViewCore.hh>

#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosWindowCore
    : public bdn::test::TestIosViewCoreMixin<bdn::test::TestWindowCore>
{
  protected:
    void initCore() override
    {
        TestIosViewCoreMixin<TestWindowCore>::initCore();

        _pUIWindow = (UIWindow *)_pUIView;
        REQUIRE(_pUIWindow != nullptr);
    }

    IUiProvider &getUiProvider() override
    {
        return bdn::ios::UiProvider::get();
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _pWindow->title();

        String title =
            bdn::ios::iosStringToString(_pUIWindow.rootViewController.title);

        REQUIRE(title == expectedTitle);
    }

    bool canManuallyChangePosition() const override
    {
        // we cannot modify the size and position of top level windows.
        return false;
    }

    bool canManuallyChangeSize() const override
    {
        // we cannot modify the size and position of top level windows.
        return false;
    }

    void clearAllReferencesToCore() override
    {
        TestIosViewCoreMixin<TestWindowCore>::clearAllReferencesToCore();

        _pIosViewCore = nullptr;
        _pUIView = nullptr;
        _pUIWindow = nullptr;
    }

    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(UIWindow *pUIWindow)
        {
            this->pUIWindow = pUIWindow;
        }

        // store a weak reference so that we do not keep the window alive
        __weak UIWindow *pUIWindow;
    };

    P<IBase> createInfoToVerifyCoreUiElementDestruction() override
    {
        // sanity check
        REQUIRE(_pUIWindow != nullptr);

        return newObj<DestructVerificationInfo>(_pUIWindow);
    }

    void verifyCoreUiElementDestruction(IBase *pVerificationInfo) override
    {
        __weak UIWindow *pUIWindow =
            cast<DestructVerificationInfo>(pVerificationInfo)->pUIWindow;

        // window should have been destroyed.
        REQUIRE(pUIWindow == nullptr);
    }

    UIWindow *_pUIWindow;
};

TEST_CASE("ios.WindowCore")
{
    P<TestIosWindowCore> pTest = newObj<TestIosWindowCore>();

    pTest->runTests();
}
