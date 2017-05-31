#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ScrollViewLayoutHelper.h>
#include <bdn/ScrollView.h>

#include <bdn/test/MockUiProvider.h>

using namespace bdn;

static void testScrollViewLayoutHelperPreferredSize()
{
    SECTION("scrollview null")
    {
        ScrollViewLayoutHelper helper(7, 13);

        Size prefSize = helper.calcPreferredSize(nullptr, Size::none());
        REQUIRE( prefSize == Size(0,0) );
    }

    P<bdn::test::MockUiProvider> pUiProvider = newObj<bdn::test::MockUiProvider>();
    P<Window>                    pWindow = newObj<Window>( pUiProvider );

    P<ScrollView> pScrollView = newObj<ScrollView>();

    pScrollView->horizontalScrollingEnabled() = true;
    pScrollView->verticalScrollingEnabled() = true;

    pWindow->setContentView(pScrollView);

    CONTINUE_SECTION_WHEN_IDLE(pWindow, pScrollView)
    {    
        SECTION("contentview null")
        {
            ScrollViewLayoutHelper helper(7, 13);

            SECTION("no padding")
            {
                Size prefSize = helper.calcPreferredSize(pScrollView, Size::none());
                REQUIRE( prefSize == Size(0,0) );            
            }

            SECTION("with padding")
            {
                pScrollView->padding() = UiMargin(5,6,7,8);

                Size prefSize = helper.calcPreferredSize(pScrollView, Size::none());
                REQUIRE( prefSize == Size(6+8, 5+7) );

                REQUIRE( helper.getHorizontalScrollBarVisible() == false);
                REQUIRE( helper.getVerticalScrollBarVisible() == false);
            }

            SECTION("with padding and scrollview margin")
            {
                // the scrollview's own margin should not matter - only the margin of the content view
                pScrollView->margin() = UiMargin(1,2,3,4);
                pScrollView->padding() = UiMargin(5,6,7,8);

                Size prefSize = helper.calcPreferredSize(pScrollView, Size::none());
                REQUIRE( prefSize == Size(6+8, 5+7) );

                REQUIRE( helper.getHorizontalScrollBarVisible() == false);
                REQUIRE( helper.getVerticalScrollBarVisible() == false);
            }
        }

        SECTION("contentview not null")
        {
            P<Button> pButton = newObj<Button>();
            pScrollView->setContentView(pButton);

            pButton->margin() = UiMargin(1,2,3,4);
            pScrollView->padding() = UiMargin(5,6,7,8);

            CONTINUE_SECTION_WHEN_IDLE( pButton, pScrollView, pWindow)
            {
                ScrollViewLayoutHelper helper(7, 13);

                Size buttonSize = pButton->calcPreferredSize();

                Size optimalSize = Size(2+4+6+8, 1+3+5+7) + buttonSize;

                SECTION("unlimited space")
                {
                    // should request as much space as the content needs by default
        
                    Size prefSize = helper.calcPreferredSize(pScrollView, Size::none());
                    REQUIRE( prefSize == optimalSize );
                }

                SECTION("more than enough space")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(100,100) );
                    REQUIRE( prefSize == optimalSize );
                }

                SECTION("exactly enough space")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize );
                    REQUIRE( prefSize == optimalSize );
                }

                SECTION("less width than needed, more than enough height for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(-1, 1000) );
                    REQUIRE( prefSize == optimalSize + Size(-1, 7) );
                }

                SECTION("less width than needed, enough height for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(-1, 7) );
                    REQUIRE( prefSize == optimalSize + Size(-1, 7) );
                }

                SECTION("less width than needed, not enough height for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(-1, 6) );

                    // this should cause both scrollbars to be shown. We should use all the available space
                    REQUIRE( prefSize == optimalSize + Size(-1, 6) );
                }

                SECTION("less height than needed, more than enough width for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(1000, -1) );
                    REQUIRE( prefSize == optimalSize + Size(13, -1) );
                }


                SECTION("less height than needed, enough width for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(13, -1) );
                    REQUIRE( prefSize == optimalSize + Size(13, -1) );
                }

                SECTION("less height than needed, not enough width for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(12, -1) );
                    REQUIRE( prefSize == optimalSize + Size(12, -1) );
                }

                SECTION("less width and less height than needed")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(-1, -1) );
                    REQUIRE( prefSize == optimalSize + Size(-1, -1) );
                }
            };
        }
    };
}

static void testScrollViewLayoutHelperLayout()
{
    SECTION("scrollview null")
    {
        ScrollViewLayoutHelper helper(7, 13);

        helper.calcLayout(nullptr, Size(1000, 1000) );

        REQUIRE( helper.getHorizontalScrollBarVisible() == false );
        REQUIRE( helper.getVerticalScrollBarVisible() == false );
        REQUIRE( helper.getContentViewBounds() == Rect(0, 0, 1000, 1000) );
        REQUIRE( helper.getScrolledAreaSize() == Size(1000, 1000) );
        REQUIRE( helper.getViewPortSize() == Size(1000, 1000) );
    }

    P<bdn::test::MockUiProvider> pUiProvider = newObj<bdn::test::MockUiProvider>();
    P<Window>                    pWindow = newObj<Window>( pUiProvider );

    P<ScrollView> pScrollView = newObj<ScrollView>();
    pWindow->setContentView(pScrollView);

    pScrollView->horizontalScrollingEnabled() = true;
    pScrollView->verticalScrollingEnabled() = true;
    
    SECTION("contentview null")
    {
        SECTION("no margin, no padding")
        {
            ScrollViewLayoutHelper helper(7, 13);

            helper.calcLayout(pScrollView, Size(1000, 1000) );

            REQUIRE( helper.getHorizontalScrollBarVisible() == false );
            REQUIRE( helper.getVerticalScrollBarVisible() == false );
            REQUIRE( helper.getContentViewBounds() == Rect(0, 0, 1000, 1000) );
            REQUIRE( helper.getScrolledAreaSize() == Size(1000, 1000) );
            REQUIRE( helper.getViewPortSize() == Size(1000, 1000) );
        }

        SECTION("padding")
        {
            pScrollView->padding() = UiMargin(1,2,3,4);

            // the scrollview's margin should not influence the layout
            pScrollView->margin() = UiMargin(5,6,7,8);

            CONTINUE_SECTION_WHEN_IDLE(pWindow, pScrollView)
            {
                ScrollViewLayoutHelper helper(7, 13);

                helper.calcLayout(pScrollView, Size(1000, 1000) );

                REQUIRE( helper.getHorizontalScrollBarVisible() == false );
                REQUIRE( helper.getVerticalScrollBarVisible() == false );
                REQUIRE( helper.getContentViewBounds() == Rect(4, 1, 1000-2-4, 1000-1-3) );
                REQUIRE( helper.getScrolledAreaSize() == Size(1000, 1000) );
                REQUIRE( helper.getViewPortSize() == Size(1000, 1000) );
            };
        }
    }

    SECTION("contentview not null")
    {
        P<Button> pButton = newObj<Button>();
        pScrollView->setContentView(pButton);

        pButton->margin() = UiMargin(1,2,3,4);
        pScrollView->padding() = UiMargin(5,6,7,8);

        CONTINUE_SECTION_WHEN_IDLE(pWindow, pScrollView, pButton)
        {        
            ScrollViewLayoutHelper helper(7, 13);

            Size buttonSize = pButton->calcPreferredSize();
        
            Size optimalSize = Size(2+4+6+8, 1+3+5+7) + buttonSize;

            Rect optimalButtonBounds( Point(4+8, 1+5), buttonSize);

            SECTION("huge viewport")
            {
                Size viewPortSize = optimalSize+Size(1000, 1000);

                helper.calcLayout(pScrollView, viewPortSize );

                // content view should be stretched to fill whole viewport
                REQUIRE( helper.getHorizontalScrollBarVisible() == false );
                REQUIRE( helper.getVerticalScrollBarVisible() == false );
                REQUIRE( helper.getContentViewBounds() == Rect(4+8, 1+5, viewPortSize.width - 2-4-6-8, viewPortSize.height -1-3-5-7) );
                REQUIRE( helper.getScrolledAreaSize() == viewPortSize );
                REQUIRE( helper.getViewPortSize() == viewPortSize );
            }
        
            SECTION("viewport matches needed content size")
            {
                helper.calcLayout(pScrollView, optimalSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == false );
                REQUIRE( helper.getVerticalScrollBarVisible() == false );
                REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
                REQUIRE( helper.getScrolledAreaSize() == optimalSize );
                REQUIRE( helper.getViewPortSize() == optimalSize );
            }

            SECTION("less width than needed, more than enough height for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(-1, 1000);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == true );
                REQUIRE( helper.getVerticalScrollBarVisible() == false );

                Rect contentViewBounds = helper.getContentViewBounds();
                Rect expectedContentViewBounds = Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(0, 1000-7) );
                // we allow slight deviation due to floating point calculations
                REQUIRE_ALMOST_EQUAL(  contentViewBounds.x, expectedContentViewBounds.x, 0.0001 );
                REQUIRE_ALMOST_EQUAL(  contentViewBounds.y, expectedContentViewBounds.y, 0.0001 );
                REQUIRE_ALMOST_EQUAL(  contentViewBounds.width, expectedContentViewBounds.width, 0.0001 );
                REQUIRE_ALMOST_EQUAL(  contentViewBounds.height, expectedContentViewBounds.height, 0.0001 );
            
                // scrollable area is the optimal width. The height is the viewport height minus the height of the horizontal scrollbar
                REQUIRE( helper.getScrolledAreaSize() == Size(optimalSize.width, viewPortSize.height-7) );
                // horizontal scroll bar is visible, so the final viewport size should be smaller by that amount
                REQUIRE( helper.getViewPortSize() == viewPortSize-Size(0, 7) );
            }

            SECTION("less width than needed, enough height for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(-1, 7);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == true );
                REQUIRE( helper.getVerticalScrollBarVisible() == false );
                REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                // scrollable area is the optimal size
                REQUIRE( helper.getScrolledAreaSize() == optimalSize );

                // horizontal scroll bar is visible, so the final viewport size should be smaller by that amount
                REQUIRE( helper.getViewPortSize() == viewPortSize-Size(0, 7) );
            }

            SECTION("less width than needed, not enough height for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(-1, 6);

                helper.calcLayout(pScrollView, viewPortSize );

                // the horizontal scroll bar does not fit. So we should also get a vertical scrollbar
                REQUIRE( helper.getHorizontalScrollBarVisible() == true );
                REQUIRE( helper.getVerticalScrollBarVisible() == true );
                REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                // scrollable area is the optimal size
                REQUIRE( helper.getScrolledAreaSize() == optimalSize );

                // both scroll bars are visible, so the final viewport size should be smaller by that amount
                REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 7) );
            }

            SECTION("less height than needed, more than enough width for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(1000, -1);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == false );
                REQUIRE( helper.getVerticalScrollBarVisible() == true );
                REQUIRE( helper.getContentViewBounds() == Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(1000-13, 0) ) );
            
                // scrollable area width is the viewport width minus the scrollbar width. Height is the optimal height
                REQUIRE( helper.getScrolledAreaSize() == Size(viewPortSize.width-13, optimalSize.height) );
                // vertical scroll bar is visible, so the final viewport size should be smaller by that amount
                REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 0) );
            }


            SECTION("less height than needed, enough width for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(13, -1);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == false );
                REQUIRE( helper.getVerticalScrollBarVisible() == true );
                REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                // scrollable area size is the optimal size
                REQUIRE( helper.getScrolledAreaSize() == optimalSize );
                // vertical scroll bar is visible, so the final viewport size should be smaller by that amount
                REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 0) );
            }

            SECTION("less height than needed, not enough width for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(12, -1);

                helper.calcLayout(pScrollView, viewPortSize );

                // horizontal scrollbar should be visible now as well
                REQUIRE( helper.getHorizontalScrollBarVisible() == true );
                REQUIRE( helper.getVerticalScrollBarVisible() == true );
                REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                // scrollable area size is the optimal size
                REQUIRE( helper.getScrolledAreaSize() == optimalSize );
                // both scroll bars are visible, so the final viewport size should be smaller by that amount
                REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 7) );
            }

            SECTION("less width and less height than needed")
            {
                Size viewPortSize = optimalSize + Size(-1, -1);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == true );
                REQUIRE( helper.getVerticalScrollBarVisible() == true );
                REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                REQUIRE( helper.getScrolledAreaSize() == optimalSize );
                REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 7) );
            }
        };
    }
}

TEST_CASE("ScrollViewLayoutHelper")
{
    SECTION("calcPreferredSize")
        testScrollViewLayoutHelperPreferredSize();

    SECTION("layout")
        testScrollViewLayoutHelperLayout();
}

