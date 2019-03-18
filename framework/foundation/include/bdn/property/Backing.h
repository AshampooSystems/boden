#pragma once

#include <bdn/SimpleNotifier.h>
#include <bdn/property/IValueAccessor.h>
#include <memory>
#include <utility>
#include <vector>

namespace bdn
{
    template <class ValType> class Property;

    template <class ValType> class Backing : virtual public IValueAccessor<ValType>
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

        using value_accessor_t = IValueAccessor<ValType>;
        using value_accessor_t_ptr = std::shared_ptr<value_accessor_t>;
        using notifier_t = SimpleNotifier<std::shared_ptr<value_accessor_t>>;
        using notifier_t_ptr = std::shared_ptr<notifier_t>;
        using property_t = Property<ValType>;
        using property_t_ptr = std::shared_ptr<property_t>;

      public:
        Backing() : _pOnChange(std::make_shared<notifier_t>()) {}
        ~Backing() override { unbind(); }

        std::shared_ptr<Backing<ValType>> shared_from_this()
        {
            return std::dynamic_pointer_cast<Backing<ValType>>(IValueAccessor<ValType>::shared_from_this());
        }

        ValType get() const override = 0;
        virtual void set(const ValType &value, bool notify = true) = 0;

        virtual Proxy proxy() const { return Proxy(get()); }

        virtual notifier_t &onChange() const { return *(_pOnChange.get()); }

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
        void bindSourceChanged(value_accessor_t_ptr pValue) { set(pValue->get()); }

      protected:
        mutable notifier_t_ptr _pOnChange;

        struct Binding
        {
            std::shared_ptr<INotifierSubscription> subscription;
            std::weak_ptr<Backing<ValType>> backing;
        };

        std::vector<Binding> _bindings;
    };
}
