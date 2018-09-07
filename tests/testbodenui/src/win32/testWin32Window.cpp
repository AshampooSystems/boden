#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/win32/Win32Window.h>

#include <windows.h>

using namespace bdn;
using namespace bdn::win32;

class TestWindow : public Win32Window
{
  public:
    TestWindow() : Win32Window("BUTTON", "", 0, 0, NULL) {}

    std::function<void()> _notifyDestroyFunc;

  protected:
    void notifyDestroy() override { _notifyDestroyFunc(); }
};

TEST_CASE("win32.Win32Window")
{
    SECTION("notifyDestroy called")
    {
        int notifyDestroyCallCount = 0;
        P<TestWindow> pWindow = newObj<TestWindow>();

        pWindow->_notifyDestroyFunc = [&notifyDestroyCallCount]() {
            notifyDestroyCallCount++;
        };

        SECTION("destroy()")
        {
            pWindow->destroy();

            REQUIRE(notifyDestroyCallCount == 1);
        }

        SECTION("WM_DESTROY from outside")
        {
            ::DestroyWindow(pWindow->getHwnd());

            REQUIRE(notifyDestroyCallCount == 1);
        }

        SECTION("destructor")
        {
            pWindow = nullptr;

            // while the base class destructor destroys the window, we will not
            // be notified in that case, because the derived class is destroyed
            // first. So our overload of notifyDestroy does not exist anymore at
            // that point.
            REQUIRE(notifyDestroyCallCount == 0);
        }

        pWindow = nullptr;
        REQUIRE(notifyDestroyCallCount <= 1);
    }
}
