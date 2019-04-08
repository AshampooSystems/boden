#pragma once

#include <bdn/property/Backing.h>
#include <bdn/property/Property.h>

namespace bdn
{
    template <class T, class U> class Transform
    {
      public:
        using ToFunc = std::function<T(U)>;
        using FromFunc = std::function<U(T)>;

      public:
        Transform(const Property<U> &p, ToFunc to, FromFunc from) : property(p), toFunc(to), fromFunc(from) {}

      public:
        class Backing : public bdn::Backing<T>
        {
          public:
            using ToFunc = Transform<T, U>::ToFunc;
            using FromFunc = Transform<T, U>::FromFunc;

          public:
            Backing(const Transform &t) : toFunc(t.toFunc), fromFunc(t.fromFunc)
            {
                _otherBacking = t.property.backing();
                subscription = _otherBacking->onChange().subscribe([=](auto va) {
                    bdn::Backing<T>::onChange().notify(
                        std::dynamic_pointer_cast<IValueAccessor<T>>(IValueAccessor<T>::shared_from_this()));
                });
            }
            ~Backing() override { _otherBacking->onChange().unsubscribe(subscription); }

          public:
            T get() const override { return toFunc(_otherBacking->get()); }
            void set(const T &value, bool notify = true) override { _otherBacking->set(fromFunc(value), notify); }

          private:
            ToFunc toFunc;
            FromFunc fromFunc;

            std::shared_ptr<typename Property<U>::backing_t> _otherBacking;
            typename Property<U>::backing_t::notifier_subscription_t subscription;
        };

      public:
        const Property<U> &property;
        typename Backing::ToFunc toFunc;
        typename Backing::FromFunc fromFunc;
    };
}
