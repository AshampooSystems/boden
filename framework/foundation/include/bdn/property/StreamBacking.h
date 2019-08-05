#pragma once

#include <sstream>
#include <variant>

#include <bdn/property/Backing.h>
#include <bdn/property/Property.h>

namespace bdn
{
    class StreamBacking : public Backing<std::string>
    {
      private:
        struct ToStringBase
        {
            virtual ~ToStringBase() = default;
            virtual void toString(std::ostringstream &stream) = 0;
            virtual void cloneInto(StreamBacking &sb) = 0;
            Notifier<> changed;
        };

        template <class ValType> struct ToString : public ToStringBase
        {
            const Property<ValType> &property;
            typename Property<ValType>::backing_t::notifier_t::Subscription propertySubscription;

            ToString(const Property<ValType> &p) : property(p)
            {
                propertySubscription = property.onChange().subscribe([=](auto) { changed.notify(); });
            }
            ~ToString() { property.onChange().unsubscribe(propertySubscription); }

            void toString(std::ostringstream &stream) override { stream << property.get(); }

            void cloneInto(StreamBacking &sb) override { sb << property; }
        };

        template <class T> struct ValueToString : public ToStringBase
        {
            ValueToString(T v) : value(std::move(v)) {}
            void toString(std::ostringstream &stream) override { stream << value; }
            void cloneInto(StreamBacking &sb) override { sb << value; }

            T value;
        };

      public:
        StreamBacking() {}

        StreamBacking(const StreamBacking &other)
        {
            for (auto &p : other._properties) {
                p->cloneInto(*this);
            }

            updateValue();
        }

        template <class OtherValueType> StreamBacking &operator<<(const Property<OtherValueType> &other)
        {
            std::unique_ptr<ToStringBase> newPropertyUnique(
                std::move(std::make_unique<ToString<OtherValueType>>(other)));
            newPropertyUnique->changed += std::bind(&StreamBacking::onPropertyChanged, this);
            _properties.emplace_back(std::move(newPropertyUnique));
            return *this;
        }

        template <class T> StreamBacking &operator<<(T value)
        {
            std::unique_ptr<ToStringBase> newValueUnique(
                std::move(std::make_unique<ValueToString<T>>(std::move(value))));
            _properties.emplace_back(std::move(newValueUnique));
            return *this;
        }

      protected:
        void updateValue()
        {
            std::ostringstream stream;
            for (auto &p : _properties) {
                p->toString(stream);
            }

            _value = stream.str();
        }

        void onPropertyChanged()
        {
            updateValue();
            _onChange.notify(shared_from_this());
        }

      private:
        std::vector<std::unique_ptr<ToStringBase>> _properties;

      public:
        std::string get() const override { return _value; }
        void set(const std::string &value, bool notify) override {}

      private:
        std::string _value;
    };
}
