#include <bdn/init.h>
#import <bdn/foundationkit/MainDispatcher.hh>

#include <bdn/log.h>
#include <bdn/mainThread.h>
#include <bdn/entry.h>

#import <Foundation/Foundation.h>

@interface BdnFkDispatchFuncWrapper_ : NSObject {
    std::function<void()> func;
    double delaySeconds;
}

@property std::function<void()> func;
@property double delaySeconds;

- (void)invoke;

@end

@implementation BdnFkDispatchFuncWrapper_

@synthesize func;
@synthesize delaySeconds;

- (void)invoke
{
    bdn::platformEntryWrapper(
        [=]() {
            if (delaySeconds <= 0) {
                try {
                    func();
                }
                catch (bdn::DanglingFunctionError &) {
                    // ignore. This means that the function is a weak method
                    // whose object has been deleted. This is treated like a
                    // no-op.
                }
            } else {
                double delay = delaySeconds;
                delaySeconds = 0; // set to 0 so that we will not wait again

                [self performSelector:@selector(invoke)
                           withObject:nil
                           afterDelay:delay];
            }
        },
        true);
}

@end

@interface BdnFkTimerFuncWrapper_ : NSObject {
    bdn::P<bdn::fk::MainDispatcher::TimerFuncList_> pTimerFuncList;
    std::list<std::function<bool()>>::iterator timerFuncIt;
}

@property bdn::P<bdn::fk::MainDispatcher::TimerFuncList_> pTimerFuncList;
@property std::list<std::function<bool()>>::iterator timerFuncIt;

- (void)targetMethod:(NSTimer *)theTimer;

@end

@implementation BdnFkTimerFuncWrapper_

@synthesize pTimerFuncList;
@synthesize timerFuncIt;

- (void)targetMethod:(NSTimer *)theTimer
{
    bdn::platformEntryWrapper(
        [&]() {
            if (pTimerFuncList != nullptr) {
                std::function<bool()> &timerFunc = *timerFuncIt;

                bool result;
                try {
                    result = timerFunc();
                }
                catch (bdn::DanglingFunctionError &) {
                    // ignore. This means that the function is a weak method
                    // whose object has been deleted. This is treated as if the
                    // function had returned false (i.e. the timer is stopped)
                    result = false;
                }

                if (!result) {
                    [theTimer invalidate];

                    pTimerFuncList->funcList.erase(timerFuncIt);
                    pTimerFuncList = nullptr;
                }
            }
        },
        true);
}

@end

namespace bdn
{
    namespace fk
    {

        MainDispatcher::MainDispatcher()
        {
            _pIdleQueue = newObj<IdleQueue>();
            _pTimerFuncList = newObj<TimerFuncList_>();
        }

        MainDispatcher::~MainDispatcher()
        {
            if (_idleObserverInstalled) {
                // unregister our observer with a call from the main thread
                // because this destructor might be called from a different
                // thread. Note that the observer only holds a reference to the
                // IdleQueue object, which remains valid as long as the observer
                // exists. So the scheduled call does not access the destructed
                // MainDispatcher object.
                CFRunLoopObserverRef obs = _idleObserver;
                _scheduleMainThreadCall([obs]() {
                    CFRunLoopRemoveObserver(
                        [NSRunLoop mainRunLoop].getCFRunLoop, obs,
                        kCFRunLoopCommonModes);
                });

                _idleObserverInstalled = false;
            }
        }

        void MainDispatcher::dispose()
        {
            Mutex::Lock lock(_queueMutex);

            // empty our idle queue.
            _pIdleQueue->dispose();

            while (!_normalQueue.empty()) {
                BDN_LOG_AND_IGNORE_EXCEPTION(
                    { // make a copy so that pop_front is not aborted if the
                      // destructor fails.
                        std::function<void()> item = _normalQueue.front();
                        _normalQueue.pop_front();
                    },
                    "Error clearing MainDispatcher normal queue item during "
                    "dispose. Ignoring.");
            }

            // for the timed queue the pending calls have iterators to the timed
            // item. So we cannot clear the queue. Instead we have to invalidate
            // the items.

            for (std::function<void()> &item : _timedNormalQueue) {
                BDN_LOG_AND_IGNORE_EXCEPTION(
                    {
                        // make a copy so that pop_front is not aborted if the
                        // destructor fails.
                        std::function<void()> itemCopy = item;
                        item = std::function<void()>();
                    },
                    "Error clearing MainDispatcher timed normal queue item "
                    "during dispose. Ignoring.");
            }

            // for timers we also cannot remove the items. So do the same thing
            // that we did for the normal timed items
            for (std::function<bool()> &item : _pTimerFuncList->funcList) {
                BDN_LOG_AND_IGNORE_EXCEPTION(
                    {
                        // make a copy so that pop_front is not aborted if the
                        // destructor fails.
                        std::function<bool()> itemCopy = item;
                        item = std::function<bool()>();
                    },
                    "Error clearing MainDispatcher timed normal queue item "
                    "during dispose. Ignoring.");
            }
        }

        void MainDispatcher::_scheduleMainThreadCall(
            const std::function<void()> &func, double delaySeconds)
        {
            // if delaySeconds is >0 then we also simply schedule an immediate
            // call, because there is no performSelectorOnMainThread function
            // that takes a delay parameter. So we first schedule the call
            // without a delay and then from the main thread we reschedule with
            // a delay (using performSelector:withObject:afterDelay, which must
            // be called from the main thread).

            BdnFkDispatchFuncWrapper_ *wrapper =
                [[BdnFkDispatchFuncWrapper_ alloc] init];
            wrapper.func = func;
            wrapper.delaySeconds = delaySeconds;

            [wrapper performSelectorOnMainThread:@selector(invoke)
                                      withObject:nil
                                   waitUntilDone:NO];
        }

        void MainDispatcher::enqueue(std::function<void()> func,
                                     Priority priority)
        {
            enqueueInSeconds(0, func, priority);
        }

        void MainDispatcher::enqueueInSeconds(double seconds,
                                              std::function<void()> func,
                                              Priority priority)
        {
            if (priority == Priority::normal) {
                P<MainDispatcher> pThis = this;

                // we do not schedule the func call directly. Instead we add
                // func to our own queue and schedule a call to our
                // callNextQueued. That enables us to dipose all queued items
                // when dipose() is called.

                if (seconds <= 0) {
                    {
                        Mutex::Lock lock(_queueMutex);
                        _normalQueue.push_back(func);
                    }

                    _scheduleMainThreadCall(
                        [pThis] { pThis->callNextNormalItem(); });
                } else {
                    std::list<std::function<void()>>::iterator it;
                    {
                        Mutex::Lock lock(_queueMutex);
                        _timedNormalQueue.push_back(func);
                        it = _timedNormalQueue.end();
                        --it;
                    }

                    _scheduleMainThreadCall(
                        [pThis, it] { pThis->callTimedItem(it); }, seconds);
                }
            } else if (priority == Priority::idle) {
                // we must only access the run loop from the main thread.
                // Also, if the run loop is currently idle (=waiting) then
                // we need to break that idle phase so that we will get
                // a notification when the loop is idle next.

                // Both of these can be solved by enqueuing the setup code with
                // normal priority

                P<MainDispatcher> pThis = this;

                _scheduleMainThreadCall(
                    [pThis, func] {
                        pThis->_pIdleQueue->add(func);

                        pThis->ensureIdleObserverInstalled();
                    },

                    seconds);
            } else
                throw InvalidArgumentError("MainDispatcher::enqueueInSeconds "
                                           "called with invalid priority: " +
                                           std::to_string((int)priority));
        }

        void MainDispatcher::callNextNormalItem()
        {
            std::function<void()> func;

            {
                Mutex::Lock lock(_queueMutex);

                if (_normalQueue.empty())
                    return;

                // make a copy so that exceptions in the destructor do not
                // cause an invalid list state. Also because we need to hold the
                // mutex when we access the queue.
                func = _normalQueue.front();
                _normalQueue.pop_front();
            }

            try {
                func();
            }
            catch (DanglingFunctionError &) {
                // ignore. This means that the function is a weak method
                // whose object has been deleted. This is treated like a no-op.
            }
        }

        void MainDispatcher::callTimedItem(
            std::list<std::function<void()>>::iterator it)
        {
            std::function<void()> func;

            {
                Mutex::Lock lock(_queueMutex);
                func = *it;
                _timedNormalQueue.erase(it);
            }

            try {
                func();
            }
            catch (DanglingFunctionError &) {
                // ignore. This means that the function is a weak method
                // whose object has been deleted. This is treated like a no-op.
            }
        }

        void MainDispatcher::createTimer(double intervalSeconds,
                                         std::function<bool()> func)
        {
            std::list<std::function<bool()>>::iterator it;
            {
                Mutex::Lock lock(_queueMutex);
                _pTimerFuncList->funcList.push_back(func);
                it = _pTimerFuncList->funcList.end();
                --it;
            }

            BdnFkTimerFuncWrapper_ *wrapper =
                [[BdnFkTimerFuncWrapper_ alloc] init];
            wrapper.pTimerFuncList = _pTimerFuncList;
            wrapper.timerFuncIt = it;

            [NSTimer scheduledTimerWithTimeInterval:intervalSeconds
                                             target:wrapper
                                           selector:@selector(targetMethod:)
                                           userInfo:nil
                                            repeats:YES];
        }

        void MainDispatcher::ensureIdleObserverInstalled()
        {
            if (!_idleObserverInstalled) {
                P<IdleQueue> pQueue = _pIdleQueue;

                id handler = ^(CFRunLoopObserverRef observer,
                               CFRunLoopActivity activity) {
                  if (activity == kCFRunLoopBeforeWaiting)
                      pQueue->activateNext();
                };

                _idleObserver = CFRunLoopObserverCreateWithHandler(
                    kCFAllocatorDefault, kCFRunLoopAllActivities, true,
                    0 /* order */, handler);
                CFRunLoopAddObserver([NSRunLoop mainRunLoop].getCFRunLoop,
                                     _idleObserver, kCFRunLoopCommonModes);
                CFRelease(_idleObserver);

                _idleObserverInstalled = true;
            }
        }

        void MainDispatcher::IdleQueue::activateNext()
        {
            // enqueue the next idle call in the normal loop.
            // Afterwards, if nothing else is pending, we will be called again
            // to enqueue the next one.

            // Note that funcList can be empty if the queue was disposed
            if (!_funcList.empty()) {
                _scheduleMainThreadCall(_funcList.front());
                _funcList.pop_front();
            }
        }

        void MainDispatcher::IdleQueue::dispose()
        {
            while (!_funcList.empty()) {
                BDN_LOG_AND_IGNORE_EXCEPTION(
                    { // make a copy so that pop_front is not aborted if the
                      // destructor fails.
                        std::function<void()> item = _funcList.front();
                        _funcList.pop_front();
                    },
                    "Error clearing MainDispatcher::IdleQueue item during "
                    "dispose. Ignoring.");
            }
        }
    }
}
