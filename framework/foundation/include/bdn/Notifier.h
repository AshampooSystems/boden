#pragma once

#include <functional>
#include <list>
#include <map>

namespace bdn
{
    struct _Subscription
    {
      public:
        size_t order;
    };
}

namespace std
{
    template <> struct less<std::shared_ptr<bdn::_Subscription>>
    {
        bool operator()(const std::shared_ptr<bdn::_Subscription> &k1,
                        const std::shared_ptr<bdn::_Subscription> &k2) const
        {
            return k1->order < k2->order;
        }
    };
}

namespace bdn
{

    template <class... Arguments> class Notifier
    {
      public:
        using Subscription = std::shared_ptr<_Subscription>;
        using Target = std::function<void(Arguments...)>;

      private:
        using SubscriptionMap = std::map<Subscription, Target>;
        using NotificationRun = typename SubscriptionMap::iterator;

      public:
        Subscription subscribe(Target target)
        {
            Subscription nextSubscription = createSubscription();

            _subscriptions.insert(_subscriptions.end(), std::make_pair(nextSubscription, target));
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

        void swap(Notifier<Arguments...> &other) { std::swap(_subscriptions, other._subscriptions); }

        void takeOverSubscriptions(Notifier<Arguments...> &other)
        {
            if (other._subscriptions.empty())
                return;

            _takeOverSubscriptions(other);
        }

        void takeOverSubscriptionsAndNotify(Notifier<Arguments...> &other, Arguments... arguments)
        {
            if (other._subscriptions.empty())
                return;

            auto firstNew = _takeOverSubscriptions(other);

            notifyFrom(firstNew, arguments...);
        }

        typename SubscriptionMap::iterator _takeOverSubscriptions(Notifier<Arguments...> &other)
        {
            auto otherSubs = other._subscriptions;
            other.unsubscribeAll();

            auto firstNew = _subscriptions.end();

            for (auto sub : otherSubs) {
                sub.first->order = nextId();
                auto newIt = _subscriptions.insert(_subscriptions.end(), sub);
                if (firstNew == _subscriptions.end()) {
                    firstNew = newIt;
                }
            }

            return firstNew;
        }

      public:
        void notify(Arguments... arguments)
        {
            if (_subscriptions.empty()) {
                return;
            }

            notifyFrom(_subscriptions.begin(), arguments...);
        }

      private:
        void notifyFrom(typename SubscriptionMap::iterator itStart, Arguments... arguments)
        {
            auto itRun = _notificationRuns.insert(_notificationRuns.end(), itStart);

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
        size_t nextId()
        {
            if (!_subscriptions.empty()) {
                return _subscriptions.rbegin()->first->order + 1;
            }
            return 0;
        }

        Subscription createSubscription() { return std::make_shared<_Subscription>(_Subscription{nextId()}); }

      private:
        SubscriptionMap _subscriptions;
        std::list<NotificationRun> _notificationRuns;
    };
}
