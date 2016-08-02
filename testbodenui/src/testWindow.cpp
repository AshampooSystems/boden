#include <bdn/init.h>
#include <bdn/Window.h>

#include <bdn/test.h>

#include <bdn/ViewCoreTypeNotSupportedError.h>
#include <bdn/Button.h>

#include <bdn/test/MockUiProvider.h>


using namespace bdn;

void verifyMainThread()
{
	REQUIRE( Thread::isCurrentMain() );
}





class TestWindow : public Window
{
public:
	TestWindow(IUiProvider* pUiProvider)
		: Window(pUiProvider)
	{		
	}

	int _sizingInfoUpdateCount = 0;

	void updateSizingInfo() override
	{
		REQUIRE( Thread::isCurrentMain() );
		
		_sizingInfoUpdateCount++;
		Window::updateSizingInfo();		
	}	
};

void verifyOp(P<TestWindow> pWindow, std::function<void()> opFunc, std::function<void()> verifyFunc, int expectedSizingInfoUpdates )
{
	SECTION("mainThread")
	{
		// schedule the test asynchronously, so that the initial sizing
		// info update from the view construction is already done.

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pWindow, opFunc, verifyFunc, expectedSizingInfoUpdates]()
			{
				// sizing info update count should be 1 now (from construction).
				REQUIRE( pWindow->_sizingInfoUpdateCount==1 );

				opFunc();
				verifyFunc();

				// sizing info updates should never happen immediately. We want them
				// to happen asynchronously, so that multiple changes can be handled
				// together with a single update.
				REQUIRE( pWindow->_sizingInfoUpdateCount==1 );	

				asyncCallFromMainThread(
					[pWindow, expectedSizingInfoUpdates]()
					{
						REQUIRE( pWindow->_sizingInfoUpdateCount==1 + expectedSizingInfoUpdates );

						END_ASYNC_TEST();			
					});
			} );
	}

#if BDN_HAVE_THREADS

	SECTION("otherThread")
	{
		// schedule the test asynchronously, so that the initial sizing
		// info update from the view construction is already done.

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pWindow, opFunc, verifyFunc, expectedSizingInfoUpdates]()
			{
				// sizing info update count should be 1 now (from construction).
				REQUIRE( pWindow->_sizingInfoUpdateCount==1 );

				// note that we call get on the future object, so that we wait until the
				// other thread has finished (so that any changes have been scheduled)
				Thread::exec( opFunc ).get();		

				// any changes made to properties by the opFunc are asynchronously scheduled.
				// So they have not actually been made in the core yet,

				// we want to wait until the changes have actually been made in the core.
				// So do another async call. That one will be executed after the property
				// changes.
				asyncCallFromMainThread(
					[pWindow, verifyFunc, expectedSizingInfoUpdates]()
					{
						// the core should now have been updated.
						// However, sizing info updates should never happen immediately when
						// a core changes. We want them to happen asynchronously,
						// so that multiple changes can be handled together with a single update.

						// so the sizing info update count should still be unchanged
						REQUIRE( pWindow->_sizingInfoUpdateCount==1 );	

						// now we do another async step. At that point the scheduled
						// update should have happened and the sizing info should have been
						// updated (once!)
						asyncCallFromMainThread(
							[pWindow, verifyFunc, expectedSizingInfoUpdates]()
							{
								verifyFunc();
						
								REQUIRE( pWindow->_sizingInfoUpdateCount == 1+expectedSizingInfoUpdates );

								END_ASYNC_TEST();
							}	 );
					
					}	);				
			} );
	}

#endif

}


void testSizingWithContentView(P<TestWindow> pWindow, P<bdn::test::MockUiProvider> pUiProvider, std::function<Size()> getSizeFunc)
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
	P<bdn::test::MockUiProvider> pUiProvider = newObj<bdn::test::MockUiProvider>();

	SECTION("onlyNewAllocAllowed")
	{
		REQUIRE_THROWS_AS( Window window(pUiProvider), ProgrammingError);

		REQUIRE( pUiProvider->getWindowCoresCreated()==0 );
	}

	P<TestWindow> pWindow = newObj<TestWindow>( pUiProvider );				
	REQUIRE( pUiProvider->getWindowCoresCreated()==1 );

	P<bdn::test::MockWindowCore> pCore = cast<bdn::test::MockWindowCore>( pWindow->getViewCore() );
	REQUIRE( pCore!=nullptr );

	SECTION("construct")
	{
		// the core should initialize its properties from the outer window when it is created.
		// The outer window should not set them manually after construction.		
		REQUIRE( pCore->getVisibleChangeCount()==0 );
		REQUIRE( pCore->getMarginChangeCount()==0 );
		REQUIRE( pCore->getPaddingChangeCount()==0 );
		REQUIRE( pCore->getBoundsChangeCount()==0 );
		REQUIRE( pCore->getParentViewChangeCount()==0 );
		REQUIRE( pCore->getTitleChangeCount()==0 );

		// windows should not be visible initially
		REQUIRE( !pWindow->visible() );

		REQUIRE( pWindow->bounds() == Rect(0,0,0,0) );
		REQUIRE( pWindow->margin() == UiMargin(UiLength::Unit::sem, 0, 0, 0, 0) );
		REQUIRE( pWindow->padding() == UiMargin(UiLength::Unit::sem, 0, 0, 0, 0) );

		REQUIRE( pWindow->horizontalAlignment() == View::HorizontalAlignment::left );
		REQUIRE( pWindow->verticalAlignment() == View::VerticalAlignment::top );

		REQUIRE( pWindow->getUiProvider().getPtr() == pUiProvider);

		REQUIRE( pWindow->getParentView()==nullptr );

		REQUIRE( pWindow->getViewCore().getPtr() == pCore );

		REQUIRE( pWindow->title() == "");

		std::list< P<View> > childViews;
		pWindow->getChildViews(childViews);
		REQUIRE( childViews.empty() );
		

		// sizing info should not yet have been updated. It should
		// update asynchronously, so that multiple property
		// changes can be handled with a single update.
		REQUIRE( pWindow->_sizingInfoUpdateCount==0);

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pWindow]()
			{
				REQUIRE( pWindow->_sizingInfoUpdateCount==1);			

				END_ASYNC_TEST();
			});
	}

	SECTION("changeProperty")
	{
		SECTION("visible")
		{
			verifyOp(
				pWindow,
				[pWindow]()
				{
					pWindow->visible() = true;
				},
				[pCore, pWindow]()
				{
					REQUIRE( pCore->getVisibleChangeCount()==1 );
					REQUIRE( pCore->getVisible() );	
				},
				0	// should NOT have caused a sizing info update
				);
		}
	
		SECTION("margin")
		{
			UiMargin m(UiLength::Unit::sem, 1, 2, 3, 4);

			verifyOp( 
				pWindow,
				[pWindow, m]()
				{
					pWindow->margin() = m;
				},
				[pCore, m, pWindow]()
				{
					REQUIRE( pCore->getMarginChangeCount()==1 );
					REQUIRE( pCore->getMargin() == m);
				},
				0	// should NOT have caused a sizing info update
				);
		}

		SECTION("padding")
		{
			UiMargin m(UiLength::Unit::sem, 1, 2, 3, 4);

			verifyOp( 
				pWindow,
				[pWindow, m]()
				{
					pWindow->padding() = m;
				},
				[pCore, m, pWindow]()
				{
					REQUIRE( pCore->getPaddingChangeCount()==1 );
					REQUIRE( pCore->getPadding() == m);
				},
				1	// should have caused a sizing info update
				);
		}

		SECTION("bounds")
		{
			Rect b(1, 2, 3, 4);

			verifyOp( 
				pWindow,
				[pWindow, b]()
				{
					pWindow->bounds() = b;
				},
				[pCore, b, pWindow]()
				{
					REQUIRE( pCore->getBoundsChangeCount()==1 );
					REQUIRE( pCore->getBounds() == b);
				},
				0	// should NOT have caused a sizing info update
				);
		}


		SECTION("title")
		{
			verifyOp( 
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


	SECTION("multiplePropertyChangesThatInfluenceSizing")
	{
		verifyOp(
			pWindow,

			[pWindow]()
			{
				pWindow->padding() = UiMargin(UiLength::Unit::sem, 7, 8, 9, 10);
				pWindow->padding() = UiMargin(UiLength::Unit::sem, 6, 7, 8, 9);
			},

			[pCore, pWindow]()
			{
				// padding changed twice
				REQUIRE( pCore->getPaddingChangeCount()==2 );
				REQUIRE( pCore->getPadding() == UiMargin(UiLength::Unit::sem, 6, 7, 8, 9));
			},

			1	// should cause a single(!) sizing info update

			);		
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
				testSizingWithContentView( pWindow, pUiProvider, [pWindow](){ return pWindow->calcPreferredSize(); } );

			SECTION("sizingInfo")
				testSizingWithContentView( pWindow, pUiProvider, [pWindow](){ return pWindow->sizingInfo().get().preferredSize; } );
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



