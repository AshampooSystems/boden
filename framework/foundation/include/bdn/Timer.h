#pragma once

#include <bdn/Dispatcher.h>
#include <bdn/Notifier.h>
#include <bdn/property/Property.h>
#include <chrono>
#include <memory>

namespace bdn
{
    class TimerImpl;

    class Timer
    {
      public:
        using Duration = std::chrono::duration<float>;

      public:
        static std::shared_ptr<Timer> create(std::shared_ptr<Dispatcher> dispatcher = nullptr);

      public:
        Timer(std::shared_ptr<Dispatcher> dispatcher = nullptr);
        ~Timer();

      public:
        Property<Duration> interval{Duration(0.0)};
        Property<bool> repeat = false;
        Property<bool> running = false;

      public:
        Notifier<> &onTriggered();

      public:
        void start();
        void stop();
        void restart();

      public:
        size_t currentId();

      private:
        size_t _id = 0;
        std::shared_ptr<Dispatcher> _dispatcher;
        Notifier<> _triggered;
        std::shared_ptr<TimerImpl> _impl;
        bool _isRunning = false;
    };
}
