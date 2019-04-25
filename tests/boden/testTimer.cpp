#include <bdn/Timer.h>
#include <chrono>
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>

using namespace std::chrono_literals;

namespace bdn
{
    struct TriggerConsumer
    {
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
            return cv.wait_for(lk, 1min, [&] { return triggers == numberOfTriggers; });
        }

        void trigger()
        {
            triggers++;
            cv.notify_all();
        }
    };

    TEST(Timer, SingleShot)
    {
        TriggerConsumer tc;

        Timer t;
        t.interval = 10ms;

        t.onTriggered() += [&tc]() { tc.trigger(); };
        t.start();

        bool result = tc.wait();
        EXPECT_EQ(result, true);
    }

    TEST(Timer, Repeating)
    {
        TriggerConsumer tc;

        Timer t;
        t.interval = 10ms;
        t.repeat = true;

        t.onTriggered() += [&tc]() { tc.trigger(); };
        t.start();

        bool result = tc.waitFor(10);
        EXPECT_EQ(result, true);
    }
}
