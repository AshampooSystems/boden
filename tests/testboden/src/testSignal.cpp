

#include <bdn/Signal.h>

#include <bdn/StopWatch.h>
#include <bdn/config.h>
#include <bdn/test.h>

#include <future>
#include <thread>

using namespace bdn;
using namespace std::chrono_literals;

void verifySignalState(std::shared_ptr<Signal> signal, bool expectedSet)
{
    REQUIRE(signal->isSet() == expectedSet);

    StopWatch stopWatch;
    REQUIRE(signal->wait(0s) == expectedSet);
    // should not have waited
    REQUIRE(stopWatch.elapsed() < 500ms);

    stopWatch.start();
    REQUIRE(signal->wait(1s) == expectedSet);

    if (expectedSet) {
        // should not have waited
        REQUIRE(stopWatch.elapsed() < 500ms);
    } else {
        // should have waited about 1000ms
        REQUIRE(stopWatch.elapsed() > 990ms);
    }

    // the wait functions must not have modified the state.
    REQUIRE(signal->isSet() == expectedSet);
}

TEST_CASE("Signal")
{
    std::shared_ptr<Signal> signal = std::make_shared<Signal>();

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
            auto future = std::async(std::launch::async, [signal]() {
                std::this_thread::sleep_for(3s);

                signal->set();
            });

            StopWatch stopWatch;

            // should not yet be set
            verifySignalState(signal, false);

            REQUIRE(signal->wait(5s));
            // should have waited about 3000ms in total
            REQUIRE(stopWatch.elapsed() > 2500ms);
            REQUIRE(stopWatch.elapsed() < 4500ms);
        }

        SECTION("pulseOne")
        {
            std::atomic<int> signalledCount(0);

            std::list<std::future<void>> futureList;
            for (int i = 0; i < 10; i++) {
                futureList.push_back(std::async(std::launch::async, [signal, &signalledCount]() {
                    std::this_thread::sleep_for(1s);

                    if (signal->wait(10s))
                        signalledCount++;
                }));
            }

            std::this_thread::sleep_for(4s);

            signal->pulseOne();

            std::this_thread::sleep_for(1s);

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
                futureList.push_back(std::async(std::launch::async, [signal, &signalledCount]() {
                    std::this_thread::sleep_for(1s);

                    if (signal->wait(10s))
                        signalledCount++;
                }));
            }

            std::this_thread::sleep_for(4s);

            signal->pulseAll();

            // wait for all threads to finish.
            StopWatch stopWatch;
            for (auto &f : futureList)
                f.get();

            // should not have taken long for the threads to finish
            REQUIRE(stopWatch.elapsed() < 1s);

            // all of them should have woken up.
            REQUIRE(signalledCount == 10);
        }
    }
#endif
}
