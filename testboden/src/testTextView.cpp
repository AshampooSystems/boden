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
			// the test view from the text view preparer has a text set (so that the preferredsizehint will have an effect).
			// So it is not in the initial state. So we create a new one here to test the initial state.
			P<TextView> pNewView = newObj<TextView>();
			pPreparer->getWindow()->setContentView( pNewView );

			P<bdn::test::MockTextViewCore> pNewCore = cast<bdn::test::MockTextViewCore>( pNewView->getViewCore() );

			SECTION("text")
			{
				REQUIRE( pNewView->text() == "" );
				REQUIRE( pNewCore->getText()=="" );
				REQUIRE( pNewCore->getTextChangeCount()==0 );
			}
		}

		SECTION("changeProperty")
		{   
			SECTION("text")
			{
				CONTINUE_SECTION_WHEN_IDLE(pTextView, pPreparer, pCore)
				{
					int initialChangeCount = pCore->getTextChangeCount();

					bdn::test::_testViewOp( 
						pTextView,
						pPreparer,
						[pTextView, pPreparer]()
						{
							pTextView->text() = "hello";					
						},
						[pCore, pTextView, pPreparer, initialChangeCount]
						{
							REQUIRE( pCore->getText()=="hello" );					
							REQUIRE( pCore->getTextChangeCount()==initialChangeCount+1 );					
						},
						bdn::test::ExpectedSideEffect_::invalidateSizingInfo // should have caused sizing info to be invalidated
						| bdn::test::ExpectedSideEffect_::invalidateParentLayout // should cause a parent layout update since sizing info was invalidated
						);
				};
			}        
		}

	}	
}


