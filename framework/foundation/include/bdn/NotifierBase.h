#pragma once

#include <bdn/DanglingFunctionError.h>
#include <bdn/IAsyncNotifier.h>
#include <bdn/ISyncNotifier.h>

#include <functional>
#include <map>

namespace bdn
{
    /** Base class for notifier implementations.

        The MUTEX_TYPE template parameter indicates the type of the mutex object
       that the Notifier uses. Pass bdn::std::recursive_mutex to use a normal mutex. You can
       also pass bdn::DummyMutex to use a fake mutex that does nothing (thus
       removing multithread support).
    */
    struct atomic
    {
        std::mutex _mutex;
        void lock() { _mutex.lock(); }
        void unlock() { _mutex.unlock(); }
    };

    struct nonatomic
    {
        void lock() {}
        void unlock() {}
    };

    template <typename atomicity, class... ARG_TYPES> class NotifierBase : virtual public INotifierBase<ARG_TYPES...>
    {
      private:
        atomicity _atomicity;

      public:
        NotifierBase() {}

        ~NotifierBase() {}

        std::shared_ptr<INotifierSubscription> subscribe(const std::function<void(ARG_TYPES...)> &func) override
        {
            int64_t subId = doSubscribe(func);

            return std::make_shared<Subscription_>(subId);
        }

        INotifierBase<ARG_TYPES...> &operator+=(const std::function<void(ARG_TYPES...)> &func) override
        {
            doSubscribe(func);

            return *this;
        }

        std::shared_ptr<INotifierSubscription> subscribeParamless(const std::function<void()> &func) override
        {
            return subscribe(ParamlessFunctionAdapter(func));
        }

        void unsubscribe(std::shared_ptr<INotifierSubscription> sub) override
        {
            unsubscribeById(std::dynamic_pointer_cast<Subscription_>(sub)->subId());
        }

        void unsubscribeAll() override
        {
            std::unique_lock lock(_atomicity);

            _subMap.clear();

            NotificationState *curr = _firstNotificationState;
            while (curr != nullptr) {
                curr->nextItemIt = _subMap.end();
                curr = curr->next;
            }
        }

      protected:
        /** Perform a notification call. This is the internal implementation
            that performs the actual notification calls.

            doNotify calls this.

            Template parameters:

            - CALL_MAKER_TYPE the type of the call maker function (see callMaker
           parameter)
            - ADDITIONAL_CALL_MAKER_ARGS the types of additional arguments to
           pass to the call maker function (see additionalCallMakerArgs
           parameter).

            \param callMaker a helper function that performs the actual
           individual call for a single subscriber. As its first parameter this
           function gets the subscribed function object (of type
           std::function<void(ARG_TYPES...) ). It can optionally also get
           additional arguments, as specified by additionalCallMakerArgs. \param
           additionalCallMakerArgs an arbitrary number of additional arguments
                that are passed to the call maker function. The number and type
           of these are controlled by the function's template argument
           ADDITIONAL_CALL_MAKER_ARGS.
         */
        template <typename CALL_MAKER_TYPE, typename... ADDITIONAL_CALL_MAKER_ARGS>
        void notifyImpl(CALL_MAKER_TYPE callMaker, ADDITIONAL_CALL_MAKER_ARGS... additionalCallMakerArgs)
        {
            // we do not want to hold a mutex while we call each subscriber.
            // That would create the potential for deadlocks. However, we need
            // to hold a mutex to access our internal structures, up to the
            // point in time when we actually make the call.

            {
                std::unique_lock lock(_atomicity);

                // We also need to ensure that subscribers that are removed
                // while our notification call is running are not called after
                // they are removed (unless their specific call has already
                // started).

                // When we have an async notifier then there is usually only one
                // notification call active at any given point in time, since
                // all of them are made from the main thread. However, if the
                // event loop does work somewhere in an inner function (e.g. by
                // a modal dialog created by some other framework) then a second
                // notification call might be executed before the first can
                // finish. The mutex does not protect against this since both
                // calls would happen in the same thread (the main thread).

                // When we are a synchronous notifier then there can also be
                // additional notify calls happening while we are inside the
                // loop (if one of our subscribers causes another notification
                // to happen). So we also have to be re-entrant in that case.

                NotificationState state;

                state.nextItemIt = _subMap.begin();

                activateNotificationState(&state);

                try {
                    while (state.nextItemIt != _subMap.end()) {
                        std::pair<int64_t, Sub_> item = *state.nextItemIt;

                        // increase the iterator before we call the notification
                        // function. That is necessary for cases when someone
                        // removes the next item in our map before we get to it.
                        // Note that the unsubscribe function will update the
                        // iterator properly, to ensure that it remains valid.
                        state.nextItemIt++;

                        // now we have to release the mutex. At this point in
                        // time the function might be unsubscribed by another
                        // thread, but we cannot stop the call once we started
                        // it. We also cannot block unsubscribes during this
                        // call, since that can open up the potential for
                        // deadlocks. So the caller of unsubscribe has to deal
                        // with the fact that the function might be called once
                        // more directly after unsubscribe finishes.

                        try {
                            _atomicity.unlock();

                            // note: we MUST NOT use std::forward here, since we
                            // may have to call multiple subscribers.
                            // std::forward might convert the temporary object
                            // to a move reference, this the
                            // additionalCallMakerArgs variable might otherwise
                            // be invalidated by the first subscriber call.
                            callMaker(item.second.func, additionalCallMakerArgs...);
                            _atomicity.lock();
                        }
                        catch (DanglingFunctionError &) {
                            // this is a perfectly normal case. It means that
                            // the target function was a weak reference and the
                            // target object has been destroyed. Just remove it
                            // from our list and ignore the exception.

                            unsubscribeById(item.first);
                        }
                    }
                }
                catch (...) {
                    deactivateNotificationState(&state);
                    throw;
                }

                deactivateNotificationState(&state);
            }
        }

        /** A default call maker implementation that simply calls the subscribed
           function directly. This can be used with notifyImpl().*/
        static void defaultCallMaker(const std::function<void(ARG_TYPES...)> &func, ARG_TYPES... args)
        {
            func(args...);
        }

        /** Perform a notification call.*/
        virtual void doNotify(ARG_TYPES... args)
        {
            notifyImpl<decltype(&NotifierBase::defaultCallMaker), ARG_TYPES...>(&NotifierBase::defaultCallMaker,
                                                                                args...);
        }

        /** Subscribed the specified function \c func.

            Returns the ID of the created subscription.
        */
        virtual int64_t doSubscribe(const std::function<void(ARG_TYPES...)> &func)
        {
            std::unique_lock lock(_atomicity);

            int64_t subId = _nextSubId;
            _nextSubId++;

            _subMap[subId] = Sub_(func);

            return subId;
        }

      private:
        struct Sub_
        {
            Sub_() {}

            Sub_(const std::function<void(ARG_TYPES...)> &func) : func(func) {}

            std::function<void(ARG_TYPES...)> func;
        };

        struct NotificationState
        {
            NotificationState *next = nullptr;

            typename std::map<int64_t, Sub_>::iterator nextItemIt;
        };

        void unsubscribeById(int64_t subId)
        {
            std::unique_lock lock(_atomicity);

            auto it = _subMap.find(subId);
            if (it != _subMap.end()) {
                // notifications are only done in the main thread.
                // But there can still be multiple notifications running, if the
                // event loop is worked from an inner function (like a modal
                // dialog that was created by another framwork).
                NotificationState *state = _firstNotificationState;
                while (state != nullptr) {
                    if (state->nextItemIt == it)
                        state->nextItemIt++;

                    state = state->next;
                }

                _subMap.erase(it);
            }
        }

        void activateNotificationState(NotificationState *state)
        {
            state->next = _firstNotificationState;
            _firstNotificationState = state;
        }

        void deactivateNotificationState(NotificationState *state)
        {
            // we usually have only one notification state active
            if (state == _firstNotificationState)
                _firstNotificationState = state->next;
            else {
                NotificationState *prev = _firstNotificationState;
                NotificationState *curr = _firstNotificationState->next;

                while (curr != nullptr) {
                    if (curr == state) {
                        prev->next = state->next;
                        break;
                    }

                    state = state->next;
                }
            }
        }

        class ParamlessFunctionAdapter
        {
          public:
            ParamlessFunctionAdapter(std::function<void()> func) { _func = func; }

            void operator()(ARG_TYPES... args) { _func(); }

          protected:
            std::function<void()> _func;
        };

        class Subscription_ : public Base, virtual public INotifierSubscription
        {
          public:
            Subscription_(int64_t subId) : _subId(subId) {}

            int64_t subId() const { return _subId; }

          private:
            int64_t _subId;
        };

        int64_t _nextSubId = 1;
        std::map<int64_t, Sub_> _subMap;
        NotificationState *_firstNotificationState = nullptr;
    };
}
