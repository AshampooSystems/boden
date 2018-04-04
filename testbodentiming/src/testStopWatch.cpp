#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/StopWatch.h>
#include <bdn/Thread.h>

using namespace bdn;


TEST_CASE("StopWatch")
{
    StopWatch watch;

    SECTION("noStart")
    {
        REQUIRE( watch.getMillis()>=0 );
        REQUIRE( watch.getMillis()<1000 );
        
        Thread::sleepMillis(1000);
        
        REQUIRE( watch.getMillis()>=1000 );
        REQUIRE( watch.getMillis()<2000 );
    }
    
    SECTION("restart")
    {
        Thread::sleepMillis(1000);
        
        REQUIRE( watch.getMillis()>=1000 );
        REQUIRE( watch.getMillis()<2000 );
        
        watch.start();
        
        REQUIRE( watch.getMillis()>=0 );
        REQUIRE( watch.getMillis()<1000 );
    }
}



