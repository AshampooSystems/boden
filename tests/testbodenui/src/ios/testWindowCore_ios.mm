
#include <bdn/test.h>

#include <bdn/Window.h>
#import <bdn/ios/ViewCore.hh>
#import <bdn/ios/ViewCoreFactory.hh>
#import <bdn/ios/WindowCore.hh>
#include <bdn/test/TestWindowCore.h>

#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosWindowCore : public bdn::test::TestIosViewCoreMixin<bdn::test::TestWindowCore>
{
  protected:
    void initCore() override
    {
        TestIosViewCoreMixin<TestWindowCore>::initCore();

        _uIWindow = (UIWindow *)_uiView;
        REQUIRE(_uIWindow != nullptr);
    }

    std::shared_ptr<ViewCoreFactory> viewCoreFactory() override { return bdn::ios::ViewCoreFactory::get(); }

    void verifyCoreTitle() override
    {
        String expectedTitle = _window->title;

        String title = bdn::ios::nsStringToString(_uIWindow.rootViewController.title);

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

        _iosViewCore = nullptr;
        _uiView = nullptr;
        _uIWindow = nullptr;
    }

    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(UIWindow *uIWindow) { this->uIWindow = uIWindow; }

        // store a weak reference so that we do not keep the window alive
        __weak UIWindow *uIWindow;
    };

    std::shared_ptr<Base> createInfoToVerifyCoreUIElementDestruction() override
    {
        // sanity check
        REQUIRE(_uIWindow != nullptr);

        return std::make_shared<DestructVerificationInfo>(_uIWindow);
    }

    void verifyCoreUIElementDestruction(std::shared_ptr<Base> verificationInfo) override
    {
        __weak UIWindow *uIWindow = std::dynamic_pointer_cast<DestructVerificationInfo>(verificationInfo)->uIWindow;

        // window should have been destroyed.
        REQUIRE(uIWindow == nullptr);
    }

    UIWindow *_uIWindow;
};

TEST_CASE("ios.WindowCore")
{
    std::shared_ptr<TestIosWindowCore> test = std::make_shared<TestIosWindowCore>();

    test->runTests();
}
