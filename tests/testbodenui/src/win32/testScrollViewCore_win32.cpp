#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Dip.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/win32/UiProvider.h>
#include <bdn/win32/ScrollViewCore.h>
#include "TestWin32ViewCoreMixin.h"

#include <bdn/win32/util.h>

using namespace bdn;

class TestWin32ScrollViewCore
    : public bdn::test::TestWin32ViewCoreMixin<bdn::test::TestScrollViewCore>
{
  protected:
    double getVertBarWidth()
    {
        double uiScaleFactor = _pWin32Core->getUiScaleFactor();
        double width = bdn::win32::ScrollViewCore::getVerticalScrollBarWidth(
            _pWin32Core->getUiScaleFactor());

        double pixelSize = 1.0 / uiScaleFactor;

        REQUIRE_ALMOST_EQUAL(width, GetSystemMetrics(SM_CXVSCROLL), pixelSize);

        return width;
    }

    double getHorzBarHeight()
    {
        double uiScaleFactor = _pWin32Core->getUiScaleFactor();
        double height =
            bdn::win32::ScrollViewCore::getHorizontalScrollBarHeight(
                _pWin32Core->getUiScaleFactor());

        double pixelSize = 1.0 / uiScaleFactor;

        REQUIRE_ALMOST_EQUAL(height, GetSystemMetrics(SM_CYHSCROLL), pixelSize);

        return height;
    }

    Size getNonClientSize()
    {
        RECT rect{0, 0, 0, 0};
        ::AdjustWindowRect(&rect, (DWORD)::GetWindowLongPtr(_hwnd, GWL_STYLE),
                           FALSE);

        return Size((-rect.left) + rect.right, (-rect.top + rect.bottom));
    }

    Size initiateScrollViewResizeToHaveViewPortSize(
        const Size &viewPortSize) override
    {
        Size viewSize = viewPortSize + getNonClientSize();

        viewSize = _pScrollView
                       ->adjustBounds(Rect(_pScrollView->position(), viewSize),
                                      RoundType::nearest, RoundType::nearest)
                       .getSize();

        _pScrollView->setPreferredSizeMinimum(viewSize);
        _pScrollView->setPreferredSizeMaximum(viewSize);

        _pWindow->requestAutoSize();

        return viewSize;
    }

    void verifyScrollsHorizontally(bool expectedVisible) override
    {
        DWORD style = (DWORD)::GetWindowLongPtr(_hwnd, GWL_STYLE);

        bool horzBarVisible = ((style & WS_HSCROLL) == WS_HSCROLL);

        REQUIRE(horzBarVisible == expectedVisible);
    }

    void verifyScrollsVertically(bool expectedVisible) override
    {
        DWORD style = (DWORD)::GetWindowLongPtr(_hwnd, GWL_STYLE);

        bool vertBarVisible = ((style & WS_VSCROLL) == WS_VSCROLL);

        REQUIRE(vertBarVisible == expectedVisible);
    }

    void verifyContentViewBounds(const Rect &expectedBounds,
                                 double maxDeviation = 0) override
    {
        maxDeviation += Dip::significanceBoundary();

        P<View> pContentView = _pScrollView->getContentView();

        if (pContentView != nullptr) {
            Rect bounds(_pScrollView->getContentView()->position(),
                        pContentView->size());

            if (maxDeviation == 0)
                REQUIRE(bounds == expectedBounds);
            else {
                REQUIRE_ALMOST_EQUAL(bounds.x, expectedBounds.x, maxDeviation);
                REQUIRE_ALMOST_EQUAL(bounds.y, expectedBounds.y, maxDeviation);
                REQUIRE_ALMOST_EQUAL(bounds.width, expectedBounds.width,
                                     maxDeviation);
                REQUIRE_ALMOST_EQUAL(bounds.height, expectedBounds.height,
                                     maxDeviation);
            }
        }
    }

    void verifyScrolledAreaSize(const Size &expectedSize) override
    {
        HWND contentContainerHwnd =
            cast<bdn::win32::ScrollViewCore>(_pScrollViewCore)
                ->getParentHwndForChildren();

        RECT winContentContainerRect;
        ::GetClientRect(contentContainerHwnd, &winContentContainerRect);

        Rect contentContainerRect = bdn::win32::win32RectToRect(
            winContentContainerRect, _pWin32Core->getUiScaleFactor());

        Size scrolledAreaSize = contentContainerRect.getSize();

        REQUIRE(Dip::equal(scrolledAreaSize, expectedSize));
    }

    void verifyViewPortSize(const Size &expectedSize) override
    {
        HWND windowHwnd = _pWin32Core->getHwnd();

        RECT winRect;
        ::GetWindowRect(windowHwnd, &winRect);

        Rect rect = bdn::win32::win32RectToRect(
            winRect, _pWin32Core->getUiScaleFactor());

        Size viewPortSize = rect.getSize();

        viewPortSize -= getNonClientSize();

        int style = GetWindowLong(windowHwnd, GWL_STYLE);

        if ((style & WS_HSCROLL) == WS_HSCROLL)
            viewPortSize.height -= getHorzBarHeight();

        if ((style & WS_VSCROLL) == WS_VSCROLL)
            viewPortSize.width -= getVertBarWidth();

        REQUIRE(Dip::equal(viewPortSize, expectedSize));
    }
};

TEST_CASE("win32.ScrollViewCore")
{
    P<TestWin32ScrollViewCore> pTest = newObj<TestWin32ScrollViewCore>();

    pTest->runTests();
}
