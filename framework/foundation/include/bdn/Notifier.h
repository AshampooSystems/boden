#pragma once

#include <functional>
#include <list>
#include <map>

namespace bdn
{
    template <class... Arguments> class Notifier
    {
      public:
        using Subscription = size_t;
        using Target = std::function<void(Arguments...)>;

      private:
        using SubscriptionMap = std::map<Subscription, Target>;
        using NotificationRun = typename SubscriptionMap::iterator;

      public:
        Subscription subscribe(Target target)
        {
            Subscription nextSubscription = 0;
            if (_subscriptions.size() > 0) {
                nextSubscription = _subscriptions.rbegin()->first + 1;
            }

            _subscriptions[nextSubscription] = target;
            return nextSubscription;
        }

        void unsubscribe(Subscription subscription)
        {
            auto it = _subscriptions.find(subscription);
            if (it != _subscriptions.end()) {
                for (auto itRunning = _notificationRuns.begin(); itRunning != _notificationRuns.end(); itRunning++) {
                    if (*itRunning == it) {
                        (*itRunning)++;
                    }
                }
                _subscriptions.erase(it);
            }
        }

        void unsubscribeAll()
        {
            _subscriptions.clear();
            for (auto itRunning = _notificationRuns.begin(); itRunning != _notificationRuns.end(); itRunning++) {
                (*itRunning) = _subscriptions.end();
            }
        }

        Notifier<Arguments...> operator+=(Target target)
        {
            subscribe(target);
            return *this;
        }

      public:
        void notify(Arguments... arguments)
        {
            if (_subscriptions.empty()) {
                return;
            }

            auto itRun = _notificationRuns.insert(_notificationRuns.end(), _subscriptions.begin());

            auto &notificationRun = *itRun;
            auto it = notificationRun;
            do {
                notificationRun++;
                it->second(arguments...);
                it = notificationRun;
            } while (it != _subscriptions.end());

            _notificationRuns.erase(itRun);
        }

      private:
        SubscriptionMap _subscriptions;
        std::list<NotificationRun> _notificationRuns;
    };
}
