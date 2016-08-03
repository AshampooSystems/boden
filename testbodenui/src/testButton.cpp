#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/test/testView.h>

using namespace bdn;

TEST_CASE("Button")
{
    // test the generic view properties of Button
    bdn::test::testView<Button>();

    bdn::test::ViewTestPreparer<Button>             preparer;
    P< bdn::test::ViewWithTestExtensions<Button> >  pButton = preparer.createView();
    P<bdn::test::MockButtonCore>                    pCore = cast<bdn::test::MockButtonCore>( pButton->getViewCore() );

	REQUIRE( pCore!=nullptr );

    SECTION("initialButtonState")
    {
        SECTION("label")
        {
            REQUIRE( pButton->label() == "" );
        }
    }

    SECTION("changeButtonProperty")
    {   
		SECTION("label")
		{
			bdn::test::testViewOp( 
				pButton,
				[pButton]()
				{
					pButton->label() = "hello";					
				},
				[pCore, pButton]
				{
					REQUIRE( pCore->getLabelChangeCount()==1 );
					REQUIRE( pCore->getLabel()=="hello" );					
				},
				1 // should cause a sizing update.
				);
		}        
    }
	
}


