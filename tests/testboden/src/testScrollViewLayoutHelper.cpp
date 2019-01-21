
#include <bdn/test.h>

#include <bdn/ScrollViewLayoutHelper.h>
#include <bdn/ScrollView.h>
#include <bdn/Dip.h>

#include <bdn/test/MockUiProvider.h>
#include <bdn/test/ScrollViewLayoutTesterBase.h>

using namespace bdn;

class ScrollViewLayoutHelperTester : public bdn::test::ScrollViewLayoutTesterBase<Base>
{
  public:
    ScrollViewLayoutHelperTester() : _helper(13, 7)
    {
        _uiProvider = std::make_shared<bdn::test::MockUiProvider>();
        _window = std::make_shared<Window>(_uiProvider);

        _scrollView = std::make_shared<ScrollView>();

        _window->setContentView(_scrollView);
    }

    std::shared_ptr<bdn::test::MockUiProvider> _uiProvider;
    std::shared_ptr<Window> _window;
    std::shared_ptr<ScrollView> _scrollView;

    ScrollViewLayoutHelper _helper;

    std::shared_ptr<ScrollView> getScrollView() override { return _scrollView; }

    double getVertBarWidth() override { return 13; }

    double getHorzBarHeight() override { return 7; }

    Size callCalcPreferredSize(const Size &availableSpace = Size::none()) override
    {
        return _helper.calcPreferredSize(_scrollView, availableSpace);
    }

    Size prepareCalcLayout(const Size &viewPortSize) override
    {
        _prepareCalcLayoutViewPortSize =
            _scrollView->adjustBounds(Rect(_scrollView->position, viewPortSize), RoundType::nearest, RoundType::nearest)
                .getSize();

        // the adjusted size should be roughly the same
        REQUIRE(_prepareCalcLayoutViewPortSize >= viewPortSize - Size(5, 5));
        REQUIRE(_prepareCalcLayoutViewPortSize <= viewPortSize + Size(5, 5));

        return _prepareCalcLayoutViewPortSize;
    }

    void calcLayoutAfterPreparation() override { _helper.calcLayout(_scrollView, _prepareCalcLayoutViewPortSize); }

    void verifyScrollsHorizontally(bool expectedScrolls) override
    {
        REQUIRE(_helper.getHorizontalScrollBarVisible() == expectedScrolls);
    }

    void verifyScrollsVertically(bool expectedScrolls) override
    {
        REQUIRE(_helper.getVerticalScrollBarVisible() == expectedScrolls);
    }

    void verifyContentViewBounds(const Rect &expectedBounds, double maxDeviation = 0) override
    {
        maxDeviation += Dip::significanceBoundary();

        if (maxDeviation == 0)
            REQUIRE(_helper.getContentViewBounds() == expectedBounds);
        else {
            Rect contentViewBounds = _helper.getContentViewBounds();

            REQUIRE_ALMOST_EQUAL(contentViewBounds.x, expectedBounds.x, maxDeviation);
            REQUIRE_ALMOST_EQUAL(contentViewBounds.y, expectedBounds.y, maxDeviation);
            REQUIRE_ALMOST_EQUAL(contentViewBounds.width, expectedBounds.width, maxDeviation);
            REQUIRE_ALMOST_EQUAL(contentViewBounds.height, expectedBounds.height, maxDeviation);
        }
    }

    void verifyScrolledAreaSize(const Size &expectedSize) override
    {
        REQUIRE(Dip::equal(_helper.getScrolledAreaSize(), expectedSize));
    }

    void verifyViewPortSize(const Size &expectedSize) override
    {
        REQUIRE(Dip::equal(_helper.getViewPortSize(), expectedSize));
    }

    void doPreferredSizeAndLayoutTests() override
    {
        SECTION("scrollview null")
        {
            SECTION("preferred size")
            {
                ScrollViewLayoutHelper helper(13, 7);

                Size prefSize = helper.calcPreferredSize(nullptr, Size::none());
                REQUIRE(prefSize == Size(0, 0));
            }

            SECTION("layout")
            {
                ScrollViewLayoutHelper helper(13, 7);

                helper.calcLayout(nullptr, Size(1000, 1000));

                REQUIRE(helper.getHorizontalScrollBarVisible() == false);
                REQUIRE(helper.getVerticalScrollBarVisible() == false);
                REQUIRE(helper.getContentViewBounds() == Rect(0, 0, 1000, 1000));
                REQUIRE(helper.getScrolledAreaSize() == Size(1000, 1000));
                REQUIRE(helper.getViewPortSize() == Size(1000, 1000));
            }
        }

        SECTION("scrollview not null") { ScrollViewLayoutTesterBase<Base>::doPreferredSizeAndLayoutTests(); }
    }

  private:
    Size _prepareCalcLayoutViewPortSize;
};

TEST_CASE("ScrollViewLayoutHelper")
{
    std::shared_ptr<ScrollViewLayoutHelperTester> tester = std::make_shared<ScrollViewLayoutHelperTester>();

    tester->doPreferredSizeAndLayoutTests();
}
