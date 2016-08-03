#ifndef BDN_TEST_testView_H_
#define BDN_TEST_testView_H_


#include <bdn/test/MockUiProvider.h>

namespace bdn
{
namespace test
{


template<class BaseViewClass>
class ViewWithTestExtensions : public BaseViewClass
{
public:
    template<typename... Arguments>
	ViewWithTestExtensions(Arguments... args)
        : BaseViewClass(args...)
	{		
	}


    /** Returns the number of times that the sizing information has been updated.*/
    int getSizingInfoUpdateCount() const
    {
        return _sizingInfoUpdateCount;
    }

	void updateSizingInfo() override
	{
		REQUIRE_IN_MAIN_THREAD();
		
		_sizingInfoUpdateCount++;
		BaseViewClass::updateSizingInfo();		
	}	

protected:    
	int _sizingInfoUpdateCount = 0;
};


template<class ViewType>
class ViewTestPreparer : public Base
{
public:
    ViewTestPreparer()
    {
        _pUiProvider = newObj<MockUiProvider>();
        _pWindow = newObj<Window>(pUiProvider);
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

        pWindow->setContentView(pView);

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
bool shouldViewBeInitiallyVisible()
{
    return true;
}

template<>
bool shouldViewBeInitiallyVisible<Window>()
{
    return false;
}



template<class ViewType>
bool shouldViewHaveParent()
{
    return true;
}

template<>
bool shouldViewHaveParent<Window>()
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
void testViewOp(P< ViewWithTestExtensions<ViewType> > pView,
                std::function<void()> opFunc,
                std::function<void()> verifyFunc,
                int expectedSizingInfoUpdates )
{
	SECTION("mainThread")
	{
		// schedule the test asynchronously, so that the initial sizing
		// info update from the view construction is already done.

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pView, opFunc, verifyFunc, expectedSizingInfoUpdates]()
			{
                int initialSizingInfoUpdateCount = pView->getSizingInfoUpdateCount();

				opFunc();
				verifyFunc();

				// sizing info updates should never happen immediately. We want them
				// to happen asynchronously, so that multiple changes can be handled
				// together with a single update.
				REQUIRE( pView->getSizingInfoUpdateCount() == initialSizingInfoUpdateCount );	

				asyncCallFromMainThread(
					[pView, initialSizingInfoUpdateCount, expectedSizingInfoUpdates]()
					{
						REQUIRE( pView->getSizingInfoUpdateCount() == initialSizingInfoUpdateCount + expectedSizingInfoUpdates );

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
			[pView, opFunc, verifyFunc, expectedSizingInfoUpdates]()
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
				asyncCallFromMainThread(
					[pView, verifyFunc, initialSizingInfoUpdateCount, expectedSizingInfoUpdates]()
					{
						// the core should now have been updated.
						// However, sizing info updates should never happen immediately when
						// a core changes. We want them to happen asynchronously,
						// so that multiple changes can be handled together with a single update.

						// so the sizing info update count should still be unchanged
						REQUIRE( pView->getSizingInfoUpdateCount()==initialSizingInfoUpdateCount );	

						// now we do another async step. At that point the scheduled
						// update should have happened and the sizing info should have been
						// updated (once!)
						asyncCallFromMainThread(
							[pView, verifyFunc, initialSizingInfoUpdateCount, expectedSizingInfoUpdates]()
							{
								verifyFunc();
						
								REQUIRE( pView->getSizingInfoUpdateCount() == initialSizingInfoUpdateCount+expectedSizingInfoUpdates );

								END_ASYNC_TEST();
							}	 );
					
					}	);				
			} );
	}

#endif

}



template<class ViewType >
void testView()
{
    ViewTestPreparer<ViewType> preparer;
    
	SECTION("onlyNewAllocAllowed")
	{
		REQUIRE_THROWS_AS( preparer.createLocalView(), ProgrammingError);

		REQUIRE( preparer.getUiProvider()->getCoresCreated()==0 );
	}

	P< ViewWithTestExtensions<ViewType> > pView = preparer.createView();
	REQUIRE( preparer.getUiProvider()->getCoresCreated()==1 );

	P<bdn::test::MockViewCore> pCore = cast<bdn::test::MockViewCore>( pView->getViewCore() );
	REQUIRE( pCore!=nullptr );

	SECTION("constructView")
	{
		// the core should initialize its properties from the outer window when it is created.
		// The outer window should not set them manually after construction.		
		REQUIRE( pCore->getVisibleChangeCount()==0 );
		REQUIRE( pCore->getMarginChangeCount()==0 );
		REQUIRE( pCore->getPaddingChangeCount()==0 );
		REQUIRE( pCore->getBoundsChangeCount()==0 );
		REQUIRE( pCore->getParentViewChangeCount()==0 );

		REQUIRE( pView->visible() == shouldViewBeInitiallyVisible<ViewType>() );

		REQUIRE( pView->bounds() == Rect(0,0,0,0) );
		REQUIRE( pView->margin() == UiMargin(UiLength::Unit::sem, 0, 0, 0, 0) );
		REQUIRE( pView->padding() == UiMargin(UiLength::Unit::sem, 0, 0, 0, 0) );

		REQUIRE( pView->horizontalAlignment() == View::HorizontalAlignment::left );
		REQUIRE( pView->verticalAlignment() == View::VerticalAlignment::top );

		REQUIRE( pView->getUiProvider().getPtr() == preparer.getUiProvider() );

        if( shouldViewHaveParent<ViewType>() )
		    REQUIRE( pView->getParentView()==cast<View>(preparer.getWindow()) );
        else
            REQUIRE( pView->getParentView()==nullptr );

		REQUIRE( pView->getViewCore().getPtr() == pCore );


        // the view should not have any child views
		std::list< P<View> > childViews;
		pView->getChildViews(childViews);
		REQUIRE( childViews.empty() );
		

		// sizing info should not yet have been updated. It should
		// update asynchronously, so that multiple property
		// changes can be handled with a single update.
		REQUIRE( pView->getSizingInfoUpdateCount()==0);

		MAKE_ASYNC_TEST(10);

		asyncCallFromMainThread(
			[pView]()
			{
				REQUIRE( pView->getSizingInfoUpdateCount()==1);			

				END_ASYNC_TEST();
			});
	}

	SECTION("changeViewProperty")
	{
		SECTION("visible")
		{
			testViewOp<ViewType>(
				pView,
				[pView]()
				{
					pView->visible() = !shouldViewBeInitiallyVisible<ViewType>();
				},
				[pCore, pView]()
				{
					REQUIRE( pCore->getVisibleChangeCount()==1 );
					REQUIRE( pCore->getVisible() == !shouldViewBeInitiallyVisible<ViewType>() );	
				},
				0	// should NOT have caused a sizing info update
				);
		}
	
		SECTION("margin")
		{
			UiMargin m(UiLength::Unit::sem, 1, 2, 3, 4);

			testViewOp<ViewType>( 
				pView,
				[pView, m]()
				{
					pView->margin() = m;
				},
				[pCore, m, pView]()
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

			testViewOp<ViewType>( 
				pView,
				[pView, m]()
				{
					pView->padding() = m;
				},
				[pCore, m, pView]()
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

			testViewOp<ViewType>( 
				pView,
				[pView, b]()
				{
					pView->bounds() = b;
				},
				[pCore, b, pView]()
				{
					REQUIRE( pCore->getBoundsChangeCount()==1 );
					REQUIRE( pCore->getBounds() == b);
				},
				0	// should NOT have caused a sizing info update
				);
		}
	}


	SECTION("multiplePropertyChangesThatInfluenceSizing")
	{
		testViewOp<ViewType>(
			pView,

			[pView]()
			{
				pView->padding() = UiMargin(UiLength::Unit::sem, 7, 8, 9, 10);
				pView->padding() = UiMargin(UiLength::Unit::sem, 6, 7, 8, 9);
			},

			[pCore, pView]()
			{
				// padding changed twice
				REQUIRE( pCore->getPaddingChangeCount()==2 );
				REQUIRE( pCore->getPadding() == UiMargin(UiLength::Unit::sem, 6, 7, 8, 9));
			},

			1	// should cause a single(!) sizing info update

			);		
	}
}



}
}

#endif