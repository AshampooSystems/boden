#include <array>
#include <bdn/Application.h>
#include <bdn/DispatchQueue.h>
#include <bdn/log.h>
#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <queue>
#include <random>
#include <thread>

using namespace std::chrono_literals;

namespace bdn
{

    struct DispatchConsumer
    {
        DispatchConsumer() = default;
        DispatchConsumer(const DispatchConsumer &) = delete;

        std::mutex mutex;
        std::condition_variable cv;
        int triggers = 0;

        bool wait()
        {
            std::unique_lock<std::mutex> lk(mutex);
            return cv.wait_for(lk, 1min) == std::cv_status::no_timeout;
        }

        bool waitFor(int numberOfTriggers)
        {
            std::unique_lock<std::mutex> lk(mutex);

            if (numberOfTriggers == triggers) {
                return true;
            }

            return cv.wait_for(lk, 1min, [&] { return triggers == numberOfTriggers; });
        }

        void operator()()
        {
            std::unique_lock<std::mutex> lk(mutex);
            triggers++;
            cv.notify_all();
        }
    };

    TEST(DispatchQueue, Init) { DispatchQueue queue(false); }

    TEST(DispatchQueue, Async)
    {
        DispatchConsumer consumer;
        DispatchQueue queue(false);

        queue.dispatchAsync(std::ref(consumer));

        EXPECT_TRUE(consumer.waitFor(1));
    }

    TEST(DispatchQueue, Sync)
    {
        DispatchConsumer consumer;
        DispatchQueue queue(false);

        queue.dispatchSync(std::ref(consumer));

        EXPECT_EQ(consumer.triggers, 1);
    }

    TEST(DispatchQueue, SyncRecursive)
    {
        DispatchConsumer consumer;
        DispatchQueue queue(false);

        queue.dispatchSync([&]() {
            queue.dispatchSync([&]() { consumer(); });
            consumer();
        });

        EXPECT_TRUE(consumer.waitFor(2));
    }

    TEST(DispatchQueue, AsyncRecursive)
    {
        DispatchConsumer consumer;
        DispatchQueue queue(false);

        queue.dispatchAsync([&]() {
            queue.dispatchAsync([&]() { consumer(); });
            consumer();
        });

        EXPECT_TRUE(consumer.waitFor(2));
    }

    TEST(DispatchQueue, Delayed)
    {
        DispatchConsumer consumer;
        DispatchQueue queue(false);

        auto t = DispatchQueue::Clock::now();

        queue.dispatchAsyncDelayed(100ms, std::ref(consumer));

        EXPECT_TRUE(consumer.waitFor(1));
        EXPECT_GE(DispatchQueue::Clock::now(), t + 100ms);
    }

    TEST(DispatchQueue, Timer)
    {
        DispatchConsumer consumer;
        DispatchQueue queue(false);

        auto t = DispatchQueue::Clock::now();

        queue.createTimer(10ms, [&]() {
            consumer();
            return consumer.triggers != 10;
        });

        EXPECT_TRUE(consumer.waitFor(10));
        EXPECT_GE(DispatchQueue::Clock::now(), t + 100ms);
    }

    TEST(DispatchQueue, Slave)
    {
        DispatchQueue queue(true);

        std::thread t([&]() {
            std::this_thread::sleep_for(100ms);
            queue.cancel();
        });

        queue.enter();
        t.join();
    }

    TEST(DispatchQueue, AbortSyncDispatchWaitOnDestruct)
    {
        DispatchQueue queue(true);

        std::thread t([&]() { queue.dispatchSync([]() {}); });

        queue.cancel();

        t.join();
    }

    bool stressTestTimer(bool *end, std::atomic<int> &numCalls)
    {
        numCalls++;

        if (!end || !*end) {
            return true;
        }
        return false;
    }

    void stressTestThread(std::weak_ptr<DispatchQueue> dispatchQueue, bool &end, std::atomic<int> &numCalls)
    {
        std::this_thread::sleep_for(1ms * rand() % 500);

        std::shared_ptr<int> nCalls = std::make_shared<int>(0);
        logstream(true) << "BombThread started ( id = 0x" << std::hex << std::this_thread::get_id() << " )";
        while (!end) {
            if (auto queue = dispatchQueue.lock())
                // std::this_thread::sleep_for(1ns * (rand() % 10000));
                switch (rand() % 3) {
                case 0:
                    queue->dispatchSync([&numCalls, nCalls]() {
                        numCalls++;
                        (*nCalls)++;
                    });
                    break;
                case 1:
                    queue->dispatchAsync([&numCalls, nCalls]() {
                        numCalls++;
                        (*nCalls)++;
                    });
                    break;
                case 2:
                    queue->dispatchAsyncDelayed(1ms * (rand() % 10), [&numCalls, nCalls]() {
                        numCalls++;
                        (*nCalls)++;
                    });
                    break;
                }
        }

        logstream(true) << "Thread Id = 0x" << std::hex << std::this_thread::get_id() << " Produced: " << std::dec
                        << *nCalls;
    }

    TEST(DispatchQueue, StressTest)
    {
        std::chrono::seconds testDuration = 10s;
        auto cmdLineArgs = bdn::App()->commandLineArguments.get();
        if (std::find(cmdLineArgs.begin(), cmdLineArgs.end(), std::string("--longDispatchQueueStressTest")) !=
            cmdLineArgs.end()) {
            testDuration = 2 * 60s;
        }

        auto startpoint = DispatchQueue::Clock::now();

        const int nThreads = std::max(8, (int)std::thread::hardware_concurrency()) * 10;
        std::vector<std::unique_ptr<std::thread>> threads;
        {
            std::shared_ptr<DispatchQueue> queue = std::make_shared<DispatchQueue>();
            bool end = false;
            std::atomic<int> numCalls(0);
            std::atomic<int> numTimerCalls(0);
            int nThread = 0;

            threads.resize(nThreads);

            for (auto &t : threads) {
                t = std::make_unique<std::thread>(
                    [&queue, &end, &numCalls]() { stressTestThread(queue, end, numCalls); });
                nThread++;
            }

            for (int i = 1; i < 100; i += 10) {
                queue->createTimer(1ms * i, [&]() { return stressTestTimer(&end, numTimerCalls); });
            }

            while (DispatchQueue::Clock::now() - startpoint < testDuration) {
                std::this_thread::sleep_for(500ms);
                logstream() << "NumCalls: " << numCalls.load() << ", numTimerCalls: " << numTimerCalls.load();
            }

            end = true;
            queue->cancel();
        }

        logstream() << "Waiting for threads to exit";

        for (auto &t : threads) {
            t->join();
        }
    }
}
