#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ScrollViewLayoutHelper.h>
#include <bdn/ScrollView.h>

#include <bdn/test/MockUiProvider.h>

using namespace bdn;

static void testScrollViewLayoutHelperPreferredSize(bool horzScrollingEnabled, bool vertScrollingEnabled)
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

    pScrollView->horizontalScrollingEnabled() = horzScrollingEnabled;
    pScrollView->verticalScrollingEnabled() = vertScrollingEnabled;

    pWindow->setContentView(pScrollView);

    CONTINUE_SECTION_WHEN_IDLE(pWindow, pScrollView, horzScrollingEnabled, vertScrollingEnabled)
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
            }

            SECTION("with padding and scrollview margin")
            {
                // the scrollview's own margin should not matter - only the margin of the content view
                pScrollView->margin() = UiMargin(1,2,3,4);
                pScrollView->padding() = UiMargin(5,6,7,8);

                Size prefSize = helper.calcPreferredSize(pScrollView, Size::none());
                REQUIRE( prefSize == Size(6+8, 5+7) );
            }
        }

        SECTION("contentview not null")
        {
            P<Button> pButton = newObj<Button>();
            pScrollView->setContentView(pButton);

            pButton->margin() = UiMargin(1,2,3,4);
            pScrollView->padding() = UiMargin(5,6,7,8);

            CONTINUE_SECTION_WHEN_IDLE( pButton, pScrollView, pWindow, horzScrollingEnabled, vertScrollingEnabled)
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

                    if(horzScrollingEnabled)
                    {
                        // preferred width should be the available width. preferred height should include
                        // the scrollbar
                        REQUIRE( prefSize == optimalSize + Size(-1, 7) );
                    }
                    else
                    {
                        // no horz scrolling => no additional scrollbar at bottom
                        REQUIRE( prefSize == optimalSize + Size(-1, 0) );
                    }
                }

                SECTION("less width than needed, enough height for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(-1, 7) );

                    if(horzScrollingEnabled)
                    {
                        // preferred width should be the available width. preferred height should include
                        // the scrollbar
                        REQUIRE( prefSize == optimalSize + Size(-1, 7) );
                    }
                    else
                    {
                        // no horz scrolling => no additional scrollbar at bottom
                        REQUIRE( prefSize == optimalSize + Size(-1, 0) );
                    }
                }

                SECTION("less width than needed, not enough height for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(-1, 6) );

                    if(horzScrollingEnabled && vertScrollingEnabled)
                    {
                        // this should cause both scrollbars to be shown. We should use all the available space
                        REQUIRE( prefSize == optimalSize + Size(-1, 6) );
                    }
                    else if(horzScrollingEnabled)
                    {
                        // no vertical scrolling. We will get a horizontal scrollbar, but no vertical scrollbar.
                        // We will still fill all available space.
                        REQUIRE( prefSize == optimalSize + Size(-1, 6) );
                    }
                    else
                    {
                        // no horz scrolling => no additional scrollbar at bottom
                        REQUIRE( prefSize == optimalSize + Size(-1, 0) );
                    }                    
                }

                SECTION("less height than needed, more than enough width for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(1000, -1) );

                    if(vertScrollingEnabled)
                    {
                        REQUIRE( prefSize == optimalSize + Size(13, -1) );
                    }
                    else
                    {
                        // no vert scrolling => no scrollbar added
                        REQUIRE( prefSize == optimalSize + Size(0, -1) );
                    }                    
                }


                SECTION("less height than needed, enough width for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(13, -1) );

                    if(vertScrollingEnabled)
                    {
                        REQUIRE( prefSize == optimalSize + Size(13, -1) );
                    }
                    else
                    {
                        // no vert scrolling => no scrollbar added
                        REQUIRE( prefSize == optimalSize + Size(0, -1) );
                    }
                }

                SECTION("less height than needed, not enough width for scrollbar")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(12, -1) );

                    if(vertScrollingEnabled)
                    {
                        REQUIRE( prefSize == optimalSize + Size(12, -1) );
                    }
                    else
                    {
                        // no vert scrolling => no scrollbar added
                        REQUIRE( prefSize == optimalSize + Size(0, -1) );
                    }
                    
                }

                SECTION("less width and less height than needed")
                {
                    Size prefSize = helper.calcPreferredSize(pScrollView, optimalSize + Size(-1, -1) );

                    // it does not matter wether or not scrolling is enabled. In all cases
                    // the scroll view should simply use all available space.
                    REQUIRE( prefSize == optimalSize + Size(-1, -1) );                    
                }
            };
        }
    };
}

static void testScrollViewLayoutHelperLayout(bool horzScrollingEnabled, bool vertScrollingEnabled)
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

    pScrollView->horizontalScrollingEnabled() = horzScrollingEnabled;
    pScrollView->verticalScrollingEnabled() = vertScrollingEnabled;
    
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
        

        CONTINUE_SECTION_WHEN_IDLE(pWindow, pScrollView, pButton, horzScrollingEnabled, vertScrollingEnabled)
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

                REQUIRE( helper.getHorizontalScrollBarVisible() == horzScrollingEnabled );
                REQUIRE( helper.getVerticalScrollBarVisible() == false );

                Rect contentViewBounds = helper.getContentViewBounds();
                Rect expectedContentViewBounds;

                if(horzScrollingEnabled)
                    expectedContentViewBounds = Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(0, 1000-7) );
                else
                {
                    // no scrolling >= no scrollbar. Also width is truncated
                    expectedContentViewBounds = Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-1, 1000) );
                }                
                                    
                // we allow slight deviation due to floating point calculations
                REQUIRE_ALMOST_EQUAL(  contentViewBounds.x, expectedContentViewBounds.x, 0.0001 );
                REQUIRE_ALMOST_EQUAL(  contentViewBounds.y, expectedContentViewBounds.y, 0.0001 );
                REQUIRE_ALMOST_EQUAL(  contentViewBounds.width, expectedContentViewBounds.width, 0.0001 );
                REQUIRE_ALMOST_EQUAL(  contentViewBounds.height, expectedContentViewBounds.height, 0.0001 );

                if(horzScrollingEnabled)
                {            
                    // scrollable area is the optimal width. The height is the viewport height minus the height of the horizontal scrollbar
                    REQUIRE( helper.getScrolledAreaSize() == Size(optimalSize.width, viewPortSize.height-7) );
                    // horizontal scroll bar is visible, so the final viewport size should be smaller by that amount
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(0, 7) );
                }
                else
                {
                    // width is truncated.
                    REQUIRE( helper.getScrolledAreaSize() == Size(optimalSize.width-1, viewPortSize.height) );
                    REQUIRE( helper.getViewPortSize() == viewPortSize );
                }
            }

            SECTION("less width than needed, enough height for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(-1, 7);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == horzScrollingEnabled );
                REQUIRE( helper.getVerticalScrollBarVisible() == false );

                if(horzScrollingEnabled)
                {
                    REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                    // scrollable area is the optimal size
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize );

                    // horizontal scroll bar is visible, so the final viewport size should be smaller by that amount
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(0, 7) );
                }
                else
                {
                    // content view should be expanded to fill the available height (since there is no scrollbar).
                    // width is truncated
                    REQUIRE( helper.getContentViewBounds() == Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-1, 7) ) );
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize+Size(-1,7) );

                    REQUIRE( helper.getViewPortSize() == viewPortSize );
                }
            }

            SECTION("less width than needed, not enough height for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(-1, 6);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == horzScrollingEnabled );
                REQUIRE( helper.getVerticalScrollBarVisible() == (horzScrollingEnabled && vertScrollingEnabled) );

                if(horzScrollingEnabled && vertScrollingEnabled)
                {
                    // the horizontal scroll bar does not fit. So we should also get a vertical scrollbar                
                    REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                    // scrollable area is the optimal size
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize );

                    // both scroll bars are visible, so the final viewport size should be smaller by that amount
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 7) );
                }
                else if(horzScrollingEnabled)
                {
                    // the viewport height is reduced due to the horizontal scroll bar. But vert scrolling is not allowed, so we won't
                    // get a vertical scrollbar. So the content height will be truncated to one less than what is needed.
                    REQUIRE( helper.getContentViewBounds() == Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(0,-1)) );
            
                    // scrollable area is the optimal width, since we can scroll. The height is optimal-1, since we need to truncate
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize+Size(0,-1) );

                    // only one scroll bar is visible
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(0, 7) );
                }
                else
                {
                    // horizontal scrolling is disabled. vert scrolling is not needed, since without the horz scrollbar
                    // we have enough height. So there will be no scrolling. So it does not matter if vert scrolling is enabled or not.

                    // content fills the available space
                    REQUIRE( helper.getContentViewBounds() == Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-1, 6)) );
            
                    // scrolled area is extended / truncated to fit the available space
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize+Size(-1, 6) );

                    // viewport stays the same
                    REQUIRE( helper.getViewPortSize() == viewPortSize );
                }
            }

            SECTION("less height than needed, more than enough width for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(1000, -1);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == false );
                REQUIRE( helper.getVerticalScrollBarVisible() == vertScrollingEnabled );

                if(vertScrollingEnabled)
                {
                    REQUIRE( helper.getContentViewBounds() == Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(1000-13, 0) ) );
            
                    // scrollable area width is the viewport width minus the scrollbar width. Height is the optimal height
                    REQUIRE( helper.getScrolledAreaSize() == Size(viewPortSize.width-13, optimalSize.height) );
                    // vertical scroll bar is visible, so the final viewport size should be smaller by that amount
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 0) );
                }
                else
                {
                    // cannot scroll vertically => no scrollbar shown.
                    // width fills the viewport. height is truncated to available space

                    REQUIRE( helper.getContentViewBounds() == Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(1000, -1) ) );
            
                    // scrollable area width is the viewport width. Height is the optimal height, truncated to the available space
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize + Size(1000,-1) );

                    // no scrollbar visible => full viewport size
                    REQUIRE( helper.getViewPortSize() == viewPortSize );
                }
            }


            SECTION("less height than needed, enough width for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(13, -1);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == false );
                REQUIRE( helper.getVerticalScrollBarVisible() == vertScrollingEnabled );

                if(vertScrollingEnabled)
                {
                    REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                    // scrollable area size is the optimal size
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize );
                    // vertical scroll bar is visible, so the final viewport size should be smaller by that amount
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 0) );
                }
                else
                {
                    // cannot scroll vertically => no scrollbar shown.
                    // width fills the viewport. height is truncated to available space

                    REQUIRE( helper.getContentViewBounds() == Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(13, -1) ) );
            
                    // scrollable area width is the viewport width. Height is the optimal height, truncated to the available space
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize + Size(13,-1) );

                    // no scrollbar visible => full viewport size
                    REQUIRE( helper.getViewPortSize() == viewPortSize );
                }
            }

            SECTION("less height than needed, not enough width for scrollbar")
            {
                Size viewPortSize = optimalSize + Size(12, -1);

                helper.calcLayout(pScrollView, viewPortSize );

                // if both scrolling directions are enabled, the horizontal scrollbar should be visible now as well, since the viewport
                // is reduced too much by the vertical scroll bar
                REQUIRE( helper.getHorizontalScrollBarVisible() == (horzScrollingEnabled && vertScrollingEnabled) );
                REQUIRE( helper.getVerticalScrollBarVisible() == vertScrollingEnabled );

                if(horzScrollingEnabled && vertScrollingEnabled)
                {
                    REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                    // scrollable area size is the optimal size
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize );
                    // both scroll bars are visible, so the final viewport size should be smaller by that amount
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 7) );
                }
                else if(vertScrollingEnabled)
                {
                    // the viewport width is reduced due to the vertical scroll bar. But horz scrolling is not allowed, so we won't
                    // get a horizontal scrollbar. So the content width will be truncated to one less than what is needed.
                    REQUIRE( helper.getContentViewBounds() == Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-1,0)) );
            
                    // scrollable area is the optimal height, since we can scroll. The width is optimal-1, since we need to truncate
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize+Size(-1,0) );

                    // only one scroll bar is visible
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 0) );
                }
                else
                {
                    // vertical scrolling is disabled. horizontal scrolling is not needed, since without the vertical scrollbar
                    // we have enough width. So there will be no scrolling. So it does not matter if horz scrolling is enabled or not.

                    // content fills the available space
                    REQUIRE( helper.getContentViewBounds() == Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(12, -1)) );
            
                    // scrolled area is extended / truncated to fit the available space
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize+Size(12, -1) );

                    // viewport stays the same
                    REQUIRE( helper.getViewPortSize() == viewPortSize );
                }                
            }

            SECTION("less width and less height than needed")
            {
                Size viewPortSize = optimalSize + Size(-1, -1);

                helper.calcLayout(pScrollView, viewPortSize );

                REQUIRE( helper.getHorizontalScrollBarVisible() == horzScrollingEnabled );
                REQUIRE( helper.getVerticalScrollBarVisible() == vertScrollingEnabled );

                if(horzScrollingEnabled && vertScrollingEnabled)
                {
                    REQUIRE( helper.getContentViewBounds() == optimalButtonBounds );
            
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize );
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 7) );
                }
                else if(horzScrollingEnabled)
                {
                    // we cannot scroll vertically. So height is truncated, width is optimal
                    REQUIRE( helper.getContentViewBounds() == Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(0, -1-7)) );
            
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize+Size(0,-1-7) );

                    // only one scrollbar is visible
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(0, 7) );
                }
                else if(vertScrollingEnabled)
                {
                    // we cannot scroll horizontally. So width is truncated, height is optimal
                    REQUIRE( helper.getContentViewBounds() == Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-1-13, 0)) );
            
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize+Size(-1-13, 0) );

                    // only one scrollbar is visible
                    REQUIRE( helper.getViewPortSize() == viewPortSize-Size(13, 0) );
                }
                else
                {
                    // no scrolling. Width and height are truncated
                    REQUIRE( helper.getContentViewBounds() == Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-1,-1)) );
            
                    REQUIRE( helper.getScrolledAreaSize() == optimalSize+Size(-1,-1) );

                    // only one scrollbar is visible
                    REQUIRE( helper.getViewPortSize() == viewPortSize );
                }
            }
        };
    }
}


static void testScrollViewLayoutHelper(bool horzScrollingEnabled, bool vertScrollingEnabled)
{
    SECTION("calcPreferredSize")
        testScrollViewLayoutHelperPreferredSize(horzScrollingEnabled, vertScrollingEnabled);

    SECTION("layout")
        testScrollViewLayoutHelperLayout(horzScrollingEnabled, vertScrollingEnabled);
}

TEST_CASE("ScrollViewLayoutHelper")
{
    SECTION("scrollable in both directions")
        testScrollViewLayoutHelper(true, true);
    SECTION("only horz scrolling")
        testScrollViewLayoutHelper(true, false);
    SECTION("only vert scrolling")
        testScrollViewLayoutHelper(false, true);
    SECTION("no scrolling")
        testScrollViewLayoutHelper(false, false);
}

