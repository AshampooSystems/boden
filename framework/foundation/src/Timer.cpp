#include <bdn/Application.h>
#include <bdn/Timer.h>

namespace bdn
{
    class TimerImpl
    {
      public:
        TimerImpl(Timer *timer) : _timer(timer) {}

        size_t currentId()
        {
            if (_timer) {
                return _timer->currentId();
            }
            return 0;
        }

        Notifier<> &onTriggered() { return _timer->onTriggered(); }

        Timer *_timer;
    };

    class TimerCallback
    {
      public:
        TimerCallback(std::weak_ptr<TimerImpl> timer, size_t id) : _timer(timer), _id(id) {}

        bool operator()() const
        {
            if (auto timer = _timer.lock()) {
                if (_id == timer->currentId()) {
                    timer->onTriggered().notify();
                    return true;
                }
            }
            return false;
        }

      private:
        std::weak_ptr<TimerImpl> _timer;
        size_t _id;
    };

    Timer::Timer(std::shared_ptr<Dispatcher> dispatcher)
        : _dispatcher(std::move(dispatcher)), _impl(std::make_shared<TimerImpl>(this))
    {
        if (!_dispatcher) {
            _dispatcher = App()->dispatcher();
        }

        onTriggered() += [=]() {
            if (!repeat && _isRunning) {
                _isRunning = false;
                running = false;
            }
        };

        running.onChange() += [=](auto va) {
            if (va->get()) {
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

        repeat.onChange() += [=](auto va) {
            if (va->get()) {
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
        _impl->_timer = nullptr;
    }

    Notifier<> &Timer::onTriggered() { return _triggered; }

    void Timer::start()
    {
        if (!_isRunning) {
            if (!repeat) {
                TimerCallback tc(_impl, ++_id);
                _dispatcher->enqueueDelayed(std::chrono::duration_cast<Dispatcher::Duration>(interval.get()),
                                            [tc]() { tc(); });
            } else {
                _dispatcher->createTimer(std::chrono::duration_cast<Dispatcher::Duration>(interval.get()),
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
