
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#import <bdn/mac/UIProvider.hh>
#import <bdn/mac/WindowCore.hh>
#import <bdn/mac/ChildViewCore.hh>

using namespace bdn;

class TestMacWindowCore : public bdn::test::TestWindowCore
{
  protected:
    void runPostInitTests() override
    {
        bdn::test::TestWindowCore::runPostInitTests();

        SECTION("content parent is flipped")
        {
            // the window's content parent must have the flipped
            // flag set, so that the origin of the coordinate
            // system is the top-left.
            std::shared_ptr<Button> button = std::make_shared<Button>();
            _window->setContentView(button);

            NSView *child = std::dynamic_pointer_cast<bdn::mac::ChildViewCore>(button->getViewCore())->getNSView();

            REQUIRE(child.superview.flipped);
        }
    }

    void initCore() override
    {
        TestWindowCore::initCore();

        _macWindowCore = std::dynamic_pointer_cast<bdn::mac::WindowCore>(_view->getViewCore());
        REQUIRE(_macWindowCore != nullptr);

        _nsWindow = _macWindowCore->getNSWindow();
        REQUIRE(_nsWindow != nullptr);
    }

    std::shared_ptr<UIProvider> getUIProvider() override { return bdn::mac::UIProvider::get(); }

    void verifyCoreVisibility() override
    {
        bool expectedVisible = _view->visible;

        REQUIRE(_nsWindow.visible == expectedVisible);
    }

    bdn::Rect getFrameRect() const
    {
        NSScreen *screen = _nsWindow.screen;
        if (screen == nil) // happens when window is not visible
            screen = [NSScreen mainScreen];

        bdn::Rect resultRect = bdn::mac::macRectToRect(_nsWindow.frame, screen.frame.size.height);

        return resultRect;
    }

    void verifyInitialDummyCoreSize() override
    {
        bdn::Rect rect = getFrameRect();

        REQUIRE(rect.getSize() == bdn::Size());
    }

    void verifyCorePosition() override
    {
        bdn::Rect rect = getFrameRect();
        bdn::Point expectedPosition = _view->position;

        REQUIRE(rect.getPosition() == expectedPosition);
    }

    void verifyCoreSize() override
    {
        bdn::Rect rect = getFrameRect();
        bdn::Size expectedSize = _view->size;

        REQUIRE(rect.getSize() == expectedSize);
    }

    void verifyCorePadding() override
    {
        // the padding is not reflected in Cocoa properties.
        // So nothing to test here.
    }

    void verifyCoreTitle() override
    {
        String expectedTitle = _window->title;

        String title = bdn::mac::nsStringToString(_nsWindow.title);

        REQUIRE(title == expectedTitle);
    }

    void clearAllReferencesToCore() override
    {
        TestWindowCore::clearAllReferencesToCore();

        _macWindowCore = nullptr;
        _nsWindow = nullptr;
    }

    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(NSWindow *nSWindow) { this->_nsWindow = nSWindow; }

        // store a weak reference so that we do not keep the window alive
        NSWindow __weak *_nsWindow;
    };

    std::shared_ptr<Base> createInfoToVerifyCoreUIElementDestruction() override
    {
        // sanity check
        REQUIRE(_nsWindow != nullptr);

        return std::make_shared<DestructVerificationInfo>(_nsWindow);
    }

    void verifyCoreUIElementDestruction(std::shared_ptr<Base> verificationInfo) override
    {
        NSWindow __weak *nSWindow = std::dynamic_pointer_cast<DestructVerificationInfo>(verificationInfo)->_nsWindow;

        // window should have been destroyed.
        REQUIRE(nSWindow == nullptr);
    }

    std::shared_ptr<bdn::mac::WindowCore> _macWindowCore;
    NSWindow *_nsWindow;
};

TEST_CASE("mac.WindowCore")
{
    std::shared_ptr<TestMacWindowCore> test = std::make_shared<TestMacWindowCore>();

    test->runTests();
}
