#pragma once

#include <bdn/Notifier.h>
#include <memory>
#include <utility>
#include <vector>

namespace bdn
{
    template <class ValType> class Property;

    template <class ValType> class Backing : public std::enable_shared_from_this<Backing<ValType>>
    {
      public:
        class Proxy
        {
          public:
            Proxy(ValType value) : _value(std::move(value)) {}

            const ValType *operator->() const { return &_value; }

          private:
            ValType _value;
        };

        using notifier_t = Notifier<std::shared_ptr<Backing<ValType>>>;
        using notifier_t_ptr = std::shared_ptr<notifier_t>;
        using notifier_subscription_t = typename notifier_t::Subscription;
        using property_t = Property<ValType>;
        using property_t_ptr = std::shared_ptr<property_t>;

      public:
        Backing() {}
        virtual ~Backing() { unbind(); }

        virtual ValType get() const = 0;
        virtual void set(const ValType &value, bool notify = true) = 0;

        virtual Proxy proxy() const { return Proxy(get()); }

        notifier_t &onChange() { return _onChange; }

        template <typename OtherType> void bind(std::shared_ptr<Backing<OtherType>> sourceBacking)
        {
            static_assert(std::is_convertible<OtherType, ValType>::value ||
                              std::is_constructible<OtherType, ValType>::value,
                          "Types are not convertible");

            auto subscription = sourceBacking->onChange().subscribe(
                [this](const auto &sourceBacking) { this->bindSourceChanged(sourceBacking); });

            std::weak_ptr<Backing<OtherType>> weakSourceBacking = sourceBacking;

            Binding binding = [subscription, weakSourceBacking]() {
                if (auto source = weakSourceBacking.lock()) {
                    source->onChange().unsubscribe(subscription);
                }
            };

            _bindings.push_back(binding);

            bindSourceChanged(sourceBacking);
        }

        void unbind()
        {
            for (const auto &unbind : _bindings) {
                unbind();
            }
            _bindings.clear();
        }

      public:
        template <typename OtherType> void bindSourceChanged(const std::shared_ptr<Backing<OtherType>> &sourceBacking)
        {
            set(sourceBacking->get());
        }

      protected:
        notifier_t _onChange;

        using Binding = std::function<void()>;

        std::vector<Binding> _bindings;
    };
}
