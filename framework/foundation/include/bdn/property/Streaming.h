#pragma once

#include <sstream>
#include <variant>

#include <bdn/property/Backing.h>
#include <bdn/property/Property.h>

namespace bdn
{
    class Streaming
    {
      public:
        class Backing;

      private:
        struct stringablebase
        {
            virtual ~stringablebase() = default;
            virtual void visit(Streaming::Backing *backing) = 0;
        };
        template <class T> struct stringableproperty : public stringablebase
        {
            const Property<T> &property;
            stringableproperty(const Property<T> &p) : property(p) {}
            void visit(Streaming::Backing *backing) override { *backing << property; }
        };
        template <class T> struct stringablevalue : public stringablebase
        {
            stringablevalue(T v) : _value(std::move(v)) {}
            void visit(Streaming::Backing *backing) override { *backing << _value; }
            T _value;
        };

      public:
        template <class T> Streaming &operator<<(const Property<T> &property)
        {
            std::unique_ptr<stringablebase> r(std::move(std::make_unique<stringableproperty<T>>(property)));
            _elements.emplace_back(std::move(r));
            return *this;
        }
        template <class T> Streaming &operator<<(T value)
        {
            std::unique_ptr<stringablebase> r(std::move(std::make_unique<stringablevalue<T>>(value)));
            _elements.emplace_back(std::move(r));
            return *this;
        }

      protected:
        void visit(Streaming::Backing *backing)
        {
            for (auto &e : _elements) {
                e->visit(backing);
            }
        }

      protected:
        std::vector<std::unique_ptr<stringablebase>> _elements;

      public:
        class Backing : public bdn::Backing<String>
        {
            friend class Streaming;

          private:
            struct ToStringBase
            {
                virtual ~ToStringBase() = default;
                virtual void toString(std::ostringstream &stream) = 0;
                Notifier<> changed;
            };

            template <class ValType> struct ToString : public ToStringBase
            {
                const Property<ValType> &property;
                ToString(const Property<ValType> &p) : property(p)
                {
                    property.onChange() += [=](auto) { changed.notify(); };
                }

                void toString(std::ostringstream &stream) override { stream << property.get(); }
            };

            template <class T> struct ValueToString : public ToStringBase
            {
                ValueToString(T v) : value(std::move(v)) {}
                void toString(std::ostringstream &stream) override { stream << value; }
                T value;
            };

          public:
            Backing(Streaming &stream)
            {
                stream.visit(this);
                updateValue();
            }

          protected:
            template <class OtherValueType> Backing &operator<<(const Property<OtherValueType> &other)
            {
                std::unique_ptr<ToStringBase> newPropertyUnique(
                    std::move(std::make_unique<ToString<OtherValueType>>(other)));
                newPropertyUnique->changed += std::bind(&Streaming::Backing::onPropertyChanged, this);
                _properties.emplace_back(std::move(newPropertyUnique));
                return *this;
            }

            template <class T> Backing &operator<<(T value)
            {
                std::unique_ptr<ToStringBase> newValueUnique(
                    std::move(std::make_unique<ValueToString<T>>(std::move(value))));
                _properties.emplace_back(std::move(newValueUnique));
                return *this;
            }

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
                _pOnChange->notify(shared_from_this());
            }

          private:
            std::vector<std::unique_ptr<ToStringBase>> _properties;

          public:
            String get() const override { return _value; }
            void set(const String &value, bool notify) override {}

          private:
            String _value;
        };
    };
}
