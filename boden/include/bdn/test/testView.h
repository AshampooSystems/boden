#ifndef BDN_TEST_testView_H_
#define BDN_TEST_testView_H_


#include <bdn/test/MockUiProvider.h>
#include <bdn/test/ViewWithTestExtensions.h>

namespace bdn
{
namespace test
{

    
template<class ViewType>
class ViewTestPreparer : public Base
{
public:
    ViewTestPreparer()
    {
        _pUiProvider = newObj<MockUiProvider>();
        _pWindow = newObj<Window>(_pUiProvider);
    }

    P<MockUiProvider> getUiProvider()
    {
        return _pUiProvider;
    }

    P<Window> getWindow()
    {
        return _pWindow;
    }

    P< ViewWithTestExtensions<ViewType> > createView()
    {
        P< ViewWithTestExtensions<ViewType> > pView = newObj< ViewWithTestExtensions<ViewType> >();

        _pWindow->setContentView(pView);

        return pView;
    }

    void createLocalView()
    {
        ViewType view;
    }
    
protected:
    P<MockUiProvider>       _pUiProvider;
    P<Window>               _pWindow;
};



template<>
class ViewTestPreparer<Window> : public Base
{
public:
    ViewTestPreparer()
    {
        _pUiProvider = newObj<MockUiProvider>();
    }

    P<MockUiProvider> getUiProvider()
    {
        return _pUiProvider;
    }

    P<Window> getWindow()
    {
        return _pWindow;
    }

    P< ViewWithTestExtensions<Window> > createView()
    {
        P<ViewWithTestExtensions<Window> > pWindow = newObj< ViewWithTestExtensions<Window> >( _pUiProvider );

        _pWindow = pWindow;

        return pWindow;
    }

    void createLocalView()
    {
        Window window(_pUiProvider);
    }

protected:
    P<MockUiProvider>       _pUiProvider;
    P<Window>               _pWindow;
};


template<class ViewType>
inline bool shouldViewBeInitiallyVisible()
{
    return true;
}

template<>
inline bool shouldViewBeInitiallyVisible<Window>()
{
    return false;
}



template<class ViewType>
inline bool shouldViewHaveParent()
{
    return true;
}

template<>
inline bool shouldViewHaveParent<Window>()
{
    return false;
}



/** Helper function that performs an operation on a view object and verifies the result afterwards.

    The operation is performed twice: once from the main thread and once from another thread (if the target
    platform supports multithreading).

    @param pView the view to perform the operation on
    @param opFunc a function object that performs the action on the view
    @param verifyFunc a function object that verifies that the view is in the expected state after the action.
    @param expectedSizingInfoUpdates the number of sizing info updates the operation should trigger. This is usually
        either 0 (sizing info should not be updated) or 1 (sizing info should be updated).
*/
template<class ViewType>
inline void testViewOp(P< ViewWithTestExtensions<ViewType> > pView,
                std::function<void()> opFunc,
                std::function<void()> verifyFunc,
                int expectedSizingInfoUpdates )
{
    // schedule the test asynchronously, so that the initial sizing
	// info update from the view construction is already done.

	ASYNC_SECTION("mainThread", pView, opFunc, verifyFunc, expectedSizingInfoUpdates)
	{
        int initialSizingInfoUpdateCount = pView->getSizingInfoUpdateCount();

		opFunc();

        // sizing info updates should never happen immediately. We want them
		// to happen asynchronously, so that multiple changes can be handled
		// together with a single update.
		BDN_REQUIRE( pView->getSizingInfoUpdateCount() == initialSizingInfoUpdateCount );	

		verifyFunc();

		// sizing info updates should never happen immediately. We want them
		// to happen asynchronously, so that multiple changes can be handled
		// together with a single update.
		BDN_REQUIRE( pView->getSizingInfoUpdateCount() == initialSizingInfoUpdateCount );	


        CONTINUE_SECTION_ASYNC(pView, initialSizingInfoUpdateCount, expectedSizingInfoUpdates, opFunc, verifyFunc)
        {
            BDN_REQUIRE( pView->getSizingInfoUpdateCount() == initialSizingInfoUpdateCount + expectedSizingInfoUpdates );
        };
	};

#if BDN_HAVE_THREADS

    // schedule the test asynchronously, so that the initial sizing
	// info update from the view construction is already done.
	ASYNC_SECTION("otherThread", pView, opFunc, verifyFunc, expectedSizingInfoUpdates)
	{
        int initialSizingInfoUpdateCount = pView->getSizingInfoUpdateCount();

		// note that we call get on the future object, so that we wait until the
		// other thread has finished (so that any changes have been scheduled)
		Thread::exec( opFunc ).get();		

		// any changes made to properties by the opFunc are asynchronously scheduled.
		// So they have not actually been made in the core yet,

		// we want to wait until the changes have actually been made in the core.
		// So do another async call. That one will be executed after the property
		// changes.

        CONTINUE_SECTION_ASYNC( pView, verifyFunc, initialSizingInfoUpdateCount, expectedSizingInfoUpdates )
		{
			// the core should now have been updated.
			// However, sizing info updates should never happen immediately when
			// a core changes. We want them to happen asynchronously,
			// so that multiple changes can be handled together with a single update.

			// so the sizing info update count should still be unchanged
			BDN_REQUIRE( pView->getSizingInfoUpdateCount()==initialSizingInfoUpdateCount );	

			// now we do another async step. At that point the scheduled
			// update should have happened and the sizing info should have been
			// updated (once!)

            CONTINUE_SECTION_ASYNC( pView, verifyFunc, initialSizingInfoUpdateCount, expectedSizingInfoUpdates)
            {
				verifyFunc();
						
				BDN_REQUIRE( pView->getSizingInfoUpdateCount() == initialSizingInfoUpdateCount+expectedSizingInfoUpdates );
			};					
        };
	};

#endif

}


template<class ViewType >
inline void testView_Continue(
    P< ViewTestPreparer<ViewType> >         pPreparer,
    int                                     initialCoresCreated,
    P<Window>                               pWindow,
    P< ViewWithTestExtensions<ViewType> >   pView,
    P<bdn::test::MockViewCore>              pCore);

template<class ViewType >
inline void testView()
{
    P< ViewTestPreparer<ViewType> > pPreparer = newObj< ViewTestPreparer<ViewType> >();

    int initialCoresCreated = pPreparer->getUiProvider()->getCoresCreated();

    SECTION("onlyNewAllocAllowed")
	{
		BDN_REQUIRE_THROWS_PROGRAMMING_ERROR( pPreparer->createLocalView() );

		BDN_REQUIRE( pPreparer->getUiProvider()->getCoresCreated()==initialCoresCreated );
	}

	P< ViewWithTestExtensions<ViewType> > pView = pPreparer->createView();
	BDN_REQUIRE( pPreparer->getUiProvider()->getCoresCreated()==initialCoresCreated+1 );

	P<bdn::test::MockViewCore> pCore = cast<bdn::test::MockViewCore>( pView->getViewCore() );
	BDN_REQUIRE( pCore!=nullptr );

    P<Window> pWindow = pPreparer->getWindow();

    // when the view core is created then the view schedules a sizing info update.
    // We wait until that is done before we continue.
    REQUIRE( pView->getSizingInfoUpdateCount()==0 );
    
    BDN_CONTINUE_SECTION_ASYNC( pPreparer, initialCoresCreated, pWindow, pView, pCore )
    {
        // the pending updates should have happened now
        REQUIRE( pView->getSizingInfoUpdateCount()==1 );
        
	    SECTION("initialViewState")
	    {
            // the core should initialize its properties from the outer window when it is created.
		    // The outer window should not set them manually after construction.		
		    BDN_REQUIRE( pCore->getVisibleChangeCount()==0 );
		    BDN_REQUIRE( pCore->getMarginChangeCount()==0 );
		    BDN_REQUIRE( pCore->getPaddingChangeCount()==0 );
		    BDN_REQUIRE( pCore->getParentViewChangeCount()==0 );

		    BDN_REQUIRE( pView->visible() == shouldViewBeInitiallyVisible<ViewType>() );

		    BDN_REQUIRE( pView->margin() == UiMargin(UiLength::Unit::sem, 0, 0, 0, 0) );
		    BDN_REQUIRE( pView->padding() == UiMargin(UiLength::Unit::sem, 0, 0, 0, 0) );

		    BDN_REQUIRE( pView->horizontalAlignment() == View::HorizontalAlignment::left );
		    BDN_REQUIRE( pView->verticalAlignment() == View::VerticalAlignment::top );

		    BDN_REQUIRE( pView->getUiProvider().getPtr() == pPreparer->getUiProvider() );

            if( shouldViewHaveParent<ViewType>() )
		        BDN_REQUIRE( pView->getParentView()==cast<View>(pPreparer->getWindow()) );
            else
                BDN_REQUIRE( pView->getParentView()==nullptr );

		    BDN_REQUIRE( pView->getViewCore().getPtr() == pCore );


            // the view should not have any child views
		    std::list< P<View> > childViews;
		    pView->getChildViews(childViews);
		    BDN_REQUIRE( childViews.empty() );
		

		    // sizing info should have been updated now.
		    BDN_REQUIRE( pView->getSizingInfoUpdateCount()==1);        
	    }

        SECTION("multiple needSizingInfoUpdate calls cause single update")
        {
            int updateCountBefore = pView->getSizingInfoUpdateCount();

            pView->needSizingInfoUpdate();
            pView->needSizingInfoUpdate();

            CONTINUE_SECTION_ASYNC(pPreparer, pView, updateCountBefore)
            {
                REQUIRE( pView->getSizingInfoUpdateCount() == updateCountBefore+1 );
            };
        }
    
        SECTION("parentViewNullAfterParentDestroyed")
	    {
            P<ViewType> pView;

            {
                ViewTestPreparer<ViewType> preparer2;

                pView = preparer2.createView();

                if(shouldViewHaveParent<ViewType>())
                    BDN_REQUIRE( pView->getParentView() != nullptr);
                else
                    BDN_REQUIRE( pView->getParentView() == nullptr);
            }

            // preparer2 is now gone, so the parent view is not referenced there anymore.
            // But there may still be a scheduled sizing info update pending that holds a
            // reference to the window or child view.
            // Since we want the window to be destroyed, we do the remaining test asynchronously
            // after all pending operations are done.

            CONTINUE_SECTION_ASYNC(pView, pPreparer)
            {                
                BDN_REQUIRE( pView->getParentView() == nullptr);	    
            };
	    }

	    SECTION("changeViewProperty")
	    {
		    SECTION("visible")
		    {
			    testViewOp<ViewType>(
				    pView,
				    [pView, pWindow]()
				    {
					    pView->visible() = !shouldViewBeInitiallyVisible<ViewType>();
				    },
				    [pCore, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getVisibleChangeCount()==1 );
					    BDN_REQUIRE( pCore->getVisible() == !shouldViewBeInitiallyVisible<ViewType>() );	
				    },
				    0	// should NOT have caused a sizing info update
				    );
		    }
	
		    SECTION("margin")
		    {
			    UiMargin m(UiLength::Unit::sem, 1, 2, 3, 4);

			    testViewOp<ViewType>( 
				    pView,
				    [pView, m, pWindow]()
				    {
					    pView->margin() = m;
				    },
				    [pCore, m, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getMarginChangeCount()==1 );
					    BDN_REQUIRE( pCore->getMargin() == m);
				    },
				    0	// should NOT have caused a sizing info update
				    );
		    }

		    SECTION("padding")
		    {
			    UiMargin m(UiLength::Unit::sem, 1, 2, 3, 4);

			    testViewOp<ViewType>( 
				    pView,
				    [pView, m, pWindow]()
				    {
					    pView->padding() = m;
				    },
				    [pCore, m, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getPaddingChangeCount()==1 );
					    BDN_REQUIRE( pCore->getPadding() == m);
				    },
				    1	// should have caused a sizing info update
				    );
		    }

		    SECTION("bounds")
		    {
			    Rect b(1, 2, 3, 4);

			    testViewOp<ViewType>( 
				    pView,
				    [pView, b, pWindow]()
				    {
					    pView->bounds() = b;
				    },
				    [pCore, b, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getBoundsChangeCount()==2 );
					    BDN_REQUIRE( pCore->getBounds() == b);
				    },
				    0	// should NOT have caused a sizing info update
				    );
		    }
	    }


	    SECTION("multiplePropertyChangesThatInfluenceSizing")
	    {
		    testViewOp<ViewType>(
			    pView,

			    [pView, pWindow]()
			    {
				    pView->padding() = UiMargin(UiLength::Unit::sem, 7, 8, 9, 10);
				    pView->padding() = UiMargin(UiLength::Unit::sem, 6, 7, 8, 9);
			    },

			    [pCore, pView, pWindow]()
			    {
				    // padding changed twice
				    BDN_REQUIRE( pCore->getPaddingChangeCount()==2 );
				    BDN_REQUIRE( pCore->getPadding() == UiMargin(UiLength::Unit::sem, 6, 7, 8, 9));
			    },

			    1	// should cause a single(!) sizing info update

			    );		
	    }
    };
}

}
}

#endif

