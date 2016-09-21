#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ColumnView.h>
#include <bdn/test/testView.h>

using namespace bdn;


void testChildAlignment(
    P<bdn::test::ViewTestPreparer<ColumnView> > pPreparer,
    P< bdn::test::ViewWithTestExtensions<ColumnView> > pColumnView,
    P<Button> pButton,
    View::HorizontalAlignment horzAlign,
    View::VerticalAlignment vertAlign)
{
    // add a second button that is considerably bigger.
    // That will cause the column view to become bigger.
    P<Button> pButton2 = newObj<Button>();
    pButton2->padding() = UiMargin(UiLength::dip, 500, 500 );

    pColumnView->addChildView(pButton2);

    if(pButton->horizontalAlignment()==horzAlign)
    {
        // change to another horizontal alignment, so that the setting
        // of the requested alignment is registered as a change
        pButton->horizontalAlignment() = (horzAlign==View::HorizontalAlignment::left) ? View::HorizontalAlignment::right : View::HorizontalAlignment::left;
    }

    if(pButton->verticalAlignment()==vertAlign)
    {
        // change to another vertical alignment, so that the setting
        // of the requested alignment is registered as a change
        pButton->verticalAlignment() = (vertAlign==View::VerticalAlignment::top) ? View::VerticalAlignment::bottom : View::VerticalAlignment::top;
    }

    CONTINUE_SECTION_AFTER_PENDING_EVENTS(pPreparer, pColumnView, pButton, horzAlign, vertAlign)
    {
        int sizingInfoBeforeCount = pColumnView->getSizingInfoUpdateCount();
        int layoutCountBefore = pColumnView->getLayoutCount();

        SECTION("horizontal")
        {
            pButton->horizontalAlignment() = horzAlign;

            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pPreparer, pColumnView, pButton, horzAlign, sizingInfoBeforeCount, layoutCountBefore)
            {
                // sizing info should NOT have been updated
                REQUIRE( pColumnView->getSizingInfoUpdateCount()==sizingInfoBeforeCount);

                // but layout should have happened
                REQUIRE( pColumnView->getLayoutCount() == layoutCountBefore+1 );

                Rect bounds = Rect( pButton->position(), pButton->size() );
                Rect containerBounds = Rect( pColumnView->position(), pColumnView->size() );

                // sanity check: the button should be smaller than the columnview
                // unless the alignment is "expand"
                if(horzAlign!=View::HorizontalAlignment::expand)
                    REQUIRE( bounds.width < containerBounds.width );
                
                // and the view should now be aligned accordingly.
                if(horzAlign==View::HorizontalAlignment::left)
                {
                    REQUIRE( bounds.x==0 );
                }
                else if(horzAlign==View::HorizontalAlignment::center)
                {
                    REQUIRE( bounds.x == (containerBounds.width-bounds.width)/2  );
                }
                else if(horzAlign==View::HorizontalAlignment::right)
                {
                    REQUIRE( bounds.x == containerBounds.width-bounds.width  );
                }
                else if(horzAlign==View::HorizontalAlignment::expand)
                {
                    REQUIRE( bounds.x == 0);
                    REQUIRE( bounds.width == containerBounds.width );
                }
            };
        }
        
        SECTION("vertical")
        {
            pButton->verticalAlignment() = vertAlign;

            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pPreparer, pColumnView, pButton, vertAlign, sizingInfoBeforeCount, layoutCountBefore)
            {
                // sizing info should NOT have been updated
                REQUIRE( pColumnView->getSizingInfoUpdateCount()==sizingInfoBeforeCount);

                // but layout should have
                REQUIRE( pColumnView->getLayoutCount() == layoutCountBefore+1 );

                // vertical alignment should have NO effect in a column view.
                Rect bounds = Rect( pButton->position(), pButton->size() );
                Rect containerBounds = Rect( pColumnView->position(), pColumnView->size() );

                REQUIRE( bounds.y==0 );
                REQUIRE( bounds.height < containerBounds.height );

            };
        }
    };
}

TEST_CASE("ColumnView")
{
    // test the generic view properties of Button
    SECTION("View-base")
        bdn::test::testView<ColumnView>();

	SECTION("ColumnView-specific")
	{
		P<bdn::test::ViewTestPreparer<ColumnView> >         pPreparer = newObj< bdn::test::ViewTestPreparer<ColumnView> >();
		P< bdn::test::ViewWithTestExtensions<ColumnView> >  pColumnView = pPreparer->createView();
		P<bdn::test::MockContainerViewCore>                 pCore = cast<bdn::test::MockContainerViewCore>( pColumnView->getViewCore() );

		REQUIRE( pCore!=nullptr );

        P<Button> pButton = newObj<Button>();

        pButton->position() = Point(10, 10);
        pButton->size() = Size(10, 10);

        SECTION("addChildView")
        {
            CONTINUE_SECTION_AFTER_PENDING_EVENTS(pPreparer, pColumnView, pButton, pCore)
            {
                int sizingInfoUpdateCountBefore = pColumnView->getSizingInfoUpdateCount();
                int layoutCountBefore = pColumnView->getLayoutCount();

                pColumnView->addChildView(pButton);

                CONTINUE_SECTION_AFTER_PENDING_EVENTS(pPreparer, pColumnView, pButton, pCore, sizingInfoUpdateCountBefore, layoutCountBefore)
                {
                    // should cause a sizing update and a layout update
                    REQUIRE( pColumnView->getSizingInfoUpdateCount()==sizingInfoUpdateCountBefore+1 );
                    REQUIRE( pColumnView->getLayoutCount()==layoutCountBefore+1 );                

                    Size preferredSize = pColumnView->sizingInfo().get().preferredSize;

                    Size buttonPreferredSize = pButton->sizingInfo().get().preferredSize;

                    REQUIRE( preferredSize!=Size(0,0) );

                    REQUIRE( preferredSize == pButton->sizingInfo().get().preferredSize );
                };            
            };
        }

        SECTION("with child view")
        {
            pColumnView->addChildView(pButton);

            pPreparer->getWindow()->requestAutoSize();

            CONTINUE_SECTION_AFTER_PENDING_EVENTS( pPreparer, pColumnView, pButton, pCore)
            {
                SECTION("child margins")
                {
                    CONTINUE_SECTION_AFTER_PENDING_EVENTS( pPreparer, pColumnView, pButton, pCore)
                    {
                        Size preferredSizeBefore = pColumnView->sizingInfo().get().preferredSize;
                        int sizingInfoUpdateCountBefore = pColumnView->getSizingInfoUpdateCount();
                        int layoutCountBefore = pColumnView->getLayoutCount();

                        pButton->margin() = UiMargin(UiLength::dip, 1, 2, 3, 4);

                        CONTINUE_SECTION_AFTER_PENDING_EVENTS(pPreparer, pColumnView, pButton, pCore, preferredSizeBefore, sizingInfoUpdateCountBefore, layoutCountBefore)
                        {
                            // should cause a sizing update for the column view, followed by a layout update
                            REQUIRE( pColumnView->getSizingInfoUpdateCount()==sizingInfoUpdateCountBefore+1 );
                            REQUIRE( pColumnView->getLayoutCount()==layoutCountBefore+1 );                

                            Size preferredSize = pColumnView->sizingInfo().get().preferredSize;

                            REQUIRE( preferredSize == preferredSizeBefore+Margin(1,2,3,4) );
                        };         
                    };
                }

                SECTION("child alignment")
                {
                    for(int horzAlign = (int)View::HorizontalAlignment::left; horzAlign<=(int)View::HorizontalAlignment::expand; horzAlign++)
                    {
                        for(int vertAlign = (int)View::VerticalAlignment::top; vertAlign<=(int)View::VerticalAlignment::expand; vertAlign++)
                        {
                            SECTION( toString(horzAlign)+", "+toString(vertAlign) )
                                testChildAlignment(pPreparer, pColumnView, pButton, (View::HorizontalAlignment) horzAlign, (View::VerticalAlignment)vertAlign );
                        }
                    }
                }
            };
        }

        SECTION("multiple child views")
        {
            pColumnView->addChildView(pButton);

            P<Button> pButton2 = newObj<Button>();
            pColumnView->addChildView(pButton2);
            
            pPreparer->getWindow()->requestAutoSize();

            Margin m;
            Margin m2;

            SECTION("no margins")
            {
                m = Margin(0);
                m2 = Margin(0);
            }

            SECTION("with margins")
            {
                m = Margin(10, 20, 30, 40);
                m2 = Margin(11, 22, 33, 44);
            }

            pButton->margin() = UiMargin(UiLength::dip, m.top, m.right, m.bottom, m.left );
            pButton2->margin() = UiMargin(UiLength::dip, m2.top, m2.right, m2.bottom, m2.left );

            CONTINUE_SECTION_AFTER_PENDING_EVENTS( pPreparer, pColumnView, pButton, pButton2, pCore, m, m2)
            {
                Rect bounds = Rect( pButton->position(), pButton->size());
                Rect bounds2 = Rect( pButton2->position(), pButton2->size());

                SECTION("properly arranged")
                {
                    REQUIRE( bounds.x == m.left);
                    REQUIRE( bounds.y == m.top);
                    REQUIRE( bounds.width == pButton->sizingInfo().get().preferredSize.width );
                    REQUIRE( bounds.height == pButton->sizingInfo().get().preferredSize.height );

                    REQUIRE( bounds2.x == m2.left );
                    REQUIRE( bounds2.y == bounds.y + bounds.height + m.bottom + m2.top );
                    REQUIRE( bounds2.width == pButton2->sizingInfo().get().preferredSize.width );
                    REQUIRE( bounds2.height == pButton2->sizingInfo().get().preferredSize.height );
                }
            };
        }

	}	
}


