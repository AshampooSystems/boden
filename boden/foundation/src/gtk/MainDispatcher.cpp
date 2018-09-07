#include <bdn/init.h>
#include <bdn/gtk/MainDispatcher.h>

#include <bdn/log.h>

#include <bdn/entry.h>

namespace bdn
{
    namespace gtk
    {

        MainDispatcher::MainDispatcher() {}

        void MainDispatcher::dispose()
        {
            Mutex::Lock lock(_queueMutex);

            while (!_normalQueue.empty()) {
                std::function<void()> func = _normalQueue.front();
                _normalQueue.pop_front();

                BDN_LOG_AND_IGNORE_EXCEPTION(
                    { func = std::function<void()>(); },
                    "Ignoring exception from destructor of queued normal "
                    "priority MainDispatcher item that is being disposed.");
            }

            while (!_idleQueue.empty()) {
                std::function<void()> func = _idleQueue.front();
                _idleQueue.pop_front();

                BDN_LOG_AND_IGNORE_EXCEPTION(
                    { func = std::function<void()>(); },
                    "Ignoring exception from destructor of queued idle "
                    "priority MainDispatcher item that is being disposed.");
            }

            // we cannot delete the timed items from out list.
            // so we have to dispose them by invalidating the functions
            for (TimedItem &item : _timedItemList) {
                std::function<void()> func;

                BDN_LOG_AND_IGNORE_EXCEPTION(
                    {
                        func = item.func;
                        item.func = std::function<void()>();
                    },
                    "Ignoring exception from destructor of queued "
                    "MainDispatcher timed item that is being disposed.");
            }

            // same with the timers

            for (Timer &timer : _timerList) {
                std::function<bool()> func;

                BDN_LOG_AND_IGNORE_EXCEPTION(
                    {
                        func = timer.func;
                        timer.func = std::function<bool()>();
                        timer.disposed = true;
                    },
                    "Ignoring exception from destructor of MainDispatcher "
                    "timer that is being disposed.");
            }
        }

        gboolean MainDispatcher::processNormalItemCallback(gpointer data)
        {
            P<MainDispatcher> pThis;
            pThis.attachPtr(static_cast<MainDispatcher *>(data));

            pThis->processItem(pThis->_normalQueue);

            return G_SOURCE_REMOVE;
        }

        gboolean MainDispatcher::processIdleItemCallback(gpointer data)
        {
            P<MainDispatcher> pThis;
            pThis.attachPtr(static_cast<MainDispatcher *>(data));

            pThis->processItem(pThis->_idleQueue);

            return G_SOURCE_REMOVE;
        }

        void
        MainDispatcher::processItem(std::list<std::function<void()>> &queue)
        {
            try {
                std::function<void()> func;

                {
                    Mutex::Lock lock(_queueMutex);

                    if (queue.empty())
                        return;

                    func = queue.front();
                    queue.pop_front();
                }

                try {
                    func();
                }
                catch (DanglingFunctionError &) {
                    // ignore. this means that func is a weak method and
                    // the corresponding object has been deleted.
                }
            }
            catch (...) {
                if (!bdn::unhandledException(true))
                    std::terminate();

                // we should ignore the exception. Just continue.
            }
        }

        void MainDispatcher::enqueue(std::function<void()> func,
                                     Priority priority)
        {
            int gtkPrio;

            if (priority == Priority::normal) {
                {
                    Mutex::Lock lock(_queueMutex);
                    _normalQueue.push_back(func);
                }

                // keep ourselves alive during this
                addRef();

                // note that add_idle_full is correct, even if the priority is
                // not idle. That is just a case of a slightly bad function name
                // on the GTK side.
                gdk_threads_add_idle_full(
                    G_PRIORITY_DEFAULT,
                    &MainDispatcher::processNormalItemCallback,
                    static_cast<MainDispatcher *>(this), NULL);
            } else if (priority == Priority::idle) {
                {
                    Mutex::Lock lock(_queueMutex);
                    _idleQueue.push_back(func);
                }

                // keep ourselves alive during this
                addRef();

                gdk_threads_add_idle_full(
                    G_PRIORITY_DEFAULT_IDLE,
                    &MainDispatcher::processIdleItemCallback,
                    static_cast<MainDispatcher *>(this), NULL);
            } else
                throw InvalidArgumentError("Invalid priority value passed to "
                                           "MainDispatcher::enqueue: " +
                                           std::to_string((int)priority));
        }

        gboolean MainDispatcher::processTimedItemCallback(gpointer data)
        {
            TimedItem *pItem = static_cast<TimedItem *>(data);

            pItem->pDispatcher->processTimedItem(pItem);

            return G_SOURCE_REMOVE;
        }

        void MainDispatcher::processTimedItem(TimedItem *pItem)
        {
            try {
                std::function<void()> func;

                {
                    Mutex::Lock lock(_queueMutex);

                    func = pItem->func;

                    _timedItemList.erase(pItem->it);
                }

                try {
                    func();
                }
                catch (DanglingFunctionError &) {
                    // ignore. this means that func is a weak method and
                    // the corresponding object has been deleted.
                }
            }
            catch (...) {
                if (!bdn::unhandledException(true))
                    std::terminate();

                // we should ignore the exception. Just continue.
            }
        }

        void MainDispatcher::enqueueInSeconds(double seconds,
                                              std::function<void()> func,
                                              Priority priority)
        {
            int64_t millis = (int64_t)(seconds * 1000);

            if (millis < 1)
                enqueue(func, priority);
            else {
                int gtkPrio;

                if (priority == Priority::normal)
                    gtkPrio = G_PRIORITY_DEFAULT;
                else if (priority == Priority::idle)
                    gtkPrio = G_PRIORITY_DEFAULT_IDLE;
                else
                    throw InvalidArgumentError("Invalid priority value passed "
                                               "to MainDispatcher::enqueue: " +
                                               std::to_string((int)priority));

                TimedItem *pItem;

                {
                    Mutex::Lock lock(_queueMutex);
                    _timedItemList.push_back(TimedItem(func, this));
                    std::list<TimedItem>::iterator it = _timedItemList.end();
                    --it;

                    pItem = &*it;
                    pItem->it = it;
                }

                // keep ourselves alive until the time comes
                addRef();

                gdk_threads_add_timeout_full(
                    gtkPrio, millis, processTimedItemCallback, pItem, NULL);
            }
        }

        gboolean MainDispatcher::processTimerCallback(gpointer data)
        {
            Timer *pTimer = static_cast<Timer *>(data);

            if (pTimer->pDispatcher->processTimer(pTimer))
                return G_SOURCE_CONTINUE;
            else
                return G_SOURCE_REMOVE;
        }

        bool MainDispatcher::processTimer(Timer *pTimer)
        {
            try {
                std::function<bool()> func;

                bool disposed = false;
                {
                    Mutex::Lock lock(_queueMutex);

                    func = pTimer->func;
                    disposed = pTimer->disposed;
                }

                bool funcResult;

                if (disposed) {
                    /// do not call func - stop the timer.
                    funcResult = false;
                } else {
                    try {
                        funcResult = func();
                    }
                    catch (DanglingFunctionError &) {
                        // ignore. this means that func is a weak method and
                        // the corresponding object has been deleted..
                        // We treat this as if func had returned false.
                        funcResult = false;
                    }
                }

                if (!funcResult) {
                    // timer ended
                    Mutex::Lock lock(_queueMutex);

                    // note that after erase this MainDispatcher object
                    // might be deleted (if the timer held the last reference to
                    // it)
                    _timerList.erase(pTimer->it);

                    return false;
                } else {
                    // timer continues
                    return true;
                }
            }
            catch (...) {
                if (!bdn::unhandledException(true))
                    std::terminate();

                // we should continue. Timer will also continue
                return true;
            }
        }

        void MainDispatcher::createTimer(double intervalSeconds,
                                         std::function<bool()> func)
        {
            int64_t millis = (int64_t)(intervalSeconds * 1000);

            if (millis < 1)
                millis = 1;
            else {
                Timer *pTimer;

                // note that the timer object holds a reference to us (the
                // dispatcher). So we will be kept alive as long as the timer
                // runs.

                {
                    Mutex::Lock lock(_queueMutex);
                    _timerList.push_back(Timer(func, this));
                    std::list<Timer>::iterator it = _timerList.end();
                    --it;

                    pTimer = &*it;
                    pTimer->it = it;
                }

                gdk_threads_add_timeout_full(G_PRIORITY_DEFAULT, millis,
                                             processTimerCallback, pTimer,
                                             NULL);
            }
        }
    }
}
