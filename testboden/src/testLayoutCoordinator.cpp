#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/Button.h>

#include <bdn/test/MockUiProvider.h>

using namespace bdn;

class LayoutCoordinatorForTesting : public LayoutCoordinator
{
public:

    int exceptionCount = 0;

protected:
    void handleException(const std::exception* pExceptionIfAvailable, const String& functionName) override
    {
        // we do not want exceptions to be logged. So we do not call the default implementation.
        exceptionCount++;
    }
};


class IDummyInterfaceForLayoutCoordinatorTestView_ : BDN_IMPLEMENTS IBase
{
public:

    // For functions like "autoSize" we have the problem that some base views
    // have them and some don't. If we do not mark the autoSize implementation in
    // LayoutCoordinatorTestView as override then we will get a warning if the base
    // has the function. If we do mark it as override then we will get an error
    // if the base does not have the function. This dummy interface ensures
    // that one base class (the dummy interface) always has the function. So override
    // is always correct and we do not need to do template specialization just for
    // the override keyword.
    
    virtual void autoSize()=0;
    virtual void center()=0;
    
};

template<class BaseView>
class LayoutCoordinatorTestView : public BaseView, BDN_IMPLEMENTS IDummyInterfaceForLayoutCoordinatorTestView_
{
public:
	template<typename ... ArgTypes>
    LayoutCoordinatorTestView(ArgTypes... args)
    : BaseView(args...)
    {        
    }

    void resetCalls()
    {
        layoutCalls=0;
        autoSizeCalls=0;
        centerCalls=0;
    }
    

    int layoutCalls = 0;
    bool layoutThrowsException = false;

	std::function<void()> layoutFunc;

    void layout() override
    {
        layoutCalls++;
		
		if(layoutFunc)
			layoutFunc();

        if(layoutThrowsException)
            throw InvalidArgumentError("bla");
    }


    int autoSizeCalls = 0;
    bool autoSizeThrowsException = false;

    void autoSize() override
    {
        autoSizeCalls++;

        if(autoSizeThrowsException)
            throw InvalidArgumentError("bla");
    }


    int centerCalls = 0;
    bool centerThrowsException = false;

    void center() override
    {
        centerCalls++;

        if(centerThrowsException)
            throw InvalidArgumentError("bla");
    }
};



TEST_CASE("LayoutCoordinator")
{
    P<LayoutCoordinatorForTesting> pCoord = newObj<LayoutCoordinatorForTesting>();
    
    P<bdn::test::MockUiProvider> pUiProvider = newObj<bdn::test::MockUiProvider>();

    P<LayoutCoordinatorTestView<Window> > pView1 = newObj<LayoutCoordinatorTestView<Window> >(pUiProvider);
    P<LayoutCoordinatorTestView<Window> > pView2 = newObj<LayoutCoordinatorTestView<Window> >(pUiProvider);
    P<LayoutCoordinatorTestView<Window> > pView3 = newObj<LayoutCoordinatorTestView<Window> >(pUiProvider);

    // wait for the initial updates by the global layout coordinator to be done
    CONTINUE_SECTION_WHEN_IDLE(pUiProvider, pCoord, pView1, pView2, pView3)
    {
        pView1->resetCalls();
        pView2->resetCalls();
        pView3->resetCalls();

		SECTION("exception")
		{        
			P<LayoutCoordinatorTestView<Window> > pCenterWindow = newObj<LayoutCoordinatorTestView<Window> >(pUiProvider);

			// pCenterWindow is always scheduled for centering. This is because window centering
			// is always done last, so this allows us to reasonably detect when an exception
			// causes as abort.
			pCoord->windowNeedsCentering( pCenterWindow );    

			SECTION("layout throws exception")
			{
				pView2->layoutThrowsException = true;

				pCoord->viewNeedsLayout( pView1 );
				pCoord->viewNeedsLayout( pView2 );
				pCoord->viewNeedsLayout( pView3 );

				CONTINUE_SECTION_WHEN_IDLE(pCoord, pView1, pView2, pView3, pCenterWindow)
				{
					REQUIRE( pView1->layoutCalls==1 );
					REQUIRE( pView2->layoutCalls==1 );
					REQUIRE( pView3->layoutCalls==1 );
					REQUIRE( pCenterWindow->centerCalls==1 );

					REQUIRE( pCoord->exceptionCount==1 );
				};
			}

			SECTION("autoSize throws exception")
			{
				pView2->autoSizeThrowsException = true;

				pCoord->windowNeedsAutoSizing( pView1 );
				pCoord->windowNeedsAutoSizing( pView2 );
				pCoord->windowNeedsAutoSizing( pView3 );

				CONTINUE_SECTION_WHEN_IDLE(pCoord, pView1, pView2, pView3, pCenterWindow)
				{
					REQUIRE( pView1->autoSizeCalls==1 );
					REQUIRE( pView2->autoSizeCalls==1 );
					REQUIRE( pView3->autoSizeCalls==1 );
					REQUIRE( pCenterWindow->centerCalls==1 );

					REQUIRE( pCoord->exceptionCount==1 );
				};
			}


			SECTION("center throws exception")
			{
				pView2->centerThrowsException = true;

				pCoord->windowNeedsCentering( pView1 );
				pCoord->windowNeedsCentering( pView2 );
				pCoord->windowNeedsCentering( pView3 );

				CONTINUE_SECTION_WHEN_IDLE(pCoord, pView1, pView2, pView3, pCenterWindow)
				{
					REQUIRE( pView1->centerCalls==1 );
					REQUIRE( pView2->centerCalls==1 );
					REQUIRE( pView3->centerCalls==1 );
					REQUIRE( pCenterWindow->centerCalls==1 );

					REQUIRE( pCoord->exceptionCount==1 );
				};
			}
		}


		SECTION("Pending events are handled between layouts")
		{
			// Layout operations are handled by the coordinator in "parent first" order.
			// Parents are layouted before their children, since the children's size might change
			// during the parent layout.
			// BUT that introduces a special case: when the size of a child view changes then
			// the child view will usually automatically schedule a layout in the corresponding
			// property change handler. That is also the correct behaviour. But the property change handler call
			// is posted to the event queue, so it does not happen immediately during the parent layout.
			// If the child was now already scheduled for layout, then it is important that the pending
			// change events are executed before the child's layout is started. Otherwise the duplicate layout
			// will not be detected and the child is layouted twice.
			// We test here that queued up events are executed between layout operations. That ensures that
			// no such duplicate layouts will happen.

			// note that for this test we need the two layout calls to happen in a defined
			// order. The order is only defined if one is on a deeper level, so we create a child view
			// inside the (unused) view3.

			P<LayoutCoordinatorTestView<Button> > pView4 = newObj<LayoutCoordinatorTestView<Button> >();

			pView3->setContentView(pView4);

			pView1->needLayout( View::InvalidateReason::customChange );
			pView4->needLayout( View::InvalidateReason::customChange );

			// layout order is parent-first, so view1 will always be layouted before view4

			pView1->layoutFunc =
				[pView1, pView2, pView3, pView4, pCoord]()
				{
					// view 4 should not have been layouted yet
					REQUIRE( pView4->layoutCalls==0);

					// and view 1 only once (the current call)
					REQUIRE( pView1->layoutCalls==1);

					// now schedule something to the event queue.
					// This simulates the property change notification.
					asyncCallFromMainThread(
						[pView1, pView2, pView3, pView4, pCoord]()
						{
							// view4 should still not have been layouted yet
							REQUIRE( pView4->layoutCalls==0);

							// and view1 still only once
							REQUIRE( pView1->layoutCalls==1);
						});
				};

			CONTINUE_SECTION_WHEN_IDLE(pView1, pView2, pView3, pView4, pCoord)
			{
				// verify that now all layout requests have been handled.
				REQUIRE( pView1->layoutCalls==1);
				REQUIRE( pView4->layoutCalls==1);
			};
		}
    };


}


