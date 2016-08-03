#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/test.h>
#include <bdn/test/testView.h>

#include <bdn/Button.h>

using namespace bdn;


void testSizingWithContentView(P< bdn::test::ViewWithTestExtensions<Window> > pWindow, P<bdn::test::MockUiProvider> pUiProvider, std::function<Size()> getSizeFunc)
{	
	// we add a button as a content view
	P<Button> pButton = newObj<Button>();
	pButton->label() = "HelloWorld";

	Margin buttonMargin;

	SECTION("noMargin")
	{
		// do nothing
	}

	SECTION("semMargin")
	{
		pButton->margin() = UiMargin(UiLength::Unit::sem, 1, 2, 3, 4);

		// 1 sem = 20 pixels in our mock ui
		buttonMargin = Margin(20, 40, 60, 80 );
	}

	SECTION("pixel96Margin")
	{
		pButton->margin() = UiMargin(UiLength::Unit::pixel96, 1, 2, 3, 4);

		// 1 pixel96 = 3 pixels in our mock ui
		buttonMargin = Margin(3, 6, 9, 12 );
	}

	SECTION("realPixelMargin")
	{
		pButton->margin() = UiMargin(UiLength::Unit::realPixel, 1, 2, 3, 4);

		buttonMargin = Margin(1, 2, 3, 4 );
	}

	pWindow->setContentView( pButton );

	P<bdn::test::MockButtonCore> pButtonCore = cast<bdn::test::MockButtonCore>( pButton->getViewCore() );

	// Sanity check. Verify the fake button size. 10x20 per character, plus 10x8 for border
	Size buttonSize( 10*10 + 10, 20 + 8);
	REQUIRE( pButtonCore->calcPreferredSize() == buttonSize );

	// window border size is 20, 11, 12, 13 in our fake UI
	Margin windowBorder( 20, 11, 12, 13);

	Size expectedSize = buttonSize + buttonMargin + windowBorder;

	// the sizing info will update asynchronously. So we need to do the
	// check async as well.
	MAKE_ASYNC_TEST(10);
			
	// the button's size change will propagate to the window asynchronously.
	// So we must check async as well
			
	asyncCallFromMainThread(
		[getSizeFunc, expectedSize]()
		{
			Size size = getSizeFunc();

			REQUIRE( size == expectedSize );

			END_ASYNC_TEST();				
		} );
}



TEST_CASE("Window", "[ui]")
{   
    bdn::test::testView<Window>();

    bdn::test::ViewTestPreparer<Window> preparer;

    P< bdn::test::ViewWithTestExtensions<Window> > pWindow = preparer.createView();

    P<bdn::test::MockWindowCore> pCore = cast<bdn::test::MockWindowCore>( pWindow->getViewCore() );
	REQUIRE( pCore!=nullptr );

    // testView already tests the initialization of properties defined in View.
    // So we only have to test the Window-specific things here.
	SECTION("constructWindowSpecific")
	{
		REQUIRE( pCore->getTitleChangeCount()==0 );

		REQUIRE( pWindow->title() == "");
	}

	SECTION("changeWindowProperty")
	{
		SECTION("title")
		{
			bdn::test::testViewOp( 
				pWindow,
				[pWindow]()
				{
					pWindow->title() = "hello";					
				},
				[pCore, pWindow]
				{
					REQUIRE( pCore->getTitleChangeCount()==1 );
					REQUIRE( pCore->getTitle()=="hello" );					
				},
				0	// should NOT cause a sizing info update, since the
					// title is not part of the "preferred size" calculation
				);
		}
	}
    
	SECTION("sizing")
	{
		SECTION("noContentView")
		{
			// the mock window core reports border margins 20,11,12,13
			// and a minimum size of 100x32.		
			// Since we do not have a content view we should get the min size here.
			Size expectedSize(100, 32);

			SECTION("calcPreferredSize")
				REQUIRE( pWindow->calcPreferredSize()==expectedSize );

			SECTION("sizingInfo")
			{

				// sizing info is updated asynchronously. So we need to check async as well.
				MAKE_ASYNC_TEST(10);

				asyncCallFromMainThread(
					[pWindow, expectedSize]()
					{
						View::SizingInfo sizingInfo = pWindow->sizingInfo();

						REQUIRE( sizingInfo.preferredSize == expectedSize );

						END_ASYNC_TEST();
					
					}	);				
			}
		}

		SECTION("withContentView")
		{
			SECTION("calcPreferredSize")
				testSizingWithContentView( pWindow, preparer.getUiProvider(), [pWindow](){ return pWindow->calcPreferredSize(); } );

			SECTION("sizingInfo")
				testSizingWithContentView( pWindow, preparer.getUiProvider(), [pWindow](){ return pWindow->sizingInfo().get().preferredSize; } );
		}
	}

	SECTION("autoSize")
	{
		SECTION("mainThread")
		{
			pWindow->requestAutoSize();
		}

#if BDN_HAVE_THREADS
		SECTION("otherThread")
		{
			Thread::exec(
				[pWindow]()
				{
					pWindow->requestAutoSize();				
				} )
				.get(); // wait for thread to finish.
		}
#endif

		// auto-sizing is ALWAYS done asynchronously, no matter
		// which thread the request is coming from.
		// So nothing should have happened yet.

		REQUIRE( pWindow->bounds() == Rect(0,0,0,0) );

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pWindow]()
			{
				REQUIRE( pWindow->bounds() == Rect(0,0, 100, 32) );

				END_ASYNC_TEST();
			}
			);		
	}



	SECTION("center")
	{
		pWindow->bounds() = Rect(0, 0, 200, 200);

		SECTION("mainThread")
		{
			pWindow->requestCenter();
		}

#if BDN_HAVE_THREADS
		SECTION("otherThread")
		{
			Thread::exec(
				[pWindow]()
				{
					pWindow->requestCenter();				
				} )
				.get(); // wait for thread to finish.
		}
#endif

		// centering is ALWAYS done asynchronously, no matter
		// which thread the request is coming from.
		// So nothing should have happened yet.

		REQUIRE( pWindow->bounds() == Rect(0, 0, 200, 200) );

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pWindow]()
			{
				// the work area of our mock window is 100,100 800x800
				REQUIRE( pWindow->bounds() == Rect(	100 + (800-200)/2,
													100 + (800-200)/2,
													200,
													200) );

				END_ASYNC_TEST();
			}
			);		
	}
		
}



