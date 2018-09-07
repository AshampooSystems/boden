#ifndef BDN_TEST_TestWin32ViewCoreMixin_H_
#define BDN_TEST_TestWin32ViewCoreMixin_H_

#include <bdn/View.h>
#include <bdn/win32/ViewCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/Win32Window.h>
#include <bdn/win32/util.h>

namespace bdn
{
    namespace test
    {

        /** A mixin class that adds implementations of win32 view specific
           functionality on top of the base class specified in the template
           parameter BaseClass.*/
        template <class BaseClass>
        class TestWin32ViewCoreMixin : public BaseClass
        {

          protected:
            void initCore() override
            {
                BaseClass::initCore();

                _pWin32Core = cast<bdn::win32::ViewCore>(_pView->getViewCore());
                REQUIRE(_pWin32Core != nullptr);

                _hwnd = _pWin32Core->getHwnd();
                REQUIRE(_hwnd != nullptr);
            }

            IUiProvider &getUiProvider() override
            {
                return bdn::win32::UiProvider::get();
            }

            void verifyCoreVisibility() override
            {
                bool expectedVisible = _pView->visible();

                REQUIRE(((::GetWindowLong(_hwnd, GWL_STYLE) & WS_VISIBLE) ==
                         WS_VISIBLE) == expectedVisible);
            }

            Rect getWindowRectInParent() const
            {
                RECT winRect{};
                ::GetWindowRect(_hwnd, &winRect);

                HWND parent = ::GetParent(_hwnd);
                if (parent != NULL) {
                    POINT pos{winRect.left, winRect.top};

                    ::ScreenToClient(parent, &pos);

                    int diffX = winRect.left - pos.x;
                    int diffY = winRect.top - pos.y;

                    winRect.left -= diffX;
                    winRect.right -= diffX;
                    winRect.top -= diffY;
                    winRect.bottom -= diffY;
                }

                return bdn::win32::win32RectToRect(
                    winRect, _pWin32Core->getUiScaleFactor());
            }

            void verifyInitialDummyCoreSize() override
            {
                Rect rect = getWindowRectInParent();

                REQUIRE(rect.getSize() == Size());
            }

            void verifyCorePosition() override
            {
                Rect rect = getWindowRectInParent();
                Point expectedPosition = _pView->position();

                REQUIRE(rect.getPosition() == expectedPosition);
            }

            void verifyCoreSize() override
            {
                Rect rect = getWindowRectInParent();
                Size expectedSize = _pView->size();

                REQUIRE(rect.getSize() == expectedSize);
            }

            void verifyCorePadding() override
            {
                // the padding is not reflected in win32 HWND properties.
                // So nothing to test here.
            }

            P<bdn::win32::ViewCore> _pWin32Core;
            HWND _hwnd;
        };
    }
}

#endif
