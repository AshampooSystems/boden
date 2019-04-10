#pragma once

#include <bdn/Base.h>
#include <bdn/DispatchQueue.h>

#include <list>
#include <mutex>

#import <CoreFoundation/CoreFoundation.h>

namespace bdn::fk
{
    class DispatchTimer;

    class MainDispatcher : public Base, virtual public DispatchQueue
    {
        std::shared_ptr<MainDispatcher> shared_from_this()
        {
            return std::static_pointer_cast<MainDispatcher>(Base::shared_from_this());
        }

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
