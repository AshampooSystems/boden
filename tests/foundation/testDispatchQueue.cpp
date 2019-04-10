#include <bdn/DispatchQueue.h>
#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <queue>
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
}
