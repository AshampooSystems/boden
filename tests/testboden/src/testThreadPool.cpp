#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ThreadPool.h>

#if BDN_HAVE_THREADS

using namespace bdn;

class ThreadPoolTestRunnable : public Base, BDN_IMPLEMENTS IThreadRunnable
{
  public:
    Signal stopSignal;

    void signalStop() override { stopSignal.set(); }

    Signal startedSignal;
    Signal proceedSignal;

    void run() override
    {
        startedSignal.set();
        proceedSignal.wait();

        stopSignal.wait();
    }
};

TEST_CASE("ThreadPool")
{
    SECTION("construct")
    {
        SECTION("invalid params")
        {
            SECTION("min<0")
            REQUIRE_THROWS_AS(ThreadPool(-1, 1), InvalidArgumentError);

            SECTION("max<0")
            REQUIRE_THROWS_AS(ThreadPool(-1, -1), InvalidArgumentError);

            SECTION("max=0")
            REQUIRE_THROWS_AS(ThreadPool(0, 0), InvalidArgumentError);

            SECTION("max<min")
            REQUIRE_THROWS_AS(ThreadPool(2, 1), InvalidArgumentError);
        }

        SECTION("min=0, max=1")
        {
            // this should be ok
            ThreadPool pool(0, 1);
        }
    }

    SECTION("single thread")
    {
        P<ThreadPool> pool = newObj<ThreadPool>(1, 1);

        // the min number of threads are not started right away.
        // They are added when needed (but are subsequently not destroyed).
        REQUIRE(pool->getIdleThreadCount() == 0);
        REQUIRE(pool->getBusyThreadCount() == 0);

        SECTION("no backlog")
        {
            P<ThreadPoolTestRunnable> job = newObj<ThreadPoolTestRunnable>();

            pool->addJob(job);

            // still none idle
            REQUIRE(pool->getIdleThreadCount() == 0);
            // one thread should be busy
            REQUIRE(pool->getBusyThreadCount() == 1);

            // wait until the job thread has actullally been started.
            REQUIRE(job->startedSignal.wait(5000));

            job->proceedSignal.set();
            job->stopSignal.set();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, pool)
            {
                // thread should have finished and not be busy anymore
                REQUIRE(pool->getBusyThreadCount() == 0);

                // the thread should not have been destroyed and should now be
                // idle
                REQUIRE(pool->getIdleThreadCount() == 1);

                // start another job

                P<ThreadPoolTestRunnable> job2 = newObj<ThreadPoolTestRunnable>();
                pool->addJob(job2);

                // now the idle thread should be busy.
                REQUIRE(pool->getBusyThreadCount() == 1);
                REQUIRE(pool->getIdleThreadCount() == 0);

                // wait until the job thread has actullally been started.
                REQUIRE(job2->startedSignal.wait(5000));

                job2->proceedSignal.set();
                job2->stopSignal.set();

                CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, pool)
                {
                    // we should now again have 0 busy, 1 idle
                    REQUIRE(pool->getBusyThreadCount() == 0);
                    REQUIRE(pool->getIdleThreadCount() == 1);
                };
            };
        }

        SECTION("with backlog")
        {
            P<ThreadPoolTestRunnable> a = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> b = newObj<ThreadPoolTestRunnable>();

            pool->addJob(a);
            pool->addJob(b);

            REQUIRE(pool->getIdleThreadCount() == 0);
            // only one should be busy, the other one should be queued
            REQUIRE(pool->getBusyThreadCount() == 1);

            // wait until A has actually started
            REQUIRE(a->startedSignal.wait(5000));

            // B should not have been started
            REQUIRE(!b->startedSignal.isSet());

            a->proceedSignal.set();
            a->stopSignal.set();

            // A should now finish and B should start
            REQUIRE(b->startedSignal.wait(5000));

            // still: 1 busy, 0 idle
            REQUIRE(pool->getIdleThreadCount() == 0);
            REQUIRE(pool->getBusyThreadCount() == 1);

            // let B finish
            b->proceedSignal.set();
            b->stopSignal.set();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, pool)
            {
                // now the thread should be idle again
                REQUIRE(pool->getIdleThreadCount() == 1);
                REQUIRE(pool->getBusyThreadCount() == 0);
            };
        }
    }

    SECTION("multiple threads")
    {
        P<ThreadPool> pool = newObj<ThreadPool>(2, 3);

        // the min number of threads are not started right away.
        // They are added when needed (but are subsequently not destroyed).
        REQUIRE(pool->getIdleThreadCount() == 0);
        REQUIRE(pool->getBusyThreadCount() == 0);

        SECTION("no backlog")
        {
            P<ThreadPoolTestRunnable> a = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> b = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> c = newObj<ThreadPoolTestRunnable>();

            pool->addJob(a);
            pool->addJob(b);
            pool->addJob(c);

            // still none idle
            REQUIRE(pool->getIdleThreadCount() == 0);
            // and three busy
            REQUIRE(pool->getBusyThreadCount() == 3);

            // wait until all have been started
            REQUIRE(a->startedSignal.wait(5000));
            REQUIRE(b->startedSignal.wait(5000));
            REQUIRE(c->startedSignal.wait(5000));

            // now let all of them finish
            a->proceedSignal.set();
            a->stopSignal.set();
            b->proceedSignal.set();
            b->stopSignal.set();
            c->proceedSignal.set();
            c->stopSignal.set();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, pool)
            {
                // threads should have finished and not be busy anymore
                REQUIRE(pool->getBusyThreadCount() == 0);

                // two threads should now be idle, one should have been
                // destroyed.
                REQUIRE(pool->getIdleThreadCount() == 2);
            };
        }

        SECTION("with backlog")
        {
            P<ThreadPoolTestRunnable> a = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> b = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> c = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> d = newObj<ThreadPoolTestRunnable>();

            pool->addJob(a);
            pool->addJob(b);
            pool->addJob(c);
            pool->addJob(d);

            // still none idle
            REQUIRE(pool->getIdleThreadCount() == 0);
            // and three busy. One should be queued
            REQUIRE(pool->getBusyThreadCount() == 3);

            // wait until the first three have been started
            REQUIRE(a->startedSignal.wait(5000));
            REQUIRE(b->startedSignal.wait(5000));
            REQUIRE(c->startedSignal.wait(5000));

            // D should not have been started yet
            REQUIRE(!d->startedSignal.isSet());

            // now let one finish. We use B so that we have some out-of-order
            // finishing in this test.
            b->proceedSignal.set();
            b->stopSignal.set();

            // now D should start
            REQUIRE(d->startedSignal.wait(5000));

            // still: 0 idle, 3 busy
            REQUIRE(pool->getIdleThreadCount() == 0);
            REQUIRE(pool->getBusyThreadCount() == 3);

            // let all finish
            a->proceedSignal.set();
            a->stopSignal.set();
            c->proceedSignal.set();
            c->stopSignal.set();
            d->proceedSignal.set();
            d->stopSignal.set();

            CONTINUE_SECTION_AFTER_RUN_SECONDS(0.5, pool, a, b, c, d)
            {
                // now we should have 2 idle again. One thread should have been
                // destroyed.
                REQUIRE(pool->getIdleThreadCount() == 2);
                REQUIRE(pool->getBusyThreadCount() == 0);
            };
        }
    }

    SECTION("pool destroyed")
    {
        P<ThreadPool> pool = newObj<ThreadPool>(1, 1);

        SECTION("no backlog")
        {
            P<ThreadPoolTestRunnable> a = newObj<ThreadPoolTestRunnable>();

            pool->addJob(a);

            a->startedSignal.wait(5000);

            // let A proceed to the stop signal wait
            a->proceedSignal.set();

            // sanity check: stop signal should NOT be set right now
            REQUIRE(!a->stopSignal.isSet());

            // now destroy the pool
            pool = nullptr;

            // this should have caused the stop signal of A to be set
            REQUIRE(a->stopSignal.isSet());
        }

        SECTION("with backlog")
        {
            P<ThreadPoolTestRunnable> a = newObj<ThreadPoolTestRunnable>();
            P<ThreadPoolTestRunnable> b = newObj<ThreadPoolTestRunnable>();

            pool->addJob(a);
            pool->addJob(b);

            // pool should hold a reference to the jobs
            REQUIRE(a->getRefCount() == 2);
            REQUIRE(b->getRefCount() == 2);

            a->startedSignal.wait(5000);

            // B should not have been started
            REQUIRE(!b->startedSignal.wait(1000));

            REQUIRE(pool->getIdleThreadCount() == 0);
            REQUIRE(pool->getBusyThreadCount() == 1);

            // let A proceed to the stop signal wait
            a->proceedSignal.set();

            // now destroy the pool
            pool = nullptr;

            // this should have caused the stop signal of A to be set
            REQUIRE(a->stopSignal.isSet());

            // B should NOT start. In fact, it should never start.
            REQUIRE(!b->startedSignal.wait(1000));

            // A and B should have been released by the pool
            REQUIRE(a->getRefCount() == 1);
            REQUIRE(b->getRefCount() == 1);
        }
    }
}

#endif