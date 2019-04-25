#pragma once

#include <bdn/property/Backing.h>
#include <bdn/property/Property.h>

namespace bdn
{
    template <class ValType, class OtherValType> class TransformBacking : public Backing<ValType>
    {
      public:
        using ToFunc = std::function<ValType(OtherValType)>;
        using FromFunc = std::function<OtherValType(ValType)>;

      public:
        TransformBacking(const Property<OtherValType> &p, ToFunc to, FromFunc from)
            : toFunc(to), fromFunc(from), _otherBacking(p.backing())
        {}

        TransformBacking(const TransformBacking &t)
            : toFunc(t.toFunc), fromFunc(t.fromFunc), _otherBacking(t._otherBacking)
        {
            subscription = _otherBacking->onChange().subscribe([=](auto va) {
                bdn::Backing<ValType>::onChange().notify(
                    std::dynamic_pointer_cast<IValueAccessor<ValType>>(IValueAccessor<ValType>::shared_from_this()));
            });
        }

        ~TransformBacking() override
        {
            if (subscription)
                _otherBacking->onChange().unsubscribe(subscription);
        }

      public:
        ValType get() const override { return toFunc(_otherBacking->get()); }
        void set(const ValType &value, bool notify = true) override { _otherBacking->set(fromFunc(value), notify); }

      private:
        ToFunc toFunc;
        FromFunc fromFunc;

        std::shared_ptr<typename Property<OtherValType>::backing_t> _otherBacking;
        typename Property<OtherValType>::backing_t::notifier_subscription_t subscription;
    };
}
