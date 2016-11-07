#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/LayoutCoordinator.h>

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

class LayoutCoordinatorTestView : public Window
{
public:

    void resetCalls()
    {
        updateSizingInfoCalls=0;        
        layoutCalls=0;
        autoSizeCalls=0;
        centerCalls=0;
    }

    int updateSizingInfoCalls = 0;
    bool updateSizingInfoThrowsException = false;

    void updateSizingInfo() override
    {
        updateSizingInfoCalls++;

        if(updateSizingInfoThrowsException)
            throw InvalidArgumentError("bla");
    }


    int layoutCalls = 0;
    bool layoutThrowsException = false;

    void layout() override
    {
        layoutCalls++;

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

    P<LayoutCoordinatorTestView> pView1 = newObj<LayoutCoordinatorTestView>();
    P<LayoutCoordinatorTestView> pView2 = newObj<LayoutCoordinatorTestView>();
    P<LayoutCoordinatorTestView> pView3 = newObj<LayoutCoordinatorTestView>();

    // wait for the initial updates by the global layout coordinator to be done
    CONTINUE_SECTION_WHEN_IDLE(pCoord, pView1, pView2, pView3)
    {
        pView1->resetCalls();
        pView2->resetCalls();
        pView3->resetCalls();
        
        P<LayoutCoordinatorTestView> pCenterWindow = newObj<LayoutCoordinatorTestView>();

        // pCenterWindow is always scheduled for centering. This is because window centering
        // is always done last, so this allows us to reasonably detect when an exception
        // causes as abort.
        pCoord->windowNeedsCentering( pCenterWindow );    

        SECTION("updateSizingInfo throws exception")
        {
            pView2->updateSizingInfoThrowsException = true;

            // the ordering of the coordinator is random. So we add one before and one after
            // to make it more likely that there is one more in the queue after the exception happens
            pCoord->viewNeedsSizingInfoUpdate( pView1 );
            pCoord->viewNeedsSizingInfoUpdate( pView2 );
            pCoord->viewNeedsSizingInfoUpdate( pView3 );

            CONTINUE_SECTION_WHEN_IDLE(pCoord, pView1, pView2, pView3, pCenterWindow)
            {
                REQUIRE( pView1->updateSizingInfoCalls==1 );
                REQUIRE( pView2->updateSizingInfoCalls==1 );
                REQUIRE( pView3->updateSizingInfoCalls==1 );

                REQUIRE( pCenterWindow->centerCalls==1 );

                REQUIRE( pCoord->exceptionCount==1 );
            };
        }

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
    };


}


