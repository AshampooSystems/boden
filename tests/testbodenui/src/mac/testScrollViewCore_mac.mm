#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/Dip.h>
#include <bdn/test/TestScrollViewCore.h>
#import "TestMacChildViewCoreMixin.hh"

#import <bdn/mac/util.hh>

#import <bdn/mac/UiProvider.hh>

#import <bdn/mac/ScrollViewCore.hh>

using namespace bdn;

class TestMacScrollViewCore
    : public bdn::test::TestMacChildViewCoreMixin<bdn::test::TestScrollViewCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestMacChildViewCoreMixin<
            bdn::test::TestScrollViewCore>::initCore();

        _nsScrollView = (NSScrollView *)_pNSView;
    }

    bdn::Size getScrollerSize()
    {
        bdn::Size frameSize(500, 500);
        NSSize macFrameSize = bdn::mac::sizeToMacSize(frameSize);

        NSSize macSizeWithScrollers =
            [NSScrollView contentSizeForFrameSize:macFrameSize
                          horizontalScrollerClass:[NSScroller class]
                            verticalScrollerClass:[NSScroller class]
                                       borderType:_nsScrollView.borderType
                                      controlSize:NSControlSizeRegular
                                    scrollerStyle:_nsScrollView.scrollerStyle];

        NSSize macSizeWithoutScrollers =
            [NSScrollView contentSizeForFrameSize:macFrameSize
                          horizontalScrollerClass:nil
                            verticalScrollerClass:nil
                                       borderType:_nsScrollView.borderType
                                      controlSize:NSControlSizeRegular
                                    scrollerStyle:_nsScrollView.scrollerStyle];

        bdn::Size sizeWithScrollers =
            bdn::mac::macSizeToSize(macSizeWithScrollers);
        bdn::Size sizeWithoutScrollers =
            bdn::mac::macSizeToSize(macSizeWithoutScrollers);

        return sizeWithoutScrollers - sizeWithScrollers;
    }

    double getVertBarWidth() override { return getScrollerSize().width; }

    double getHorzBarHeight() override { return getScrollerSize().height; }

    bdn::Size getNonClientSize()
    {
        // our scroll views do not have a border.
        return bdn::Size(0, 0);
    }

    bdn::Size initiateScrollViewResizeToHaveViewPortSize(
        const bdn::Size &viewPortSize) override
    {
        bdn::Size viewSize = viewPortSize + getNonClientSize();

        viewSize =
            _pScrollView
                ->adjustBounds(bdn::Rect(_pScrollView->position(), viewSize),
                               RoundType::nearest, RoundType::nearest)
                .getSize();

        // we cannot resize the scroll view directly with adjustAndSetBounds.
        // That would not have any effect outside of a layout cycle.
        // Instead we set the preferred size min and max to force the outer view
        // to resize it to the specified size.

        _pScrollView->setPreferredSizeMinimum(viewSize);
        _pScrollView->setPreferredSizeMaximum(viewSize);

        _pWindow->requestAutoSize();

        return viewSize;
    }

    void verifyScrollsHorizontally(bool expectedScrolls) override
    {
        // the scroller object always exists if scrolling is generally enabled.
        // To check whether the view actually scrolls we have to check if the
        // content size is bigger than the view size.
        NSSize viewPortSize = _nsScrollView.contentSize;
        NSSize docSize{0, 0};

        if (_nsScrollView.documentView != nil)
            docSize = _nsScrollView.documentView.frame.size;

        bool scrolls = (docSize.width > viewPortSize.width);

        REQUIRE(scrolls == expectedScrolls);
    }

    void verifyScrollsVertically(bool expectedScrolls) override
    {
        // the scroller object always exists if scrolling is generally enabled.
        // To check whether the view actually scrolls we have to check if the
        // content size is bigger than the view size.
        NSSize viewPortSize = _nsScrollView.contentSize;
        NSSize docSize{0, 0};

        if (_nsScrollView.documentView != nil)
            docSize = _nsScrollView.documentView.frame.size;

        bool scrolls = (docSize.height > viewPortSize.height);

        REQUIRE(scrolls == expectedScrolls);
    }

    void verifyContentViewBounds(const bdn::Rect &expectedBounds,
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

    void verifyScrolledAreaSize(const bdn::Size &expectedSize) override
    {
        bdn::Size scrolledAreaSize =
            bdn::mac::macSizeToSize(_nsScrollView.documentView.frame.size);

        bdn::Size viewPortSize =
            bdn::mac::macSizeToSize(_nsScrollView.contentSize);

        scrolledAreaSize.applyMinimum(viewPortSize);

        REQUIRE(Dip::equal(scrolledAreaSize, expectedSize));
    }

    void verifyViewPortSize(const bdn::Size &expectedSize) override
    {
        // NSScrollView refers to the viewport as the "content view".
        // The actual inner view with the scrolled content is the "document
        // view".
        bdn::Size viewPortSize =
            bdn::mac::macSizeToSize(_nsScrollView.contentSize);

        REQUIRE(Dip::equal(viewPortSize, expectedSize));
    }

  private:
    NSScrollView *_nsScrollView;
};

void printScrollerStyle()
{
    static bool first = true;

    if (first) {
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSLog(@"System Scroller Style: %@",
              [defaults stringForKey:@"AppleShowScrollBars"]);
        first = false;
    }
}

TEST_CASE("mac.ScrollViewCore")
{
    printScrollerStyle();

    P<TestMacScrollViewCore> pTest = newObj<TestMacScrollViewCore>();
    pTest->runTests();
}
