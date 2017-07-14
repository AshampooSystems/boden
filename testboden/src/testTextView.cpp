#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextView.h>
#include <bdn/test/testView.h>
#include <bdn/test/MockTextViewCore.h>

using namespace bdn;

TEST_CASE("TextView")
{
    // test the generic view properties of Button
    SECTION("View-base")
        bdn::test::testView<TextView>();

	SECTION("TextView-specific")
	{
		P<bdn::test::ViewTestPreparer<TextView> >         pPreparer = newObj< bdn::test::ViewTestPreparer<TextView> >();
		P< bdn::test::ViewWithTestExtensions<TextView> >  pTextView = pPreparer->createView();
		P<bdn::test::MockTextViewCore>                    pCore = cast<bdn::test::MockTextViewCore>( pTextView->getViewCore() );

		REQUIRE( pCore!=nullptr );

		SECTION("initialState")
		{
			SECTION("text")
			{
				REQUIRE( pTextView->text() == "" );
				REQUIRE( pCore->getText()=="" );
				REQUIRE( pCore->getTextChangeCount()==0 );
			}
		}

		SECTION("changeProperty")
		{   
			SECTION("text")
			{
				bdn::test::_testViewOp( 
					pTextView,
                    pPreparer,
					[pTextView, pPreparer]()
					{
						pTextView->text() = "hello";					
					},
					[pCore, pTextView, pPreparer]
					{
						REQUIRE( pCore->getText()=="hello" );					
						REQUIRE( pCore->getTextChangeCount()==1 );					
					},
                    bdn::test::ExpectedSideEffect_::invalidateSizingInfo // should have caused sizing info to be invalidated
                    | bdn::test::ExpectedSideEffect_::invalidateParentLayout // should cause a parent layout update since sizing info was invalidated
					);
			}        
		}

	}	
}


