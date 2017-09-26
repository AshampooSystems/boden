#ifndef BDN_TEST_testView_H_
#define BDN_TEST_testView_H_


#include <bdn/test/MockUiProvider.h>
#include <bdn/test/ViewWithTestExtensions.h>
#include <bdn/test/testCalcPreferredSize.h>
#include <bdn/test/MockViewCore.h>

namespace bdn
{
namespace test
{

template<class ViewType>
static void _initViewTestPreparerTestView(ViewType* pView)
{
	// do nothing by default
}
    
template<>
void _initViewTestPreparerTestView<TextView>(TextView* pView)
{
	// must have a text set so that the preferred size hint will have a measurable effect
	pView->text() = "hello world";
}

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
        P< ViewWithTestExtensions<ViewType> > pView = createViewWithoutParent();

        _pWindow->setContentView(pView);

        return pView;
    }
    
    P< ViewWithTestExtensions<ViewType> > createViewWithoutParent()
    {
        P< ViewWithTestExtensions<ViewType> > pView = newObj< ViewWithTestExtensions<ViewType> >();
        
        _initViewTestPreparerTestView<ViewType>(pView);
        
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
        return createViewWithoutParent();
    }
    
    P< ViewWithTestExtensions<Window> > createViewWithoutParent()
    {
        P<ViewWithTestExtensions<Window> > pWindow = newObj< ViewWithTestExtensions<Window> >( _pUiProvider );
        
        _initViewTestPreparerTestView<Window>(pWindow);
        
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




/** Used internally - do not use outside the Boden framework.*/
enum class ExpectedSideEffect_
{
    invalidateSizingInfo = 1 << 0,
    invalidateLayout = 1 << 1,
    invalidateParentLayout = 1 << 2
};

inline int operator | (ExpectedSideEffect_ left, ExpectedSideEffect_ right)
{
    return static_cast<int>(left) | static_cast<int>(right);
}

inline int operator | (int left, ExpectedSideEffect_ right)
{
    return left | static_cast<int>(right);
}

inline int operator & (int left, ExpectedSideEffect_ right)
{
    return left & static_cast<int>(right);
}


/** Used internally. Do not use outside the Boden framework.
    
    Helper function that performs an operation on a view object and verifies the result afterwards.

    The operation is performed twice: once from the main thread and once from another thread (if the target
    platform supports multithreading).

    _testViewOp may schedule parts of the test to be continued later
    after pending UI events have been handled (see CONTINUE_SECTION_WHEN_IDLE). If this happens then
    the _testViewOp function returns before the test has actually finished. The Boden test system will
    automatically manage the postponed continuation of the test, but the caller must be aware that local
    objects that are used by the opFunc and verifyFunc might not be there anymore at that point in time.
    To solve this problem the caller can pass an opaque object in pKeepAliveDuringContinuations
    that is kept alive until all test continuations have finished. If only one object is needed by opFunc and
    verifyFunc then it can be passed in pKeepAliveDuringContinuations directly. If multiple objects need to
    be kept alive then one should add them to a container and then pass the container in pKeepAliveDuringContinuations.
    If no objects need to be kept alive artificially then the caller can simply pass null for pKeepAliveDuringContinuations.

    Note that pView is automatically kept alive during the test. It is not necessary to pass it in pKeepAliveDuringContinuations.

    @param pView the view to perform the operation on
    @param pKeepAliveDuringContinuations an arbitrary object that is kept alive during the test.
        This can be used to ensure that resources needed by opFunc and verifyFunc remain alive, even
        if the test is continued asynchronously (see above for more information).
        pKeepAliveDuringContinuations can be null if it is not needed.
    @param opFunc a function object that performs the action on the view
    @param verifyFunc a function object that verifies that the view is in the expected state after the action.
    @param expectedSideEffects a combination of flags from the bdn::ExpectedSideEffect_ enumeration. The flags
        indicate what kind of side effect the operation should have on the layout system.
*/
template<class ViewType>
inline void _testViewOp(P< ViewWithTestExtensions<ViewType> > pView,
                P<IBase> pKeepAliveDuringContinuations,
                std::function<void()> opFunc,
                std::function<void()> verifyFunc,
                int expectedSideEffects )
{
    // schedule the test asynchronously, so that the initial sizing
	// info update from the view construction is already done.
        
    ASYNC_SECTION("mainThread", pView, opFunc, verifyFunc, pKeepAliveDuringContinuations, expectedSideEffects)
	{
        int initialNeedLayoutCount = cast<MockViewCore>(pView->getViewCore())->getNeedLayoutCount();
        int initialSizingInvalidatedCount = cast<MockViewCore>(pView->getViewCore())->getInvalidateSizingInfoCount();

        P<View> pParent = pView->getParentView();
        int initialParentLayoutInvalidatedCount = 0;
        int initialParentLayoutCount = 0;
        if(pParent!=nullptr)
        {
            initialParentLayoutInvalidatedCount = cast<MockViewCore>(pParent->getViewCore())->getNeedLayoutCount();
            initialParentLayoutCount = cast<MockViewCore>(pParent->getViewCore())->getLayoutCount();
        }

		opFunc();

        // the results of the change may depend on notification calls. Those are posted
        // to the main event queue. So we need to process those now before we verify.
        BDN_CONTINUE_SECTION_WHEN_IDLE(pView, pParent, pKeepAliveDuringContinuations, initialNeedLayoutCount, initialSizingInvalidatedCount, initialParentLayoutInvalidatedCount, initialParentLayoutCount, expectedSideEffects, verifyFunc)
        {
		    verifyFunc();

            bool expectSizingInfoInvalidation = (expectedSideEffects & ExpectedSideEffect_::invalidateSizingInfo)!=0;            
            bool expectParentLayoutInvalidation = (expectedSideEffects & ExpectedSideEffect_::invalidateParentLayout)!=0;
            bool expectLayoutInvalidation = (expectedSideEffects & ExpectedSideEffect_::invalidateLayout)!=0;

            // if the parent layout is being invalidated then that can cause the view's size to change, which can also trigger
            // a child layout invalidation. So if the parent layout is invalidated then we allow (but do not require)
            // the child layout to be invalidated as wenn.
            bool mightInvalidateLayout = expectParentLayoutInvalidation || expectLayoutInvalidation;

            BDN_REQUIRE( (cast<MockViewCore>(pView->getViewCore())->getInvalidateSizingInfoCount() > initialSizingInvalidatedCount) == expectSizingInfoInvalidation );

            if( expectLayoutInvalidation )
                BDN_REQUIRE( cast<MockViewCore>(pView->getViewCore())->getNeedLayoutCount() > initialNeedLayoutCount );
            else if(!mightInvalidateLayout)
                BDN_REQUIRE( cast<MockViewCore>(pView->getViewCore())->getNeedLayoutCount() == initialNeedLayoutCount );

            // if the parent should be invalidated and the view is not a top level
            // window then it MUST have a parent.
            if(expectParentLayoutInvalidation && tryCast<Window>(pView)==nullptr )
                REQUIRE( pParent!=nullptr );

            if(pParent!=nullptr)
            {
                BDN_REQUIRE( ( cast<MockViewCore>(pParent->getViewCore())->getNeedLayoutCount()>initialParentLayoutInvalidatedCount ) == expectParentLayoutInvalidation );
                
                // verify that the layout was only updates once in total (if an update was expected)
                BDN_REQUIRE( cast<MockViewCore>(pParent->getViewCore())->getLayoutCount() == initialParentLayoutCount + (expectParentLayoutInvalidation ? 1 : 0) );
            }
        };
	};

#if BDN_HAVE_THREADS

    // schedule the test asynchronously, so that the layout update from the view construction is already done.
	ASYNC_SECTION("otherThread", pView, pKeepAliveDuringContinuations, opFunc, verifyFunc, expectedSideEffects)
	{
        int initialNeedLayoutCount = cast<MockViewCore>(pView->getViewCore())->getNeedLayoutCount();
        int initialSizingInvalidatedCount = cast<MockViewCore>(pView->getViewCore())->getInvalidateSizingInfoCount();

        P<View> pParent = pView->getParentView();
        int initialParentLayoutInvalidatedCount = 0;
        int initialParentLayoutCount = 0;
        if(pParent!=nullptr)
        {
            initialParentLayoutInvalidatedCount = cast<MockViewCore>(pParent->getViewCore())->getNeedLayoutCount();
            initialParentLayoutCount = cast<MockViewCore>(pParent->getViewCore())->getLayoutCount();
        }

		// note that we call get on the future object, so that we wait until the
		// other thread has finished (so that any changes have been scheduled)
		Thread::exec( opFunc ).get();		

		// any changes made to properties by the opFunc are asynchronously scheduled.
		// So they have not actually been made in the core yet,

		// we want to wait until the changes have actually been made in the core.
		// So do another async call. That one will be executed after the property
		// changes.

        BDN_CONTINUE_SECTION_WHEN_IDLE( pView, pParent, pKeepAliveDuringContinuations, verifyFunc, initialNeedLayoutCount, initialSizingInvalidatedCount, initialParentLayoutInvalidatedCount, initialParentLayoutCount, expectedSideEffects )
		{
            // the results of the change may depend on notification calls. Those are posted
            // to the main event queue. So we need to process those now before we verify.
            BDN_CONTINUE_SECTION_WHEN_IDLE(pView, pParent, pKeepAliveDuringContinuations, initialNeedLayoutCount, initialSizingInvalidatedCount, initialParentLayoutInvalidatedCount, initialParentLayoutCount, expectedSideEffects, verifyFunc)
            {
		        verifyFunc();

                bool expectSizingInfoInvalidation = (expectedSideEffects & ExpectedSideEffect_::invalidateSizingInfo)!=0;
                bool expectLayoutInvalidation = (expectedSideEffects & ExpectedSideEffect_::invalidateLayout)!=0;
                bool expectParentLayoutInvalidation = (expectedSideEffects & ExpectedSideEffect_::invalidateParentLayout)!=0;

                 // if the parent layout is being invalidated then that can cause the view's size to change, which can also trigger
                // a child layout invalidation. So if the parent layout is invalidated then we allow (but do not require)
                // the child layout to be invalidated as wenn.
                bool mightInvalidateLayout = expectParentLayoutInvalidation || expectLayoutInvalidation;

		        BDN_REQUIRE( ( cast<MockViewCore>(pView->getViewCore())->getInvalidateSizingInfoCount() > initialSizingInvalidatedCount ) == expectSizingInfoInvalidation );

                if( expectLayoutInvalidation )
                    BDN_REQUIRE( cast<MockViewCore>(pView->getViewCore())->getNeedLayoutCount() > initialNeedLayoutCount );
                else if(!mightInvalidateLayout)
                    BDN_REQUIRE( cast<MockViewCore>(pView->getViewCore())->getNeedLayoutCount() == initialNeedLayoutCount );


                // if the parent should be invalidated and the view is not a top level
                // window then it MUST have a parent.
                if(expectParentLayoutInvalidation && tryCast<Window>(pView)==nullptr )
                    REQUIRE( pParent!=nullptr );

                if(pParent!=nullptr)
                {
                    BDN_REQUIRE( ( cast<MockViewCore>(pParent->getViewCore())->getNeedLayoutCount() > initialParentLayoutInvalidatedCount ) == expectParentLayoutInvalidation );

                    // verify that the layout was only updates once in total (if an update was expected)
                    BDN_REQUIRE( cast<MockViewCore>(pParent->getViewCore())->getLayoutCount() == initialParentLayoutCount + (expectParentLayoutInvalidation ? 1 : 0) );
                }
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
    
    SECTION("without parent")
    {
        // we test the view before the core is created (i.e. before it has a parent).
        P<ViewType> pView = pPreparer->createViewWithoutParent();
        
        if( tryCast<Window>(pView) == nullptr)
        {
            SECTION("core null")
                REQUIRE( pView->getViewCore()==nullptr );
        }
        
        SECTION("parent null")
            REQUIRE( pView->getParentView()==nullptr );
        
        SECTION("calcPreferredSize")
            REQUIRE( pView->calcPreferredSize() == Size(0,0) );
        
        SECTION("adjustBounds")
            REQUIRE( pView->adjustBounds( Rect(1, 2, 3, 4), RoundType::nearest, RoundType::nearest ) == Rect(1, 2, 3, 4) );
        
        SECTION("adjustAndSetBounds")
        {
            REQUIRE( pView->adjustAndSetBounds(Rect(1, 2, 3, 4) ) == Rect(1, 2, 3, 4) );
            REQUIRE( pView->position() == Point(1, 2) );
            REQUIRE( pView->size() == Size(3, 4) );
        }
    }

	P< ViewWithTestExtensions<ViewType> > pView = pPreparer->createView();
	BDN_REQUIRE( pPreparer->getUiProvider()->getCoresCreated()==initialCoresCreated+1 );

	P<bdn::test::MockViewCore> pCore = cast<bdn::test::MockViewCore>( pView->getViewCore() );
	BDN_REQUIRE( pCore!=nullptr );

    P<Window> pWindow = pPreparer->getWindow();

    
    // Normally the default for a view's visible property is true.
    // But for top level Windows, for example, the default is false. This is a change that is done in the constructor
    // of the Window object. At that point there are no subscribers for the property's change event, BUT a notification
    // is still scheduled. And if there are subscribers at the point when the notification
    // is actually handled then we will get a visibility change recorded. So the expected
    // value here is 1.
    // In general that means that we expect a visible change count of 0 for views that
    // are initially visible and a change count of 1 for those that are initially invisible.
    int initialVisibleChangeCount = shouldViewBeInitiallyVisible<ViewType>() ? 0 : 1;
    
    BDN_CONTINUE_SECTION_WHEN_IDLE( pPreparer, initialCoresCreated, pWindow, pView, pCore, initialVisibleChangeCount )
    {
	    SECTION("initialViewState")
	    {
            // the core should initialize its properties from the outer window when it is created.
		    // The outer window should not set them manually after construction.		
            BDN_REQUIRE( pCore->getPaddingChangeCount()==0 );
		    BDN_REQUIRE( pCore->getParentViewChangeCount()==0 );

            BDN_REQUIRE( pCore->getVisibleChangeCount()==initialVisibleChangeCount );
            
            BDN_REQUIRE( pView->visible() == shouldViewBeInitiallyVisible<ViewType>() );
		    
            BDN_REQUIRE( pView->margin() == UiMargin( UiLength() ) );
		    BDN_REQUIRE( pView->padding().get().isNull() );

		    BDN_REQUIRE( pView->horizontalAlignment() == View::HorizontalAlignment::left );
		    BDN_REQUIRE( pView->verticalAlignment() == View::VerticalAlignment::top );

            BDN_REQUIRE( pView->preferredSizeHint() == Size::none() );
            BDN_REQUIRE( pView->preferredSizeMinimum() == Size::none() );
            BDN_REQUIRE( pView->preferredSizeMaximum() == Size::none() );

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

	    }

        SECTION("invalidateSizingInfo calls core")
        {
            int callCountBefore = pCore->getInvalidateSizingInfoCount();

            pView->invalidateSizingInfo( View::InvalidateReason::customDataChanged );

            REQUIRE( pCore->getInvalidateSizingInfoCount() == callCountBefore + 1);
        }

        SECTION("invalidateSizingInfo notifies parent")
        {
            P<View>         pParent = pView->getParentView();
            if(pParent!=nullptr)
            {
                P<MockViewCore> pParentCore = cast<MockViewCore>( pParent->getViewCore() );

                int childSizingInfoInvalidatedCount = pParentCore->getChildSizingInfoInvalidatedCount();
                int parentSizingInvalidatedCount = pParentCore->getInvalidateSizingInfoCount();
                int parentNeedLayoutCount = pParentCore->getNeedLayoutCount();

                pView->invalidateSizingInfo( View::InvalidateReason::customDataChanged );

                // should have invalidated the parent sizing info and layout via a childSizingInfoInvalidated call.
                REQUIRE( pParentCore->getChildSizingInfoInvalidatedCount() == childSizingInfoInvalidatedCount+1 );
                REQUIRE( pParentCore->getInvalidateSizingInfoCount() == parentSizingInvalidatedCount+1 );
                REQUIRE( pParentCore->getNeedLayoutCount() == parentNeedLayoutCount+1 );
            }
        }

        SECTION("preferred size caching (finite space)")
        {
            // fill cache
            Size prefSize = pView->calcPreferredSize( Size(1000, 2000) );

            int calcCountBefore = pCore->getCalcPreferredSizeCount();

            // sanity check - should not call view's calcPreferredSize
            Size prefSize2 = pView->calcPreferredSize( Size(1000, 2000) );
            REQUIRE( pCore->getCalcPreferredSizeCount() == calcCountBefore );
            REQUIRE( prefSize2 == prefSize );

            SECTION("invalidateSizingInfo")
            {
                pView->invalidateSizingInfo( View::InvalidateReason::customDataChanged );

                // now call calc again. This time the size should be freshly calculated again
                Size prefSize3 = pView->calcPreferredSize( Size(1000, 2000) );
                REQUIRE( pCore->getCalcPreferredSizeCount() == calcCountBefore+1 );
                REQUIRE( prefSize3 == prefSize);
            }

            SECTION("different available space")
            {
                pView->calcPreferredSize( Size(1001, 2000) );
                REQUIRE( pCore->getCalcPreferredSizeCount() == calcCountBefore+1 );
                pView->calcPreferredSize( Size(1000, 2001) );
                REQUIRE( pCore->getCalcPreferredSizeCount() == calcCountBefore+2 );
            }
        }

        SECTION("preferred size caching (infinite space)")
        {
            // fill cache
            Size prefSize = pView->calcPreferredSize( Size::none() );

            int calcCountBefore = pCore->getCalcPreferredSizeCount();

            SECTION("same infinite space")
            {
                Size prefSize2 = pView->calcPreferredSize( Size::none() );
                REQUIRE( pCore->getCalcPreferredSizeCount() == calcCountBefore );
                REQUIRE( prefSize2 == prefSize );
            }

            SECTION("prefSize")
            {
                // specifying an available space > prefSize should return prefSize from the cache
                Size prefSize2 = pView->calcPreferredSize( prefSize );
                REQUIRE( pCore->getCalcPreferredSizeCount() == calcCountBefore );
                REQUIRE( prefSize2 == prefSize );
            }

            SECTION("width smaller than prefSize")
            {
                pView->calcPreferredSize( prefSize - Size(1,0) );
                // should calculate a new preferred size
                REQUIRE( pCore->getCalcPreferredSizeCount() == calcCountBefore+1 );
            }

            SECTION("height smaller than prefSize")
            {
                pView->calcPreferredSize( prefSize - Size(0,1) );
                // should calculate a new preferred size
                REQUIRE( pCore->getCalcPreferredSizeCount() == calcCountBefore+1 );
            }

        }


        SECTION("invalidateSizingInfo")
        {
            _testViewOp( pView,
                        pPreparer,
                        [pView]()
                        {
                            pView->invalidateSizingInfo( View::InvalidateReason::customDataChanged );                            
                        },
                        []()
                        {
                        },
                        ExpectedSideEffect_::invalidateSizingInfo
                        | ExpectedSideEffect_::invalidateParentLayout // parent layout is invalidated since sizing info changed
                            );
        }


        SECTION("needLayout")
        {
            int callCountBefore = pCore->getNeedLayoutCount();

            _testViewOp( pView,
                        pPreparer,
                        [pView]()
                        {
                            pView->needLayout( View::InvalidateReason::customDataChanged );
                        },
                        []()
                        {
                        },
                        0 | ExpectedSideEffect_::invalidateLayout );
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

            CONTINUE_SECTION_WHEN_IDLE(pView, pPreparer)
            {                
                BDN_REQUIRE( pView->getParentView() == nullptr);	    
            };
	    }


	    SECTION("changeViewProperty")
	    {
		    SECTION("visible")
		    {
			    _testViewOp<ViewType>(
				    pView,
                    pPreparer,
				    [pView, pWindow]()
				    {
					    pView->visible() = !shouldViewBeInitiallyVisible<ViewType>();
				    },
				    [pCore, pView, pWindow, initialVisibleChangeCount]()
				    {
					    BDN_REQUIRE( pCore->getVisibleChangeCount()==initialVisibleChangeCount+1 );
					    BDN_REQUIRE( pCore->getVisible() == !shouldViewBeInitiallyVisible<ViewType>() );	
				    },
				    0   // no layout invalidations
				    );
		    }
	
		    SECTION("margin")
		    {
			    UiMargin m( UiLength::sem(1),
                            UiLength::sem(2),
                            UiLength::sem(3),
                            UiLength::sem(4) );

			    _testViewOp<ViewType>( 
				    pView,
                    pPreparer,
				    [pView, m, pWindow]()
				    {
					    pView->margin() = m;
				    },
				    [pCore, m, pView, pWindow]()
				    {
                        BDN_REQUIRE( pCore->getMarginChangeCount()==1 );
					    BDN_REQUIRE( pCore->getMargin() == m);

                        // margin property should still have the value we set
                        REQUIRE( pView->margin().get() == m );
				    },
				    0 | ExpectedSideEffect_::invalidateParentLayout
                        // should NOT have caused a sizing info update, since the view's
                        // preferred size does not depend on its margin
				    );
		    }

		    SECTION("padding")
		    {
			    UiMargin m(UiLength::sem(1), UiLength::sem(2), UiLength::sem(3), UiLength::sem(4) );

			    _testViewOp<ViewType>( 
				    pView,
                    pPreparer,
				    [pView, m, pWindow]()
				    {
					    pView->padding() = m;
				    },
				    [pCore, m, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getPaddingChangeCount()==1 );
					    BDN_REQUIRE( pCore->getPadding() == m);
				    },
                    ExpectedSideEffect_::invalidateSizingInfo // should have caused sizing info to be invalidated                    
                    | ExpectedSideEffect_::invalidateParentLayout // should cause a parent layout update since sizing info was invalidated
                    | ExpectedSideEffect_::invalidateLayout // should have caused layout to be invalidated
				    );
		    }


            SECTION("horizontalAlignment")
		    {
			    _testViewOp<ViewType>( 
				    pView,
                    pPreparer,
				    [pView, pWindow]()
				    {
					    pView->horizontalAlignment() = View::HorizontalAlignment::center;
				    },
				    [pCore, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getHorizontalAlignmentChangeCount()==1 );
					    BDN_REQUIRE( pCore->getHorizontalAlignment() == View::HorizontalAlignment::center);
				    },
                    0 | ExpectedSideEffect_::invalidateParentLayout
				    // should not have caused sizing info to be invalidated
                    // but should have invalidated the parent layout
				    );
		    }

            SECTION("verticalAlignment")
		    {
			    _testViewOp<ViewType>( 
				    pView,
                    pPreparer,
				    [pView, pWindow]()
				    {
					    pView->verticalAlignment() = View::VerticalAlignment::bottom;
				    },
				    [pCore, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getVerticalAlignmentChangeCount()==1 );
					    BDN_REQUIRE( pCore->getVerticalAlignment() == View::VerticalAlignment::bottom);
				    },
                    0 | ExpectedSideEffect_::invalidateParentLayout
				    // should not have caused sizing info to be invalidated
                    // but should have invalidated the parent layout
				    );
		    }

            SECTION("preferredSizeMinimum")
		    {
			    _testViewOp<ViewType>( 
				    pView,
                    pPreparer,
				    [pView, pWindow]()
				    {
					    pView->preferredSizeMinimum() = Size(10,20);
				    },
				    [pCore, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getPreferredSizeMinimumChangeCount()==1 );
					    BDN_REQUIRE( pCore->getPreferredSizeMinimum() == Size(10,20) );
				    },
				    ExpectedSideEffect_::invalidateSizingInfo // should have caused sizing info to be invalidated
                    | ExpectedSideEffect_::invalidateParentLayout // should cause a parent layout update since sizing info was invalidated
				    );
		    }

            SECTION("preferredSizeMaximum")
		    {
			    _testViewOp<ViewType>( 
				    pView,
                    pPreparer,
				    [pView, pWindow]()
				    {
					    pView->preferredSizeMaximum() = Size(10,20);
				    },
				    [pCore, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getPreferredSizeMaximumChangeCount()==1 );
					    BDN_REQUIRE( pCore->getPreferredSizeMaximum() == Size(10,20) );
				    },
				    ExpectedSideEffect_::invalidateSizingInfo // should have caused sizing info to be invalidated
                    | ExpectedSideEffect_::invalidateParentLayout // should cause a parent layout update since sizing info was invalidated
				    );
		    }

            SECTION("preferredSizeHint")
		    {
			    _testViewOp<ViewType>( 
				    pView,
                    pPreparer,
				    [pView, pWindow]()
				    {
					    pView->preferredSizeHint() = Size(10,20);
				    },
				    [pCore, pView, pWindow]()
				    {
					    BDN_REQUIRE( pCore->getPreferredSizeHintChangeCount()==1 );
					    BDN_REQUIRE( pCore->getPreferredSizeHint() == Size(10,20) );
				    },
				    ExpectedSideEffect_::invalidateSizingInfo // should have caused sizing info to be invalidated
                    | ExpectedSideEffect_::invalidateParentLayout // should cause a parent layout update since sizing info was invalidated
				    );
		    }

		    SECTION("adjustAndSetBounds")
		    {
                SECTION("no need to adjust")
                {
			        Rect bounds(1, 2, 3, 4);

                    int boundsChangeCountBefore = pCore->getBoundsChangeCount();

			        _testViewOp<ViewType>( 
				        pView,
                        pPreparer,
				        [pView, bounds, pWindow]()
				        {
					        Rect adjustedBounds = pView->adjustAndSetBounds(bounds);
                            REQUIRE( adjustedBounds==bounds );
				        },
				        [pCore, bounds, pView, pWindow, boundsChangeCountBefore]()
				        {
					        BDN_REQUIRE( pCore->getBoundsChangeCount()==boundsChangeCountBefore+1 );
					        BDN_REQUIRE( pCore->getBounds() == bounds );

                            // the view's position and size properties should reflect the new bounds
					        BDN_REQUIRE( pView->position() == bounds.getPosition() );
                            BDN_REQUIRE( pView->size() == bounds.getSize() );
				        },
				        0 | ExpectedSideEffect_::invalidateLayout
                            // should NOT have caused a sizing info update
                            // should not cause a parent layout update
                            // should cause a layout update (since size was changed)
				        );
                }

                SECTION("need adjustment")
                {
                    Rect bounds(1.3, 2.4, 3.1, 4.9);

                    // the mock view uses 3 pixels per DIP. Coordinates should be rounded to the
                    // NEAREST value
                    Rect expectedAdjustedBounds( 1+1.0/3, 2 + 1.0/3, 3, 5 );

                    int boundsChangeCountBefore = pCore->getBoundsChangeCount();

			        _testViewOp<ViewType>( 
				        pView,
                        pPreparer,
				        [pView, bounds, expectedAdjustedBounds, pWindow]()
				        {
					        Rect adjustedBounds = pView->adjustAndSetBounds(bounds);                            
                            REQUIRE( adjustedBounds==expectedAdjustedBounds );
				        },
				        [pCore, bounds, expectedAdjustedBounds, pView, pWindow, boundsChangeCountBefore]()
				        {
					        BDN_REQUIRE( pCore->getBoundsChangeCount()==boundsChangeCountBefore+1 );
                            BDN_REQUIRE( pCore->getBounds()==expectedAdjustedBounds );

                            // the view's position and size properties should reflect the new, adjusted bounds
					        BDN_REQUIRE( pView->position() == expectedAdjustedBounds.getPosition() );
                            BDN_REQUIRE( pView->size() == expectedAdjustedBounds.getSize() );
				        },
				        0 | ExpectedSideEffect_::invalidateLayout
                            // should NOT have caused a sizing info update
                            // should not a parent layout update
                            // should cause a layout update (since size was changed)
				        );

                }
		    }


            SECTION("adjustBounds")
		    {
                SECTION("no need to adjust")
                {
			        Rect bounds(1, 2, 3, 4);
                    Rect origBounds = pCore->getBounds();

                    std::list<RoundType> roundTypes{RoundType::nearest, RoundType::up, RoundType::down};

                    for(RoundType positionRoundType: roundTypes)
                    {
                        for(RoundType sizeRoundType: roundTypes)
                        {
                            SECTION( "positionRoundType: "+std::to_string((int)positionRoundType)+", "+std::to_string((int)sizeRoundType) )
                            {
                                Rect adjustedBounds = pView->adjustBounds(bounds, positionRoundType, sizeRoundType);

                                // no adjustments are necessary. So we should always get out the same that we put in
                                REQUIRE( adjustedBounds==bounds );

                                // view properties should not have changed
                                REQUIRE( pView->position() == origBounds.getPosition() );
                                REQUIRE( pView->size() == origBounds.getSize() );

                                // the core bounds should not have been updated
                                REQUIRE( pCore->getBounds() == origBounds );
                            }
                        }
                    }
                }

                SECTION("need adjustments")
                {
			        Rect bounds(1.3, 2.4, 3.1, 4.9);
                    Rect origBounds = pCore->getBounds();

                    std::list<RoundType> roundTypes{RoundType::nearest, RoundType::up, RoundType::down};

                    for(RoundType positionRoundType: roundTypes)
                    {
                        for(RoundType sizeRoundType: roundTypes)
                        {
                            SECTION( "positionRoundType: "+std::to_string((int)positionRoundType)+", "+std::to_string((int)sizeRoundType) )
                            {
                                Rect adjustedBounds = pView->adjustBounds(bounds, positionRoundType, sizeRoundType);

                                Point expectedPos;
                                if(positionRoundType==RoundType::down)
                                    expectedPos = Point(1, 2 + 1.0/3);
                                else if(positionRoundType==RoundType::up)
                                    expectedPos = Point(1 + 1.0/3, 2 + 2.0/3);
                                else
                                    expectedPos = Point(1 + 1.0/3, 2 + 1.0/3);

                                Size expectedSize;
                                if(sizeRoundType==RoundType::down)
                                    expectedSize = Size(3, 4+2.0/3);
                                else if(sizeRoundType==RoundType::up)
                                    expectedSize = Size(3+1.0/3, 5);
                                else
                                    expectedSize = Size(3, 5);

                                REQUIRE( adjustedBounds==Rect(expectedPos, expectedSize) );

                                // view properties should not have changed
                                REQUIRE( pView->position() == origBounds.getPosition() );
                                REQUIRE( pView->size() == origBounds.getSize() );

                                // the core bounds should not have been updated
                                REQUIRE( pCore->getBounds() == origBounds );
                            }
                        }
                    }
                }

            }
	    }

        SECTION("preferredSize")
            bdn::test::_testCalcPreferredSize<ViewType, View>(pView, pView, pPreparer);

	    SECTION("multiplePropertyChangesThatInfluenceSizing")
	    {
		    _testViewOp<ViewType>(
			    pView,
                pPreparer,
			    [pView, pWindow]()
			    {
				    pView->padding() = UiMargin(UiLength::sem(7), UiLength::sem(8), UiLength::sem(9), UiLength::sem(10));
				    pView->padding() = UiMargin(UiLength::sem(6), UiLength::sem(7), UiLength::sem(8), UiLength::sem(9) );
			    },

			    [pCore, pView, pWindow]()
			    {
				    // padding changed twice
				    BDN_REQUIRE( pCore->getPaddingChangeCount()==2 );
				    BDN_REQUIRE( pCore->getPadding() == UiMargin(UiLength::sem(6), UiLength::sem(7), UiLength::sem(8), UiLength::sem(9) ));
			    },


                ExpectedSideEffect_::invalidateSizingInfo // should have caused sizing info to be invalidated
                    | ExpectedSideEffect_::invalidateParentLayout // should cause a parent layout update since sizing info was invalidated
                    | ExpectedSideEffect_::invalidateLayout // should cause layout update
			    );		
	    }


#if BDN_HAVE_THREADS
        SECTION("core deinit called from main thread")
        {
            struct Data : public Base
            {
                P<ViewType> pView;
                P< ViewTestPreparer<ViewType> > pPreparer2;
            };

            P<Data> pData = newObj<Data>();

            pData->pPreparer2 = newObj< ViewTestPreparer<ViewType> >();

            pData->pView = pData->pPreparer2->createView();

            // the view should have a core
            REQUIRE( pData->pView->getViewCore()!=nullptr );

            CONTINUE_SECTION_IN_THREAD( pData )
            {
                // release the view and the preparer here.
                // That will cause the corresponding core to be deleted.
                // And the mock core object will verify that that happened
                // in the main thread.
                pData->pPreparer2 = nullptr;
                pData->pView = nullptr;
            };
        }

        SECTION("core initialized from main thread")
        {
            CONTINUE_SECTION_IN_THREAD()
            {
                P< ViewTestPreparer<ViewType> > pPreparer2 = newObj< ViewTestPreparer<ViewType> >();

                // create the view. The core creation should be moved to the main thread
                // automatically. The mock core constructor will verify this, so we will get
                // a failed REQUIRE here if the view calls the constructor from the main thread.
                P<ViewType> pView = pPreparer2->createView();
            };

        }
#endif

    };

}

}
}

#endif

