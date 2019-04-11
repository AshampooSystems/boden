#pragma once

#include <chrono>
#include <future>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <thread>

namespace bdn
{
    class DispatchQueue
    {
      public:
        using Function = std::function<void()>;
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

      protected:
        using MutexType = std::mutex;
        using LockType = std::unique_lock<MutexType>;

      public:
        DispatchQueue(bool slave = false) : _slave(slave)
        {
            if (!_slave) {
                _thread = std::make_unique<std::thread>(std::bind(&DispatchQueue::workerThread, this));
                _threadId = _thread->get_id();
            }
        }
        virtual ~DispatchQueue()
        {
            cancel();
            if (_thread) {
                _thread->join();
                _thread.reset();
            }
        }

      public:
        void dispatchAsync(Function function)
        {
            LockType lk(_queueMutex);
            dispatchAsync(std::move(function), lk);
        }

        void dispatchSync(Function function)
        {
            if (std::this_thread::get_id() == _threadId) {
                function();
                return;
            }

            LockType lk(_queueMutex);
            if (_cancelled) {
                return;
            }

            std::packaged_task<void()> task(function);
            auto future = task.get_future();
            dispatchAsync(std::ref(task), lk);

            lk.unlock();
            while (!_cancelled) {
                if (future.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready)
                    break;
            }
        }

        template <class _Rep, class _Period>
        void dispatchAsyncDelayed(std::chrono::duration<_Rep, _Period> delay, Function function)
        {
            LockType lk(_queueMutex);

            TimePoint executeTimePoint = Clock::now() + std::chrono::duration_cast<Clock::duration>(delay);
            _timedQueue[executeTimePoint].push(std::move(function));
            newTimed(lk);
            notifyWorker(lk);
        }

        template <class _Rep, class _Period>
        void createTimer(std::chrono::duration<_Rep, _Period> interval, std::function<bool()> timer)
        {
            auto intervalInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(interval);
            createTimerInternal(intervalInSeconds, timer);
        }

      public:
        void enter()
        {
            if (_thread) {
                throw std::logic_error("This queue is already served by its own thread!");
            }
            if (_cancelled) {
                throw std::logic_error("This queue is already cancelled!");
            }

            _threadId = std::this_thread::get_id();

            workerThread();

            LockType lk(_queueMutex);
            emptyQueues(lk);
        }
        void cancel()
        {
            LockType lk(_queueMutex);
            _cancelled = true;
            notifyWorker(lk);
        }
        void executeSync()
        {
            if (_thread) {
                throw std::logic_error("This queue is already served by its own thread!");
            }

            LockType lk(_queueMutex);
            executeNext(lk);
        }

      protected:
        virtual void notifyWorker(LockType &lk) { _notification.notify_all(); }
        virtual void newTimed(LockType &lk) { _nTimed++; }
        virtual void createTimerInternal(std::chrono::duration<double> interval, std::function<bool()> timer)
        {
            auto delayInNanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(interval);
            dispatchAsyncDelayed(interval, Timer{this, delayInNanoseconds, timer});
        }

      private:
        void dispatchAsync(Function function, LockType &lk)
        {
            if (_cancelled) {
                return;
            }

            _queue.push(std::move(function));
            notifyWorker(lk);
        }

        void executeNext(LockType &lk)
        {
            if (_queue.empty()) {
                return;
            }

            auto &next = _queue.front();
            lk.unlock();
            (next)();
            lk.lock();
            _queue.pop();
        }

        std::optional<TimePoint> processTimed(LockType &lk)
        {
            while (!_timedQueue.empty() && !_cancelled) {
                auto it = _timedQueue.begin();
                if (it->first <= Clock::now()) {
                    while (!it->second.empty() && !_cancelled) {
                        auto &f = it->second.front();
                        lk.unlock();
                        f();
                        lk.lock();
                        it->second.pop();
                    }
                    _timedQueue.erase(it);
                } else {
                    break;
                }
            }
            if (!_timedQueue.empty()) {
                return _timedQueue.begin()->first;
            }

            return std::nullopt;
        }

      protected:
        std::optional<TimePoint> processQueue(LockType &lk)
        {
            auto nextTimed = processTimed(lk);

            while (!_queue.empty()) {
                executeNext(lk);
                if (nextTimed) {
                    if (Clock::now() >= *nextTimed)
                        break;
                }
            }

            return nextTimed;
        }

        std::mutex &queueMutex() { return _queueMutex; }

        void emptyQueues(LockType &lk)
        {
            while (!_queue.empty()) {
                _queue.pop();
            }
            _timedQueue.clear();
        }

      private:
        void workerThread()
        {
            int oldTimed = -1;
            LockType lk(_queueMutex);
            std::optional<TimePoint> nextTimed;
            while (true) {
                if (_cancelled) {
                    return;
                }

                nextTimed = processQueue(lk);
                oldTimed = _nTimed;

                if (nextTimed) {
                    _notification.wait_until(lk, *nextTimed,
                                             [&]() { return _cancelled || !_queue.empty() || _nTimed != oldTimed; });
                } else {
                    _notification.wait(lk, [&]() { return _cancelled || !_queue.empty() || _nTimed != oldTimed; });
                }
            }
        }

      private:
        class Timer
        {
          public:
            void operator()()
            {
                if (_function()) {
                    _queue->dispatchAsyncDelayed(_interval, std::move(*this));
                }
            }

            DispatchQueue *_queue = nullptr;
            std::chrono::nanoseconds _interval;
            std::function<bool()> _function;
        };

      private:
        std::thread::id _threadId;
        std::unique_ptr<std::thread> _thread;
        const bool _slave;

        std::mutex _queueMutex;
        std::queue<Function> _queue;
        std::map<TimePoint, std::queue<Function>> _timedQueue;
        std::condition_variable _notification;
        int _nTimed = 0;
        bool _cancelled = false;
    };
}
