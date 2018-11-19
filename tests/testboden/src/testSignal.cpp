#include <bdn/init.h>

#include <bdn/Signal.h>

#include <bdn/test.h>
#include <bdn/StopWatch.h>

using namespace bdn;

void verifySignalState(Signal *signal, bool expectedSet)
{
    REQUIRE(signal->isSet() == expectedSet);

    StopWatch stopWatch;
    REQUIRE(signal->wait(0) == expectedSet);
    // should not have waited
    REQUIRE(stopWatch.getMillis() < 500);

    stopWatch.start();
    REQUIRE(signal->wait(1000) == expectedSet);

    if (expectedSet) {
        // should not have waited
        REQUIRE(stopWatch.getMillis() < 500);
    } else {
        // should have waited about 1000ms
        REQUIRE(stopWatch.getMillis() > 990);
    }

    // the wait functions must not have modified the state.
    REQUIRE(signal->isSet() == expectedSet);
}

TEST_CASE("Signal")
{
    P<Signal> signal = newObj<Signal>();

    SECTION("initialState")
    verifySignalState(signal, false);

    SECTION("sameThread")
    {
        SECTION("setWaitClear")
        {
            signal->set();
            verifySignalState(signal, true);

            signal->clear();
            verifySignalState(signal, false);
        }

        SECTION("pulseOne")
        {
            signal->pulseOne();
            // should have no effect if no one is currently waiting
            verifySignalState(signal, false);
        }

        SECTION("pulseAll")
        {
            signal->pulseAll();
            // should have no effect if no one is currently waiting
            verifySignalState(signal, false);
        }

        SECTION("pulseOneWhileSet")
        {
            signal->set();
            signal->pulseOne();
            // should have reset the signal
            verifySignalState(signal, false);
        }

        SECTION("pulseAllWhileSet")
        {
            signal->set();
            signal->pulseAll();
            // should have reset the signal
            verifySignalState(signal, false);
        }
    }

#if BDN_HAVE_THREADS
    SECTION("otherThread")
    {
        SECTION("setWait")
        {
            Thread::exec([signal]() {
                Thread::sleepMillis(3000);

                signal->set();
            });

            StopWatch stopWatch;

            // should not yet be set
            verifySignalState(signal, false);

            REQUIRE(signal->wait(5000));
            // should have waited about 3000ms in total
            REQUIRE(stopWatch.getMillis() > 2500);
            REQUIRE(stopWatch.getMillis() < 4500);
        }

        SECTION("pulseOne")
        {
            std::atomic<int> signalledCount(0);

            std::list<std::future<void>> futureList;
            for (int i = 0; i < 10; i++) {
                futureList.push_back(Thread::exec([signal, &signalledCount]() {
                    Thread::sleepMillis(1000);

                    if (signal->wait(10000))
                        signalledCount++;
                }));
            }

            Thread::sleepMillis(4000);

            signal->pulseOne();

            Thread::sleepMillis(1000);

            // one thread should have woken up
            REQUIRE(signalledCount == 1);

            // wait for all threads to finish.
            for (auto &f : futureList)
                f.get();

            // still only one thread should have woken up
            REQUIRE(signalledCount == 1);
        }

        SECTION("pulseAll")
        {
            std::atomic<int> signalledCount(0);

            std::list<std::future<void>> futureList;
            for (int i = 0; i < 10; i++) {
                futureList.push_back(Thread::exec([signal, &signalledCount]() {
                    Thread::sleepMillis(1000);

                    if (signal->wait(10000))
                        signalledCount++;
                }));
            }

            Thread::sleepMillis(4000);

            signal->pulseAll();

            // wait for all threads to finish.
            StopWatch stopWatch;
            for (auto &f : futureList)
                f.get();

            // should not have taken long for the threads to finish
            REQUIRE(stopWatch.getMillis() < 1000);

            // all of them should have woken up.
            REQUIRE(signalledCount == 10);
        }
    }
#endif
}
