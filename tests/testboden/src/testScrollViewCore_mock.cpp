
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Dip.h>
#include <bdn/test/TestScrollViewCore.h>
#include <bdn/test/MockScrollViewCore.h>
#include "TestMockViewCoreMixin.h"

using namespace bdn;

class TestMockScrollViewCore : public bdn::test::TestMockViewCoreMixin<bdn::test::TestScrollViewCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMockViewCoreMixin<bdn::test::TestScrollViewCore>::initCore();

        _mockScrollViewCore = std::dynamic_pointer_cast<bdn::test::MockScrollViewCore>(_mockCore);
    }

    double getVertBarWidth() override { return 10; }

    double getHorzBarHeight() override { return 10; }

    Size initiateScrollViewResizeToHaveViewPortSize(const Size &viewPortSize) override
    {
        Size adjustedSize =
            _scrollView->adjustBounds(Rect(_scrollView->position, viewPortSize), RoundType::nearest, RoundType::nearest)
                .getSize();

        _scrollView->preferredSizeMinimum = (adjustedSize);
        _scrollView->preferredSizeMaximum = (adjustedSize);

        _window->requestAutoSize();

        return adjustedSize;
    }

    void verifyScrollsHorizontally(bool expectedVisible) override
    {
        REQUIRE(_mockScrollViewCore->getHorizontalScrollBarVisible() == expectedVisible);
    }

    void verifyScrollsVertically(bool expectedVisible) override
    {
        REQUIRE(_mockScrollViewCore->getVerticalScrollBarVisible() == expectedVisible);
    }

    void verifyContentViewBounds(const Rect &expectedBounds, double maxDeviation = 0) override
    {
        maxDeviation += Dip::significanceBoundary();

        std::shared_ptr<View> contentView = _scrollView->getContentView();

        if (contentView != nullptr) {
            Rect bounds(_scrollView->getContentView()->position, contentView->size);

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

    void verifyScrolledAreaSize(const Size &expectedSize) override
    {
        Size scrolledAreaSize = _mockScrollViewCore->getClientSize();

        REQUIRE(Dip::equal(scrolledAreaSize, expectedSize));
    }

    void verifyViewPortSize(const Size &expectedSize) override
    {
        Size viewPortSize = _mockScrollViewCore->getViewPortSize();

        REQUIRE(Dip::equal(viewPortSize, expectedSize));
    }

    std::shared_ptr<bdn::test::MockScrollViewCore> _mockScrollViewCore;
};

TEST_CASE("mock.ScrollViewCore")
{
    std::shared_ptr<TestMockScrollViewCore> test = std::make_shared<TestMockScrollViewCore>();

    test->runTests();
}
