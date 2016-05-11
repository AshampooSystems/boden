#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/mainThread.h>
#include <bdn/Thread.h>
#include <bdn/StopWatch.h>

using namespace bdn;

TEST_CASE("callFromMainThread")
{
    StopWatch watch;
    
	SECTION("mainThread")
	{
		int callCount = 0;

		std::future<int> result = callFromMainThread( [&callCount](int x){ callCount++; return x*2; }, 42 );
        
        // should have been called immediately, since we are currently in the main thread
		REQUIRE( callCount==1 );

		REQUIRE( result.wait_for( std::chrono::milliseconds::duration(0)) == std::future_status::ready  );

		REQUIRE( result.get()==84 );
        
        // should not have waited at any point.
        REQUIRE( watch.getMillis()<1000 );
	}

	SECTION("otherThread")
	{
        SECTION("storingFuture")
        {
            Thread::exec(
                []()
                {
                    volatile int callCount = 0;

                    std::future<int> result = callFromMainThread(
                        [&callCount](int x)
                        {
                            // sleep a little to ensure that we have time to check callCount
                            Thread::sleepSeconds(1);
                            callCount++;
                            return x*2;
                        },
                        42 );


                    // should NOT have been called immediately, since we are in a different thread.
                    // Instead the call should have been deferred to the main thread.
                    REQUIRE( callCount==0 );
                    
                    StopWatch threadWatch;

                    REQUIRE( result.wait_for( std::chrono::milliseconds::duration(5000) ) == std::future_status::ready );
                    
                    REQUIRE( threadWatch.getMillis()>=500 );
                    REQUIRE( threadWatch.getMillis()<=5500 );

                    REQUIRE( callCount==1 );
                    
                    threadWatch.start();

                    REQUIRE( result.get()==84 );
                    
                    // should not have waited
                    REQUIRE( threadWatch.getMillis()<=500 );
                    
                    END_ASYNC_TEST();
                } );
        

            // time to start thread should have been less than 1000ms
            REQUIRE( watch.getMillis()<1000 );
            
            MAKE_ASYNC_TEST(10);
        }
        
        SECTION("notStoringFuture")
        {
            MAKE_ASYNC_TEST(10);
            
            Thread::exec(
                 []()
                 {
                     struct Data : public Base
                     {
                         volatile int callCount = 0;
                     };
                     
                     P<Data> pData = newObj<Data>();
                     
                     StopWatch threadWatch;
                     
                     callFromMainThread(   [pData](int x)
                                          {
                                              pData->callCount++;
                                              return x*2;
                                          },
                                          42 );
                     
                     
                     // should NOT have been called immediately, since we are in a different thread.
                     // Instead the call should have been deferred to the main thread.
                     REQUIRE( pData->callCount==0 );
                     
                     // should NOT have waited.
                     REQUIRE( threadWatch.getMillis()<1000 );
                     
                     END_ASYNC_TEST();
                 } );
            
            
            // time to start thread should have been less than 1000ms
            REQUIRE( watch.getMillis()<1000 );
            
            
            // wait a little
            Thread::sleepMillis(2000);
        }
    }
}

