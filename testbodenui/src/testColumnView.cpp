#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ColumnView.h>
#include <bdn/test/testView.h>

using namespace bdn;

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

        pButton->bounds() = Rect(10, 10, 10, 10);

        SECTION("addChildView")
        {
            CONTINUE_SECTION_ASYNC(pPreparer, pColumnView, pButton, pCore)
            {
                int sizingInfoUpdateCountBefore = pColumnView->getSizingInfoUpdateCount();
                int layoutCountBefore = pColumnView->getLayoutCount();

                pColumnView->addChildView(pButton);

                CONTINUE_SECTION_ASYNC(pPreparer, pColumnView, pButton, pCore, sizingInfoUpdateCountBefore, layoutCountBefore)
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

            CONTINUE_SECTION_ASYNC( pPreparer, pColumnView, pButton, pCore)
            {
                SECTION("child margins handled")
                {
                    CONTINUE_SECTION_ASYNC( pPreparer, pColumnView, pButton, pCore)
                    {
                        Size preferredSizeBefore = pColumnView->sizingInfo().get().preferredSize;
                        int sizingInfoUpdateCountBefore = pColumnView->getSizingInfoUpdateCount();
                        int layoutCountBefore = pColumnView->getLayoutCount();

                        pButton->margin() = UiMargin(UiLength::realPixel, 1, 2, 3, 4);

                        CONTINUE_SECTION_ASYNC(pPreparer, pColumnView, pButton, pCore, preferredSizeBefore, sizingInfoUpdateCountBefore, layoutCountBefore)
                        {
                            // should cause a sizing update for the column view, followed by a layout update
                            REQUIRE( pColumnView->getSizingInfoUpdateCount()==sizingInfoUpdateCountBefore+1 );
                            REQUIRE( pColumnView->getLayoutCount()==layoutCountBefore+1 );                

                            Size preferredSize = pColumnView->sizingInfo().get().preferredSize;

                            REQUIRE( preferredSize == preferredSizeBefore+Margin(1,2,3,4) );
                        };         
                    };
                }
            };
        }
	}	
}


