#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ThreadPool.h>

using namespace bdn;


class ThreadPoolTestRunnable : public Base, BDN_IMPLEMENTS IThreadRunnable
{
public:

    Signal stopSignal;

    void signalStop() override
    {
        stopSignal.set();
    }


    Signal startedSignal;
    Signal proceedSignal;

    void run() override
    {
        startedSignal.set();
        proceedSignal.wait();

        stopSignal.wait();
    }
};

TEST_CASE( "ThreadPool" )
{
    SECTION("construct")
    {
        SECTION("invalid params")
        {
            SECTION("min<0")
                REQUIRE_THROWS_AS( ThreadPool(-1,1), InvalidArgumentError );

            SECTION("max<0")
                REQUIRE_THROWS_AS( ThreadPool(-1,-1), InvalidArgumentError );

            SECTION("max=0")
                REQUIRE_THROWS_AS( ThreadPool(0,0), InvalidArgumentError );        

            SECTION("max<min")
                REQUIRE_THROWS_AS( ThreadPool(2,1), InvalidArgumentError );
        }
        
        SECTION("min=0, max=1")
        {
            // this should be ok
            ThreadPool pool(0, 1);
        }
    }


    SECTION("single thread")
    {
        P<ThreadPool> pPool = newObj<ThreadPool>(1,1);

        // the min number of threads are not started right away.
        // They are added when needed (but are subsequently not destroyed).
        REQUIRE( pPool->getIdleThreadCount()==0 );
        REQUIRE( pPool->getBusyThreadCount()==0 );

        SECTION("no backlog")
        {
            P<ThreadPoolTestRunnable> pJob = newObj<ThreadPoolTestRunnable>();

            pPool->addJob(pJob);

            // still none idle
            REQUIRE( pPool->getIdleThreadCount()==0 );
            // one thread should be busy
            REQUIRE( pPool->getBusyThreadCount()==1 );

            // wait until the job thread has actullally been started.
            REQUIRE( pJob->startedSignal.wait(5000) );

            pJob->proceedSignal.set();
            pJob->stopSignal.set();

            CONTINUE_SECTION_AFTER_SECONDS(3, pPool)
            {
                // thread should have finished and not be busy anymore
                REQUIRE( pPool->getBusyThreadCount()==0 );

                // the thread should not have been destroyed and should now be idle
                REQUIRE( pPool->getIdleThreadCount()==1 );

                // start another job

                P<ThreadPoolTestRunnable> pJob2 = newObj<ThreadPoolTestRunnable>();
                pPool->addJob(pJob2);
            
                // now the idle thread should be busy.
                REQUIRE( pPool->getBusyThreadCount()==1 );
                REQUIRE( pPool->getIdleThreadCount()==0 );

                // wait until the job thread has actullally been started.
                REQUIRE( pJob2->startedSignal.wait(5000) );

                pJob2->proceedSignal.set();
                pJob2->stopSignal.set();
            
                CONTINUE_SECTION_AFTER_SECONDS(3, pPool)
                {
                    // we should now again have 0 busy, 1 idle
                    REQUIRE( pPool->getBusyThreadCount()==0 );
                    REQUIRE( pPool->getIdleThreadCount()==1 );
                };
            };
        }

        SECTION("with backlog")
        {
            P<ThreadPoolTestRunnable> pA = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> pB = newObj<ThreadPoolTestRunnable>();

            pPool->addJob(pA);
            pPool->addJob(pB);

            REQUIRE( pPool->getIdleThreadCount()==0 );
            // only one should be busy, the other one should be queued
            REQUIRE( pPool->getBusyThreadCount()==1 );

            // wait until A has actually started
            REQUIRE( pA->startedSignal.wait(5000) );

            // B should not have been started
            REQUIRE( !pB->startedSignal.isSet() );
            
            pA->proceedSignal.set();
            pA->stopSignal.set();

            // A should now finish and B should start
            REQUIRE( pB->startedSignal.wait(5000) );

            // still: 1 busy, 0 idle
            REQUIRE( pPool->getIdleThreadCount()==0 );
            REQUIRE( pPool->getBusyThreadCount()==1 );

            // let B finish
            pB->proceedSignal.set();
            pB->stopSignal.set();

            CONTINUE_SECTION_AFTER_SECONDS(3, pPool)
            {
                // now the thread should be idle again
                REQUIRE( pPool->getIdleThreadCount()==1 );
                REQUIRE( pPool->getBusyThreadCount()==0);
            };
        }
    }

    SECTION("multiple threads")
    {
        P<ThreadPool> pPool = newObj<ThreadPool>(2, 3);

        // the min number of threads are not started right away.
        // They are added when needed (but are subsequently not destroyed).
        REQUIRE( pPool->getIdleThreadCount()==0 );
        REQUIRE( pPool->getBusyThreadCount()==0 );

        SECTION("no backlog")
        {
            P<ThreadPoolTestRunnable> pA = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> pB = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> pC = newObj<ThreadPoolTestRunnable>();

            pPool->addJob(pA);
            pPool->addJob(pB);
            pPool->addJob(pC);

            // still none idle
            REQUIRE( pPool->getIdleThreadCount()==0 );
            // and three busy
            REQUIRE( pPool->getBusyThreadCount()==3 );

            // wait until all have been started
            REQUIRE( pA->startedSignal.wait(5000) );
            REQUIRE( pB->startedSignal.wait(5000) );
            REQUIRE( pC->startedSignal.wait(5000) );

            // now let all of them finish
            pA->proceedSignal.set();
            pA->stopSignal.set();
            pB->proceedSignal.set();
            pB->stopSignal.set();
            pC->proceedSignal.set();
            pC->stopSignal.set();

            CONTINUE_SECTION_AFTER_SECONDS(3, pPool)
            {
                // threads should have finished and not be busy anymore
                REQUIRE( pPool->getBusyThreadCount()==0 );

                // two threads should now be idle, one should have been destroyed.
                REQUIRE( pPool->getIdleThreadCount()==2 );
            };
        }

        SECTION("with backlog")
        {
            P<ThreadPoolTestRunnable> pA = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> pB = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> pC = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> pD = newObj<ThreadPoolTestRunnable>();

            pPool->addJob(pA);
            pPool->addJob(pB);
            pPool->addJob(pC);
            pPool->addJob(pD);

            // still none idle
            REQUIRE( pPool->getIdleThreadCount()==0 );
            // and three busy. One should be queued
            REQUIRE( pPool->getBusyThreadCount()==3 );

            // wait until the first three have been started
            REQUIRE( pA->startedSignal.wait(5000) );
            REQUIRE( pB->startedSignal.wait(5000) );
            REQUIRE( pC->startedSignal.wait(5000) );

            // D should not have been started yet
            REQUIRE( !pD->startedSignal.isSet() );

            // now let one finish. We use B so that we have some out-of-order finishing
            // in this test.
            pB->proceedSignal.set();
            pB->stopSignal.set();

            // now D should start
            REQUIRE( pD->startedSignal.wait(5000) );

            // still: 0 idle, 3 busy
            REQUIRE( pPool->getIdleThreadCount()==0 );
            REQUIRE( pPool->getBusyThreadCount()==3 );

            // let all finish
            pA->proceedSignal.set();
            pA->stopSignal.set();
            pC->proceedSignal.set();
            pC->stopSignal.set();
            pD->proceedSignal.set();
            pD->stopSignal.set();

            CONTINUE_SECTION_AFTER_SECONDS(3, pPool, pA, pB, pC, pD)
            {
                // now we should have 2 idle again. One thread should have been destroyed.
                REQUIRE( pPool->getIdleThreadCount()==2 );
                REQUIRE( pPool->getBusyThreadCount()==0 );                
            };
        }
    }

    SECTION("pool destroyed")
    {
        P<ThreadPool> pPool = newObj<ThreadPool>(1, 1);
        
        SECTION("no backlog")
        {
            P<ThreadPoolTestRunnable> pA = newObj<ThreadPoolTestRunnable>();

            pPool->addJob(pA);

            pA->startedSignal.wait(5000);

            // let A proceed to the stop signal wait
            pA->proceedSignal.set();

            // sanity check: stop signal should NOT be set right now
            REQUIRE( !pA->stopSignal.isSet() );

            // now destroy the pool
            pPool = nullptr;

            // this should have caused the stop signal of A to be set
            REQUIRE( pA->stopSignal.isSet() );
        }

        SECTION("with backlog")
        {
            P<ThreadPoolTestRunnable> pA = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> pB = newObj<ThreadPoolTestRunnable>();

            pPool->addJob(pA);
            pPool->addJob(pB);

            // pool should hold a reference to the jobs
            REQUIRE( pA->getRefCount()==2 );
            REQUIRE( pB->getRefCount()==2 );

            pA->startedSignal.wait(5000);

            // B should not have been started
            REQUIRE( !pB->startedSignal.wait(1000) );

            REQUIRE( pPool->getIdleThreadCount()==0 );
            REQUIRE( pPool->getBusyThreadCount()==1 );

            // let A proceed to the stop signal wait
            pA->proceedSignal.set();
            
            // now destroy the pool
            pPool = nullptr;

            // this should have caused the stop signal of A to be set
            REQUIRE( pA->stopSignal.isSet() );

            // B should NOT start. In fact, it should never start.
            REQUIRE( !pB->startedSignal.wait(1000) );

            // A and B should have been released by the pool
            REQUIRE( pA->getRefCount()==1 );
            REQUIRE( pB->getRefCount()==1 );
        }
    }
}
    