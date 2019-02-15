
#include <bdn/test.h>

#import "TestIosViewCoreMixin.hh"
#include <bdn/Dip.h>
#include <bdn/Window.h>
#include <bdn/test/TestScrollViewCore.h>

#import <bdn/ios/util.hh>

#import <bdn/ios/UIProvider.hh>

#import <bdn/ios/ScrollViewCore.hh>

using namespace bdn;

class TestIosScrollViewCore : public bdn::test::TestIosViewCoreMixin<bdn::test::TestScrollViewCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestIosViewCoreMixin<bdn::test::TestScrollViewCore>::initCore();

        _uiScrollView = (UIScrollView *)_uiView;
    }

    double getVertBarWidth() override
    {
        // scroll bars are only shown during scrolling as an overlay.
        // So they do not take up space in layout.
        return 0;
    }

    double getHorzBarHeight() override
    {
        // scroll bars are only shown during scrolling as an overlay.
        // So they do not take up space in layout.
        return 0;
    }

    bdn::Size getNonClientSize()
    {
        // our scroll views do not have a border.
        return bdn::Size(0, 0);
    }

    bdn::Size initiateScrollViewResizeToHaveViewPortSize(const bdn::Size &viewPortSize) override
    {
        bdn::Size viewSize = viewPortSize + getNonClientSize();

        viewSize =
            _scrollView
                ->adjustBounds(bdn::Rect(_scrollView->position, viewSize), RoundType::nearest, RoundType::nearest)
                .getSize();

        // we cannot resize the scroll view directly with adjustAndSetBounds.
        // That would not have any effect outside of a layout cycle.
        // Instead we set the preferred size min and max to force the outer view
        // to resize it to the specified size.

        _scrollView->preferredSizeMinimum = (viewSize);
        _scrollView->preferredSizeMaximum = (viewSize);

        // also request a re-layout here. With the normal propagation of the
        // property changes it would take two event cycles until the layout
        // happens. But we want it to happen immediately after the properties
        // have been changed.
        _scrollView->getParentView()->needLayout(View::InvalidateReason::customDataChanged);

        return viewSize;
    }

    void verifyScrollsHorizontally(bool expectedScrolls) override
    {
        // To check whether the view actually scrolls we have to check if the
        // content size is bigger than the view size.

        CGSize viewPortSize = _uiScrollView.frame.size;
        CGSize contentSize = _uiScrollView.contentSize;

        bool scrolls = (contentSize.width > viewPortSize.width && _uiScrollView.scrollEnabled);

        REQUIRE(scrolls == expectedScrolls);
    }

    void verifyScrollsVertically(bool expectedScrolls) override
    {
        // To check whether the view actually scrolls we have to check if the
        // content size is bigger than the view size.
        CGSize viewPortSize = _uiScrollView.frame.size;
        CGSize contentSize = _uiScrollView.contentSize;

        bool scrolls = (contentSize.height > viewPortSize.height && _uiScrollView.scrollEnabled);

        REQUIRE(scrolls == expectedScrolls);
    }

    void verifyContentViewBounds(const bdn::Rect &expectedBounds, double maxDeviation = 0) override
    {
        maxDeviation += Dip::significanceBoundary();

        std::shared_ptr<View> contentView = _scrollView->getContentView();

        if (contentView != nullptr) {
            bdn::Rect bounds(_scrollView->getContentView()->position, contentView->size);

            if (maxDeviation == 0)
                REQUIRE(bounds == expectedBounds);
            else {
                REQUIRE_ALMOST_EQUAL(bounds.x, expectedBounds.x, maxDeviation);
                REQUIRE_ALMOST_EQUAL(bounds.y, expectedBounds.y, maxDeviation);
                REQUIRE_ALMOST_EQUAL(bounds.width, expectedBounds.width, maxDeviation);
                REQUIRE_ALMOST_EQUAL(bounds.height, expectedBounds.height, maxDeviation);
            }
        }
    }

    void verifyScrolledAreaSize(const bdn::Size &expectedSize) override
    {
        bdn::Size scrolledAreaSize = bdn::ios::iosSizeToSize(_uiScrollView.contentSize);
        bdn::Size viewPortSize = bdn::ios::iosSizeToSize(_uiScrollView.frame.size);

        scrolledAreaSize.applyMinimum(viewPortSize);

        REQUIRE(Dip::equal(scrolledAreaSize, expectedSize));
    }

    void verifyViewPortSize(const bdn::Size &expectedSize) override
    {
        bdn::Size viewPortSize = bdn::ios::iosSizeToSize(_uiScrollView.frame.size);

        REQUIRE(Dip::equal(viewPortSize, expectedSize));
    }

  private:
    UIScrollView *_uiScrollView;
};

TEST_CASE("ios.ScrollViewCore")
{
    std::shared_ptr<TestIosScrollViewCore> test = std::make_shared<TestIosScrollViewCore>();

    test->runTests();
}
