#include <bdn/Application.h>
#include <bdn/Timer.h>

namespace bdn
{
    class TimerImpl
    {
      public:
        TimerImpl(Timer *timer) : _timer(timer) {}

        bool notify(size_t id)
        {
            std::unique_lock<std::mutex> lk(_mutex);
            if (_timer && _timer->currentId() == id) {
                _timer->onTriggered().notify();
                return true;
            }
            return false;
        }

        void clear()
        {
            std::unique_lock<std::mutex> lk(_mutex);
            _timer = nullptr;
        }

      private:
        std::mutex _mutex;
        Timer *_timer;
    };

    class TimerCallback
    {
      public:
        TimerCallback(std::weak_ptr<TimerImpl> timer, size_t id) : _timer(std::move(timer)), _id(id) {}

        bool operator()() const
        {
            if (auto timer = _timer.lock()) {
                return timer->notify(_id);
            }
            return false;
        }

      private:
        std::weak_ptr<TimerImpl> _timer;
        size_t _id;
    };

    Timer::Timer(std::shared_ptr<DispatchQueue> dispatchQueue)
        : _dispatchQueue(std::move(dispatchQueue)), _impl(std::make_shared<TimerImpl>(this))
    {
        if (!_dispatchQueue) {
            _dispatchQueue = App()->dispatchQueue();
        }

        onTriggered() += [=]() {
            if (!repeat && _isRunning) {
                _isRunning = false;
                running = false;
            }
        };

        running.onChange() += [=](auto &property) {
            if (property.get()) {
                start();
            } else {
                stop();
            }
        };

        interval.onChange() += [=](auto) {
            if (running.get()) {
                restart();
            }
        };

        repeat.onChange() += [=](auto &property) {
            if (property.get()) {
                if (_isRunning && running) {
                    restart();
                }
            } else {
                stop();
            };
        };
    }

    Timer::~Timer()
    {
        stop();
        _impl->clear();
    }

    Notifier<> &Timer::onTriggered() { return _triggered; }

    void Timer::start()
    {
        if (!_isRunning) {
            if (!repeat) {
                TimerCallback tc(_impl, ++_id);
                _dispatchQueue->dispatchAsyncDelayed(
                    std::chrono::duration_cast<std::chrono::milliseconds>(interval.get()), [tc]() { tc(); });
            } else {
                _dispatchQueue->createTimer(std::chrono::duration_cast<std::chrono::milliseconds>(interval.get()),
                                            TimerCallback{_impl, ++_id});
            }

            _isRunning = true;
            running = true;
        }
    }

    void Timer::stop()
    {
        running = false;
        _id++;
        _isRunning = false;
    }

    void Timer::restart()
    {
        stop();
        start();
    }

    size_t Timer::currentId() { return _id; }
}
