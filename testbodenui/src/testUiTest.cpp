#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/mainThread.h>

#include <chrono>

using namespace bdn;


struct TestData : public Base
{
    int callCount = 0;
};



template<typename FuncType>
void testContinueSection( FuncType scheduleContinue )
{
    // we verify that CONTINUE_SECTION_ASYNC works as expected

    P<TestData> pData = newObj<TestData>();

    SECTION("notCalledImmediately")
    {
        scheduleContinue(
            [pData]()
            {
                pData->callCount++;            
            } );        

        // should not have been called yet
        REQUIRE( pData->callCount==0 );
    }


    SECTION("notCalledBeforeExitingInitialFunction")
    {
        scheduleContinue(
            [pData]()
            {
                pData->callCount++;            
            } );        

        // even if we wait a while, the continuation should not be called yet
        // (not even if it runs in another thread).
        Thread::sleepMillis(2000);
        REQUIRE( pData->callCount==0 );
    }


    static P<TestData> pCalledBeforeNextSectionData;
    SECTION("calledBeforeNextSection-a")
    {
        pCalledBeforeNextSectionData = pData;

        scheduleContinue(
            [pData]()
            {
                pData->callCount++;            
            } );        
    }

    SECTION("calledBeforeNextSection-b")
    {
        REQUIRE( pCalledBeforeNextSectionData!=nullptr );

        // the continuation of the previous section should have been called

        REQUIRE( pCalledBeforeNextSectionData->callCount==1 );
    }

    SECTION("notCalledMultipleTimes")
    {        
        scheduleContinue(
            [pData]()
            {
                pData->callCount++;            

                REQUIRE( pData->callCount==1 );
            } );
    }


    static int subSectionInContinuationMask=0;
    SECTION("subSectionInContinuation-a")
    {
        scheduleContinue(
            [scheduleContinue]()
            {
                SECTION("a")
                {
                    SECTION("a1")
                    {
                        subSectionInContinuationMask |= 1;
                    }

                    SECTION("a2")
                    {
                        subSectionInContinuationMask |= 2;
                    }
                }

                // add another continuation
                SECTION("b")
                {
                    scheduleContinue(
                        []()
                        {
                            SECTION("b1")
                            {
                                subSectionInContinuationMask |= 4;
                            }

                            SECTION("b2")
                            {
                                subSectionInContinuationMask |= 8;
                            }
                        } );
                }
            });
    }

    SECTION("subSectionInContinuation-b")
    {
        REQUIRE( subSectionInContinuationMask==15 );
    }

}

void testContinueSection_expectedFail( void (*scheduleContinue)(std::function<void()>) )
{    
    SECTION("exceptionInContinuation")
    {
        scheduleContinue(
            []()
            {
                throw std::runtime_error("dummy error");
            } );        
    }


    SECTION("failAfterContinuationScheduled")
    {
        scheduleContinue(
            []()
            {
            } );        

        REQUIRE(false);
    }
}


void scheduleContinueAsync( std::function<void()> continuationFunc )
{
    CONTINUE_SECTION_ASYNC(
        [continuationFunc]()
        {
            REQUIRE( Thread::isCurrentMain() );
            continuationFunc();
        } );    
}

TEST_CASE("CONTINUE_SECTION_ASYNC")
{
    testContinueSection( scheduleContinueAsync );
}

TEST_CASE("CONTINUE_SECTION_ASYNC-expectedFail", "[!shouldfail]")
{
    testContinueSection_expectedFail( scheduleContinueAsync );
}


void scheduleContinueInThread( std::function<void()> continuationFunc )
{
    CONTINUE_SECTION_IN_THREAD(
        [continuationFunc]()
        {
            REQUIRE( !Thread::isCurrentMain() );
            continuationFunc();
        } );    
}

TEST_CASE("CONTINUE_SECTION_IN_THREAD")
{
    testContinueSection( scheduleContinueInThread );
}

TEST_CASE("CONTINUE_SECTION_IN_THREAD-expectedFail", "[!shouldfail]")
{
    testContinueSection_expectedFail( scheduleContinueInThread );
}


