#pragma once

#include <bdn/DispatchQueue.h>

#include <list>
#include <mutex>

#import <CoreFoundation/CoreFoundation.h>

namespace bdn::fk
{
    class DispatchTimer;

    class MainDispatcher : public DispatchQueue, public std::enable_shared_from_this<MainDispatcher>
    {
      public:
        MainDispatcher();
        ~MainDispatcher() override;

        void dispose();

        void process();

        void timerFinished(DispatchTimer *timer);

      protected:
        void notifyWorker(LockType &lk) override;
        void newTimed(LockType &lk) override;
        void createTimerInternal(std::chrono::duration<double> interval, std::function<bool()> timer) override;

      private:
        void scheduleCall();
        void scheduleCallAt(DispatchQueue::TimePoint at);

      private:
        std::list<std::unique_ptr<DispatchTimer>> _timers;
    };
}
