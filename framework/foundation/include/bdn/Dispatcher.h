#pragma once

#include <chrono>
#include <map>

namespace bdn
{
    class Dispatcher
    {
      public:
        virtual ~Dispatcher() = default;

      public:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;
        using Duration = Clock::duration;
        using SteppedDuration = std::chrono::duration<long long, std::deci>;

      public:
        enum class Priority
        {
            idle = -100,
            normal = 0,
        };

        static constexpr const int NumberOfPriorities = 2;

      public:
        virtual void enqueue(std::function<void()> func, Priority priority = Priority::normal) = 0;

        virtual void enqueueDelayed(Dispatcher::Duration delay, std::function<void()> func,
                                    Priority priority = Priority::normal) = 0;

        virtual void createTimer(Dispatcher::Duration interval, std::function<bool()> func) = 0;
    };
}
