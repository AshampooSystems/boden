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

        SECTION("addChildView")
        {
            pColumnView->addChildView(pButton);

            ASYNC_SECTION("updates sizingInfo", = )
            {            
                // should cause a sizing update and a layout update
                REQUIRE( pColumnView->getSizingInfoUpdateCount()==1 );
                REQUIRE( pColumnView->getLayoutCount()==1 );                

                Size prefSize = pColumnView->sizingInfo().get().preferredSize;

                REQUIRE( prefSize!=Size(0,0) );

                REQUIRE( prefSize == pButton->sizingInfo().get().preferredSize );
            };            
        }

        SECTION("margins properly included in layout")
        {
        }
	}	
}


