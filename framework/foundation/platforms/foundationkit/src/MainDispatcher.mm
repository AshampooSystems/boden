
#import <bdn/foundationkit/MainDispatcher.hh>

#include <bdn/entry.h>
#include <bdn/log.h>
#include <iostream>

#import <Foundation/Foundation.h>

namespace bdn::fk
{
    MainDispatcher::MainDispatcher() : bdn::DispatchQueue(true) {}

    MainDispatcher::~MainDispatcher() { dispose(); }

    void MainDispatcher::scheduleCall()
    {
        dispatch_async(dispatch_get_main_queue(), ^{
          process();
        });
    }

    void MainDispatcher::scheduleCallAt(DispatchQueue::TimePoint at)
    {
        auto duration = at - DispatchQueue::Clock::now();

        auto durationInNanoSeconds =
            std::max((long long)0, std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());

        if (durationInNanoSeconds == 0) {
            scheduleCall();
        } else {
            auto when = dispatch_time(DISPATCH_TIME_NOW, durationInNanoSeconds);
            dispatch_after(when, dispatch_get_main_queue(), ^{
              process();
            });
        }
    }

    void MainDispatcher::notifyWorker(DispatchQueue::LockType &lk) { scheduleCall(); }

    void MainDispatcher::newTimed(DispatchQueue::LockType &lk) { scheduleCall(); }

    void MainDispatcher::createTimerInternal(std::chrono::duration<double> interval, std::function<bool()> timer)
    {
        DispatchQueue::LockType lk(queueMutex());

        auto intervalInNanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(interval);
        _timers.emplace_back(std::make_unique<DispatchTimer>(shared_from_this(), timer, intervalInNanoseconds.count()));
    }

    void MainDispatcher::process()
    {
        DispatchQueue::LockType lk(queueMutex());
        auto nextTimed = processQueue(lk);

        if (nextTimed) {
            scheduleCallAt(*nextTimed);
        }
    }

    void MainDispatcher::timerFinished(DispatchTimer *timer)
    {
        auto it = std::find_if(_timers.begin(), _timers.end(), [timer](auto &p) { return p.get() == timer; });
        if (it != _timers.end())
            _timers.erase(it);
    }

    void MainDispatcher::dispose()
    {
        DispatchQueue::LockType lk(queueMutex());
        emptyQueues(lk);
        _timers.clear();
    }

    class DispatchTimer
    {
      public:
        DispatchTimer(std::weak_ptr<MainDispatcher> dispatcher, std::function<bool()> timer,
                      long long intervalInNanoseconds)
            : _dispatcher(dispatcher), _timer(timer)
        {
            _source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
            dispatch_time_t intervalStart = dispatch_walltime(NULL, intervalInNanoseconds);

            dispatch_source_set_timer(_source, intervalStart, (dispatch_time_t)intervalInNanoseconds,
                                      10 * 1000 * 1000); // 10 ms leeway

            dispatch_source_set_event_handler(_source, ^{
              if (!timer()) {
                  cancel();
              }
            });

            dispatch_resume(_source);
        }

        ~DispatchTimer() { cancel(); }

        void cancel()
        {
            if (_source) {
                dispatch_source_cancel(_source);
                _source = nullptr;
            }
            if (auto dispatcher = _dispatcher.lock()) {
                dispatcher->timerFinished(this);
            }
        }

      private:
        dispatch_source_t _source = nullptr;
        std::weak_ptr<MainDispatcher> _dispatcher;
        std::function<bool()> _timer;
    };
}
