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
	CONTINUE_SECTION_AFTER_PENDING_EVENTS(getSizeFunc, expectedSize)
	{
		Size size = getSizeFunc();

		REQUIRE( size == expectedSize );
	};
}



TEST_CASE("Window", "[ui]")
{   
    SECTION("View-base")
        bdn::test::testView<Window>();

	SECTION("Window-specific")
	{
		P<bdn::test::ViewTestPreparer<Window> > pPreparer = newObj<bdn::test::ViewTestPreparer<Window> >();

		P< bdn::test::ViewWithTestExtensions<Window> > pWindow = pPreparer->createView();

		P<bdn::test::MockWindowCore> pCore = cast<bdn::test::MockWindowCore>( pWindow->getViewCore() );
		REQUIRE( pCore!=nullptr );

		// continue testing after the async init has finished
        CONTINUE_SECTION_AFTER_PENDING_EVENTS(pPreparer, pWindow, pCore)
    {
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

            SECTION("contentView")
		    {
                SECTION("!=null")
		        {
                    P<Button> pButton = newObj<Button>();
                    bdn::test::testViewOp( 
				        pWindow,
				        [pWindow, pButton]()
				        {
					        pWindow->setContentView(pButton);
				        },
				        [pWindow, pButton]
				        {
                            REQUIRE( pWindow->getContentView() == cast<View>(pButton) );
				        },
				        1	// should have caused a sizing info update
				    );		        
		        }


                SECTION("null")
		        {
                    // basically we only test here that there is no crash when the content view is set to null
                    // and that it does not result in a sizing info update.
                    bdn::test::testViewOp( 
				        pWindow,
				        [pWindow]()
				        {
					        pWindow->setContentView(nullptr);
				        },
				        [pWindow]
				        {
                            REQUIRE( pWindow->getContentView() == nullptr);
				        },
				        0	// should not have caused a sizing info update
				    );		        
		        }
		    }
	    }

        SECTION("childParent")
	    {
            P<Button> pChild = newObj<Button>();

	        SECTION("setWhenAdded")
            {
                pWindow->setContentView(pChild);

                BDN_REQUIRE( pChild->getParentView() == cast<View>(pWindow) );
            }

            SECTION("nullAfterDestroy")
            {
                {
                    bdn::test::ViewTestPreparer<Window> preparer2;

                    P< bdn::test::ViewWithTestExtensions<Window> > pWindow2 = preparer2.createView();

                    pWindow2->setContentView(pChild);
                }

            
                // preparer2 is now gone, so the window is not referenced there anymore.
                // But there may still be a scheduled sizing info update pending that holds a
                // reference to the window.
                // Since we want the window to be destroyed, we do the remaining test asynchronously
                // after all pending operations are done.

                CONTINUE_SECTION_AFTER_PENDING_EVENTS_WITH(
                    [pChild]()
                    {                
                        BDN_REQUIRE( pChild->getParentView() == nullptr);	    
                    });
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
                    CONTINUE_SECTION_AFTER_PENDING_EVENTS(pWindow, expectedSize)
                    {
                        View::SizingInfo sizingInfo = pWindow->sizingInfo();

                        REQUIRE( sizingInfo.preferredSize == expectedSize );
                
                    };				
			    }
		    }

		    SECTION("withContentView")
		    {
			    SECTION("calcPreferredSize")
				    testSizingWithContentView( pWindow, pPreparer->getUiProvider(), [pWindow](){ return pWindow->calcPreferredSize(); } );

			    SECTION("sizingInfo")
				    testSizingWithContentView( pWindow, pPreparer->getUiProvider(), [pWindow](){ return pWindow->sizingInfo().get().preferredSize; } );
		    }
	    }

	    SECTION("autoSize")
	    {
            Rect boundsBefore = pWindow->bounds();

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

		    REQUIRE( pWindow->bounds() == boundsBefore );

            CONTINUE_SECTION_AFTER_PENDING_EVENTS_WITH(
			    [pWindow]()
			    {
				    REQUIRE( pWindow->bounds() == Rect(0,0, 100, 32) );
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

		    CONTINUE_SECTION_AFTER_PENDING_EVENTS(pWindow)
			{
				// the work area of our mock window is 100,100 800x800
				REQUIRE( pWindow->bounds() == Rect(	100 + (800-200)/2,
													100 + (800-200)/2,
													200,
													200) );
			};
	    }		
    };

    }
}



