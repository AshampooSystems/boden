
#import <bdn/foundationkit/MainDispatcher.hh>

#include <bdn/log.h>
#include <bdn/mainThread.h>
#include <bdn/entry.h>
#include <bdn/InvalidArgumentError.h>
#include <iostream>

#import <Foundation/Foundation.h>

@interface BdnFkDispatchFuncWrapper_ : NSObject {
}

@property std::function<void()> func;
@property bdn::IDispatcher::Duration delay;

- (void)invoke;

@end

@implementation BdnFkDispatchFuncWrapper_

@synthesize func;
@synthesize delay;

- (void)invoke
{
    bdn::platformEntryWrapper(
        [=]() {
            if (delay <= bdn::IDispatcher::Duration::zero()) {
                try {
                    func();
                }
                catch (bdn::DanglingFunctionError &) {
                    // ignore. This means that the function is a weak method
                    // whose object has been deleted. This is treated like a
                    // no-op.
                }
            } else {

                double delayInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(delay).count();

                delay = bdn::IDispatcher::Duration::zero();

                [self performSelector:@selector(invoke) withObject:nil afterDelay:delayInSeconds];
            }
        },
        true);
}

@end

@interface BdnFkTimerFuncWrapper_ : NSObject {
    std::shared_ptr<bdn::fk::MainDispatcher::TimerFuncList_> timerFuncList;
    std::list<std::function<bool()>>::iterator timerFuncIt;
}

@property std::shared_ptr<bdn::fk::MainDispatcher::TimerFuncList_> timerFuncList;
@property std::list<std::function<bool()>>::iterator timerFuncIt;

- (void)targetMethod:(NSTimer *)theTimer;

@end

@implementation BdnFkTimerFuncWrapper_

@synthesize timerFuncList;
@synthesize timerFuncIt;

- (void)targetMethod:(NSTimer *)theTimer
{
    bdn::platformEntryWrapper(
        [&]() {
            if (timerFuncList != nullptr) {
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

                    timerFuncList->funcList.erase(timerFuncIt);
                    timerFuncList = nullptr;
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
            _idleQueue = std::make_shared<IdleQueue>();
            _timerFuncList = std::make_shared<TimerFuncList_>();
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
                    CFRunLoopRemoveObserver([NSRunLoop mainRunLoop].getCFRunLoop, obs, kCFRunLoopCommonModes);
                });

                _idleObserverInstalled = false;
            }
        }

        void MainDispatcher::dispose()
        {
            std::unique_lock lock(_queueMutex);

            // empty our idle queue.
            _idleQueue->dispose();

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
            for (std::function<bool()> &item : _timerFuncList->funcList) {
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

        void MainDispatcher::_scheduleMainThreadCall(const std::function<void()> &func, IDispatcher::Duration delay)
        {
            // if delay is >0 then we also simply schedule an immediate
            // call, because there is no performSelectorOnMainThread function
            // that takes a delay parameter. So we first schedule the call
            // without a delay and then from the main thread we reschedule with
            // a delay (using performSelector:withObject:afterDelay, which must
            // be called from the main thread).

            BdnFkDispatchFuncWrapper_ *wrapper = [[BdnFkDispatchFuncWrapper_ alloc] init];
            wrapper.func = func;
            wrapper.delay = delay;

            [wrapper performSelectorOnMainThread:@selector(invoke) withObject:nil waitUntilDone:NO];
        }

        void MainDispatcher::enqueue(std::function<void()> func, Priority priority)
        {
            enqueueDelayed(IDispatcher::Duration::zero(), func, priority);
        }

        void MainDispatcher::enqueueDelayed(IDispatcher::Duration delay, std::function<void()> func, Priority priority)
        {
            if (priority == Priority::normal) {
                std::shared_ptr<MainDispatcher> self(shared_from_this());

                // we do not schedule the func call directly. Instead we add
                // func to our own queue and schedule a call to our
                // callNextQueued. That enables us to dipose all queued items
                // when dipose() is called.

                if (delay <= IDispatcher::Duration::zero()) {
                    {
                        std::unique_lock lock(_queueMutex);
                        _normalQueue.push_back(func);
                    }

                    _scheduleMainThreadCall([self] { self->callNextNormalItem(); });
                } else {
                    std::list<std::function<void()>>::iterator it;
                    {
                        std::unique_lock lock(_queueMutex);
                        _timedNormalQueue.push_back(func);
                        it = _timedNormalQueue.end();
                        --it;
                    }

                    _scheduleMainThreadCall([self, it] { self->callTimedItem(it); }, delay);
                }
            } else if (priority == Priority::idle) {
                // we must only access the run loop from the main thread.
                // Also, if the run loop is currently idle (=waiting) then
                // we need to break that idle phase so that we will get
                // a notification when the loop is idle next.

                // Both of these can be solved by enqueuing the setup code with
                // normal priority

                std::shared_ptr<MainDispatcher> self(shared_from_this());

                _scheduleMainThreadCall(
                    [self, func] {
                        self->_idleQueue->add(func);

                        self->ensureIdleObserverInstalled();
                    },

                    delay);
            } else
                throw InvalidArgumentError("MainDispatcher::enqueueIn"
                                           "called with invalid priority: " +
                                           std::to_string((int)priority));
        }

        void MainDispatcher::callNextNormalItem()
        {
            std::function<void()> func;

            {
                std::unique_lock lock(_queueMutex);

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

        void MainDispatcher::callTimedItem(std::list<std::function<void()>>::iterator it)
        {
            std::function<void()> func;

            {
                std::unique_lock lock(_queueMutex);
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

        void MainDispatcher::createTimer(IDispatcher::Duration interval, std::function<bool()> func)
        {
            std::list<std::function<bool()>>::iterator it;
            {
                std::unique_lock lock(_queueMutex);
                _timerFuncList->funcList.push_back(func);
                it = _timerFuncList->funcList.end();
                --it;
            }

            BdnFkTimerFuncWrapper_ *wrapper = [[BdnFkTimerFuncWrapper_ alloc] init];
            wrapper.timerFuncList = _timerFuncList;
            wrapper.timerFuncIt = it;

            double intervalInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(interval).count();

            [NSTimer scheduledTimerWithTimeInterval:intervalInSeconds
                                             target:wrapper
                                           selector:@selector(targetMethod:)
                                           userInfo:nil
                                            repeats:YES];
        }

        void MainDispatcher::ensureIdleObserverInstalled()
        {
            if (!_idleObserverInstalled) {
                std::shared_ptr<IdleQueue> queue = _idleQueue;

                id handler = ^(CFRunLoopObserverRef observer, CFRunLoopActivity activity) {
                  if (activity == kCFRunLoopBeforeWaiting)
                      queue->activateNext();
                };

                _idleObserver = CFRunLoopObserverCreateWithHandler(kCFAllocatorDefault, kCFRunLoopAllActivities, true,
                                                                   0 /* order */, handler);
                CFRunLoopAddObserver([NSRunLoop mainRunLoop].getCFRunLoop, _idleObserver, kCFRunLoopCommonModes);
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
