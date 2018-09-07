#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/android/UiProvider.h>
#include <bdn/android/JScrollView.h>
#include <bdn/android/JHorizontalScrollView.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;

class TestAndroidScrollViewCore
    : public bdn::test::TestAndroidViewCoreMixin<bdn::test::TestScrollViewCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<
            bdn::test::TestScrollViewCore>::initCore();
    }

    double getVertBarWidth() override
    {
        // scroll bars are overlays that do not take up layout space
        return 0;
    }

    double getHorzBarHeight() override
    {
        // scroll bars are overlays that do not take up layout space
        return 0;
    }

    Size initiateScrollViewResizeToHaveViewPortSize(
        const Size &viewPortSize) override
    {
        // we cannot resize the scroll view directly with adjustAndSetBounds.
        // That would not have any effect outside of a layout cycle.
        // Instead we set the preferred size min and max to force the outer view
        // to resize it to the specified size.

        Size adjustedSize =
            _pScrollView
                ->adjustBounds(Rect(_pScrollView->position(), viewPortSize),
                               RoundType::nearest, RoundType::nearest)
                .getSize();

        _pScrollView->setPreferredSizeMinimum(adjustedSize);
        _pScrollView->setPreferredSizeMaximum(adjustedSize);

        // also request a re-layout here. With the normal propagation of the
        // property changes it would take two event cycles until the layout
        // happens. But we want it to happen immediately after the properties
        // have been changed.
        _pScrollView->getParentView()->needLayout(
            View::InvalidateReason::customDataChanged);

        return adjustedSize;
    }

    Size getScrollAreaSizePixels()
    {
        // the view will scroll if the width of the content area is bigger than
        // the width of the scroll view.

        // The Boden scroll view is made up of several android views:
        // - a vertical scroll view
        // - inside that a horizontal scroll view
        // - inside that a NativeViewGroup as the glue between the android
        // layout system and ours
        // - inside that the content view

        // the NativeViewGroup has the size of the scrolled area.

        bdn::android::JScrollView vertView(_jView.getRef_());
        REQUIRE(vertView.getChildCount() == 1);

        bdn::android::JHorizontalScrollView horzView(
            vertView.getChildAt(0).getRef_());
        REQUIRE(horzView.getChildCount() == 1);

        bdn::android::JNativeViewGroup scrollAreaView(
            horzView.getChildAt(0).getRef_());

        int scrollAreaWidth = scrollAreaView.getWidth();
        int scrollAreaHeight = scrollAreaView.getHeight();

        return Size(scrollAreaWidth, scrollAreaHeight);
    }

    void verifyScrollsHorizontally(bool expectedScrolls) override
    {
        Size scrollAreaSizePixels = getScrollAreaSizePixels();

        int viewWidth = _jView.getWidth();

        double scrolls = (scrollAreaSizePixels.width > viewWidth);
        REQUIRE(scrolls == expectedScrolls);
    }

    void verifyScrollsVertically(bool expectedScrolls) override
    {
        Size scrollAreaSizePixels = getScrollAreaSizePixels();

        int viewHeight = _jView.getHeight();

        double scrolls = (scrollAreaSizePixels.height > viewHeight);
        REQUIRE(scrolls == expectedScrolls);
    }

    void verifyContentViewBounds(const Rect &expectedBounds,
                                 double maxDeviation = 0) override
    {
        maxDeviation += Dip::significanceBoundary();

        P<View> pContentView = _pScrollView->getContentView();

        if (pContentView != nullptr) {
            bdn::Rect bounds(_pScrollView->getContentView()->position(),
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
        Size scrollAreaSizePixels = getScrollAreaSizePixels();

        double scaleFactor = _pAndroidViewCore->getUiScaleFactor();

        Size scrollAreaSize(scrollAreaSizePixels.width / scaleFactor,
                            scrollAreaSizePixels.height / scaleFactor);

        /*REQUIRE_ALMOST_EQUAL( scrollAreaSize.width, expectedSize.width,
        (1.0/scaleFactor)+Dip::significanceBoundary() ); REQUIRE_ALMOST_EQUAL(
        scrollAreaSize.height, expectedSize.height,
        (1.0/scaleFactor)+Dip::significanceBoundary() );*/

        REQUIRE(Dip::equal(scrollAreaSize, expectedSize));
    }

    void verifyViewPortSize(const Size &expectedSize) override
    {
        double scaleFactor = _pAndroidViewCore->getUiScaleFactor();

        Size viewPortSize(_jView.getWidth() / scaleFactor,
                          _jView.getHeight() / scaleFactor);

        /*REQUIRE_ALMOST_EQUAL( viewPortSize.width, expectedSize.width,
        (1.0/scaleFactor)+Dip::significanceBoundary() ); REQUIRE_ALMOST_EQUAL(
        viewPortSize.height, expectedSize.height,
        (1.0/scaleFactor)+Dip::significanceBoundary() );*/

        REQUIRE(Dip::equal(viewPortSize, expectedSize));
    }
};

TEST_CASE("android.ScrollViewCore")
{
    P<TestAndroidScrollViewCore> pTest = newObj<TestAndroidScrollViewCore>();

    pTest->runTests();
}
