#include <bdn/init.h>
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

        _pMockScrollViewCore = cast<bdn::test::MockScrollViewCore>(_pMockCore);
    }

    double getVertBarWidth() override { return 10; }

    double getHorzBarHeight() override { return 10; }

    Size initiateScrollViewResizeToHaveViewPortSize(const Size &viewPortSize) override
    {
        Size adjustedSize =
            _pScrollView
                ->adjustBounds(Rect(_pScrollView->position(), viewPortSize), RoundType::nearest, RoundType::nearest)
                .getSize();

        _pScrollView->setPreferredSizeMinimum(adjustedSize);
        _pScrollView->setPreferredSizeMaximum(adjustedSize);

        _pWindow->requestAutoSize();

        return adjustedSize;
    }

    void verifyScrollsHorizontally(bool expectedVisible) override
    {
        REQUIRE(_pMockScrollViewCore->getHorizontalScrollBarVisible() == expectedVisible);
    }

    void verifyScrollsVertically(bool expectedVisible) override
    {
        REQUIRE(_pMockScrollViewCore->getVerticalScrollBarVisible() == expectedVisible);
    }

    void verifyContentViewBounds(const Rect &expectedBounds, double maxDeviation = 0) override
    {
        maxDeviation += Dip::significanceBoundary();

        P<View> pContentView = _pScrollView->getContentView();

        if (pContentView != nullptr) {
            Rect bounds(_pScrollView->getContentView()->position(), pContentView->size());

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
        Size scrolledAreaSize = _pMockScrollViewCore->getClientSize();

        REQUIRE(Dip::equal(scrolledAreaSize, expectedSize));
    }

    void verifyViewPortSize(const Size &expectedSize) override
    {
        Size viewPortSize = _pMockScrollViewCore->getViewPortSize();

        REQUIRE(Dip::equal(viewPortSize, expectedSize));
    }

    P<bdn::test::MockScrollViewCore> _pMockScrollViewCore;
};

TEST_CASE("mock.ScrollViewCore")
{
    P<TestMockScrollViewCore> pTest = newObj<TestMockScrollViewCore>();

    pTest->runTests();
}
