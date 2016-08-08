#include <bdn/init.h>

#include <bdn/Signal.h>

#include <bdn/test.h>
#include <bdn/StopWatch.h>

using namespace bdn;


void verifySignalState(Signal* pSignal, bool expectedSet)
{
    REQUIRE( pSignal->isSet()==expectedSet );

    StopWatch stopWatch;
    REQUIRE( pSignal->wait(0)==expectedSet );      
    // should not have waited
    REQUIRE( stopWatch.getMillis()<500 );

    stopWatch.start();
    REQUIRE( pSignal->wait(1000)==expectedSet );

    if(expectedSet)
    {
        // should not have waited
        REQUIRE( stopWatch.getMillis()<500 );        
    }
    else
    {
        // should have waited about 1000ms
        REQUIRE( stopWatch.getMillis()>990 );        
    }

    // the wait functions must not have modified the state.
    REQUIRE( pSignal->isSet() == expectedSet );
}


TEST_CASE("Signal")
{
    P<Signal> pSignal = newObj<Signal>();

    SECTION("initialState")
        verifySignalState(pSignal, false);        

    SECTION("sameThread")
    {
        SECTION("setWaitClear")
        {
            pSignal->set();
            verifySignalState(pSignal, true);            

            pSignal->clear();
            verifySignalState(pSignal, false);            
        }

        SECTION("pulseOne")
        {
            pSignal->pulseOne();
            // should have no effect if no one is currently waiting
            verifySignalState(pSignal, false);            
        }

        SECTION("pulseAll")
        {
            pSignal->pulseAll();
            // should have no effect if no one is currently waiting
            verifySignalState(pSignal, false);            
        }

        SECTION("pulseOneWhileSet")
        {
            pSignal->set();
            pSignal->pulseOne();
            // should have reset the signal
            verifySignalState(pSignal, false);            
        }

        SECTION("pulseAllWhileSet")
        {
            pSignal->set();
            pSignal->pulseAll();
            // should have reset the signal
            verifySignalState(pSignal, false);            
        }
    }

#if BDN_HAVE_THREADS
    SECTION("otherThread")
    {
        SECTION("setWait")
        {
            Thread::exec( 
                [pSignal]()
                {
                    Thread::sleepMillis(1000);

                    pSignal->set();            
                } );

            // should not yet be set
            verifySignalState(pSignal, false);

            StopWatch stopWatch;
            REQUIRE( pSignal->wait(5000) );
            // should have waited about 1000ms
            REQUIRE( stopWatch.getMillis()>500 );
            REQUIRE( stopWatch.getMillis()<1500 );
        }

        SECTION("pulseOne")
        {
            std::atomic<int> signalledCount;

            std::list< std::future<void> > futureList;
            for(int i=0; i<10; i++)
            {
                futureList.push_back(
                    Thread::exec( 
                        [pSignal, &signalledCount]()
                        {
                            Thread::sleepMillis(1000);
    
                            if(pSignal->wait(10000))
                                signalledCount++;
                        } ) );
            }

            Thread::sleepMillis(4000);

            pSignal->pulseOne();

            Thread::sleepMillis(1000);

            // one thread should have woken up
            REQUIRE( signalledCount==1 );

            // wait for all threads to finish.
            for( auto& f: futureList)
                f.get();

            // still only one thread should have woken up
            REQUIRE( signalledCount==1 );
        }


        SECTION("pulseAll")
        {
            std::atomic<int> signalledCount;

            std::list< std::future<void> > futureList;
            for(int i=0; i<10; i++)
            {
                futureList.push_back(
                    Thread::exec( 
                        [pSignal, &signalledCount]()
                        {
                            Thread::sleepMillis(1000);
    
                            if(pSignal->wait(10000))
                                signalledCount++;
                        } ) );
            }

            Thread::sleepMillis(4000);

            pSignal->pulseAll();

            // wait for all threads to finish.
            StopWatch stopWatch;
            for( auto& f: futureList)
                f.get();

            // should not have taken long for the threads to finish
            REQUIRE( stopWatch.getMillis()<1000 );

            // all of them should have woken up.
            REQUIRE( signalledCount==10 );
        }
    }
#endif
}

