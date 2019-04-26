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

        virtual void bind(std::shared_ptr<Backing<ValType>> sourceBacking)
        {
            Binding binding{sourceBacking->onChange().subscribe(
                                std::bind(&Backing::bindSourceChanged, this, std::placeholders::_1)),
                            sourceBacking};

            _bindings.push_back(binding);

            bindSourceChanged(sourceBacking);
        }

        void unbind()
        {
            for (const auto &binding : _bindings) {
                if (auto strongBacking = binding.backing.lock()) {
                    strongBacking->onChange().unsubscribe(binding.subscription);
                }
            }
            _bindings.clear();
        }

      public:
        void bindSourceChanged(const std::shared_ptr<Backing<ValType>> &otherBacking) { set(otherBacking->get()); }

      protected:
        notifier_t _onChange;

        struct Binding
        {
            notifier_subscription_t subscription;
            std::weak_ptr<Backing<ValType>> backing;
        };

        std::vector<Binding> _bindings;
    };
}
