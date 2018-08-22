#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Checkbox.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockCheckboxCore.h>

using namespace bdn;

TEST_CASE("Checkbox")
{
    // test the generic view properties of Checkbox
    SECTION("View-base")
        bdn::test::testView<Checkbox>();

	SECTION("Checkbox-specific")
	{
		P<bdn::test::ViewTestPreparer<Checkbox> >         pPreparer = newObj< bdn::test::ViewTestPreparer<Checkbox> >();
		P< bdn::test::ViewWithTestExtensions<Checkbox> >  pCheckbox= pPreparer->createView();
		P<bdn::test::MockCheckboxCore>                    pCore = cast<bdn::test::MockCheckboxCore>( pCheckbox->getViewCore() );

		REQUIRE( pCore!=nullptr );

		SECTION("initialCheckboxState")
		{
			SECTION("label")
			{
				REQUIRE( pCheckbox->label() == "" );
				REQUIRE( pCore->getLabel() == "" );
				REQUIRE( pCore->getLabelChangeCount() == 0 );
			}

			SECTION("state") 
			{
				REQUIRE( pCheckbox->state() == TriState::off );
                REQUIRE( pCore->getState() == TriState::off );
				REQUIRE( pCore->getStateChangeCount() == 0 );
			}
		}

		SECTION("changeCheckboxProperty")
		{   
			SECTION("label")
			{
				bdn::test::_testViewOp( 
					pCheckbox,
                    pPreparer,
					[pCheckbox]()
					{
						pCheckbox->setLabel( "hello" );
					},
					[pCore, pCheckbox]
					{
						REQUIRE( pCore->getLabel()=="hello" );					
						REQUIRE( pCore->getLabelChangeCount()==1 );					
					},
                    (int)bdn::test::ExpectedSideEffect_::invalidateSizingInfo // should have caused sizing info to be invalidated
                    | (int)bdn::test::ExpectedSideEffect_::invalidateParentLayout // should cause a parent layout update since sizing info was invalidated					
					);
			}

			SECTION("state")
			{
				bdn::test::_testViewOp(
					pCheckbox,
					pPreparer,
					[pCheckbox]()
					{
						pCheckbox->setState( TriState::on );
					},
					[pCore, pCheckbox]
					{
						REQUIRE( pCore->getState()==TriState::on );
						REQUIRE( pCore->getStateChangeCount()==1 );
					},
					0
				);
			}
		}
	}	
}


