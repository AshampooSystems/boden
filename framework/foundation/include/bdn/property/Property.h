#pragma once

#include <bdn/NotifierBase.h>
#include <bdn/property/IValueAccessor.h>

#include <bdn/property/GetterSetter.h>
#include <bdn/property/GetterSetterBacking.h>
#include <bdn/property/InternalValueBacking.h>
#include <bdn/property/Setter.h>
#include <bdn/property/SetterBacking.h>
#include <bdn/property/property_forward_decl.h>

namespace bdn
{
    enum class BindMode
    {
        unidirectional,
        bidirectional
    };

    /** Represents a property of a class.

        Properties provide a simple means for accessing, manipulating and
       observing arbitrary value types such as int or String via the ValType
       template parameter. You may observe value changes using the onChange()
       method or bind two or more properties using the bind() method.

        Usage
        =====

        Properties are intended to be exposed as public data members of a class:

        \code
        class Circle
        {
        public:
            Property<double> radius;
        };

        // Usage

        Circle circle;
        circle.radius = 10.;
        double radius = circle.radius; // value will be 10.
        \endcode

        Initial Property Values
        -----------------------

        Properties can be initialized in the member initialization list of a
       class:

        \code
        class Circle
        {
        public:
            Property<double> radius = 10.;
        };
        \endcode

        Alternatively, a property's value can also be initialzed in the
       constructor of a class:

        \code
        class Circle
        {
        public:
            Circle() : radius(10.) {}

        private:
            Property<double> radius;
        };
        \endcode

        Read-only Access
        ----------------

        If you want to provide users of your class with read access to certain
       properties only, you can declare `Property` private and provide a public
       `const Property` reference:

        \code
        class Circle
        {
        public:
            Circle(double initRadius) : _radius(initRadius) {}

            // Property accessible for reading only from the outside
            const Property<double>& radius = _radius;

        private:
            // Property accessible for reading and writing from inside the
       Circle class Property<double> _radius;
        }
        \endcode

        Sometimes it is infeasible to reference a `Property` instance at compile
       time. In those situations, use the `connect()` method to connect the
       `ReadProperty` instance at runtime. Consider the following example:

        \code
        class Model
        {
        public:
            Property<String> text = "Hello world";
        };

        class ViewModel
        {
        public:
            ViewModel()
            {
                // Instantiate a Model
                _model = newObj<Model>();

                // Connect read-only property text of ViewModel to read-write
                // text property of Model
                text.connect(_model.text);
            }

            // This property should be connected to a Model instance's text
       property to provide
            // read access only in the ViewModel class
            const Property<String> text;

        private:
            P<Model> _model;
        };
        \endcode

        Here, the read-write Property of the `Model` class is not available for
       initialization until the constructor of the `ViewModel` class gets
       called. Hence, the `ViewModel`'s `const Property` member `text` can only
       be connected as soon as the `Model` instance has been created.

        Getters and Setters
        -------------------

        Properties allow for custom getter and/or setter functions. You may
       implement these as outlined in the following example:

        class Person
        {
        public:
            Property<String> name = {
                GetterSetter<String>{
                // Pointer to the instance which provides the getter/setter
                this,
                // Pointer to getter member function
                &Person::getName,
                // Pointer to setter member function
                &person::setName,
                // Pointer to data member storing name's value
                &_name
                }
            };

            String getName() const {
                return _name;
            }

            // Besides setting the property value, setters must return a bool
       indicating whether
            // the property's value has been changed by the set operation
            bool setName(const String& name) {
                if (name != _name) {
                    _name = name;
                    return true; // value did change
                }

                return false; // value did not change
            }

        private:
            String _name;
        };

        In the example above, we provided simple custom implementations for both
       a getter and a setter function.

        It is also legal to provide only a getter with no setter and no member
       pointer. In that case the property is runtime read-only. That is, an
       exception is thrown when the setter is called.

        If no getter is provided, then a member pointer must be given. If
       neither getter nor setter are specified, the Property class will
       substitute both functions with default implementations.

        Data Bindings
        -------------

        A property can be bound to another property so as to synchronize their
       values:

        \code
        class ViewModel
        {
        public:
            Property<String> buttonText = "Hello world!";
        };

        class MainViewController : public Base
        {
        public:
            MainViewController(ViewModel* viewModel)
            {
                // Create the button
                _button = newObj<Button>();

                // Update the button's label when the buttonText property of
       ViewModel changes button.label.bind(viewModle.buttonText);

                // [...]
            }

        private:
            P<Button> _button;
        };
        \endcode

        Change Notifications
        --------------------

        Properties can notify observers when their value changes. Observers can
       register for receiving notifications using the onChange() method.

        `auto` Keyword
        --------------

        Properties support the C++11 `auto` keyword. However, you have to
       dereference a property using the `*` operator when using `auto`:

        \code
        Property<String> nameProperty = "John";
        auto name = nameProperty;  // Error: copy constructor is explicitly
       deleted auto name = *nameProperty; // type of name will be deduced to
       String \endcode

        Alternatively, `get()` can be used when working with `auto`:

        \code
        Property<String> nameProperty = "John";
        auto name = nameProperty.get();
        \endcode

        Non-primitive Value Types
        -------------------------

        Properties support non-primitive value types such as user-defined
       classes and structs. Members can be read using the arrow operator:

        \code
        struct Person
        {
            String name = "John";
            int age = 30;
        };

        Property<Person> person;
        String name = person->name;
        int age = person->age;
        \endcode

        Writing to a member of a non-primitive type inside a property is not
       supported:

        \code
        Property<Person> person;
        person->name = "Jack"; // Error: no viable overloaded =
        \endcode

        If you want to write to a property holding a non-primitive type, use
       `get()` and `set()` to copy and modify the property's value:

        \code
        Property<Person> personProperty;
        Person person = personProperty.get();
        person.name = "Jack";
        personProperty.set(person);
        \endcode

        Pointer Types
        -------------

        Pointer types, including smart pointers, are supported by the `Property`
       class. Consider the following example:

        \code
        class Model : public Base
        {
        public:
            Property<String> labelText;
        };

        class ViewModel : public Base
        {
        public:
            Property<P<Model>> model;
        };

        P<ViewModel> viewModel = newObj<ViewModel>();
        // Do something with the view model
        \endcode

        When using pointer types, `Property`'s arrow operator can be used to
       write to nested properties:

        \code
        P<ViewModel> viewModel = newObj<ViewModel>();
        viewModel->model->labelText = "Hello world!";
        \endcode

        Copying
        -------

        The Property class is not copy-constructible. However, property values
       can be copied by default constructing a `Property` instance and then
       assigning using `operator =`:

        \code
        Property<String> name;
        Property<String> badNameCopy = name; // Error: copy constructor is
       explicitly deleted

        Property<String> goodNameCopy;
        goodNameCopy = name; // OK: value of name is copied to value of
       goodNameCopy by assignment \endcode

        Consequently, structs or classes containing `Property` data members are
       also not copy-constructible:

        \code
        struct Person
        {
            Property<String> name = "John";
        };

        Person person;
        Person person2 = person; // Error: copy constructor of Person is
       implicitly deleted \endcode

        If your struct or class needs to be copy-constructible, you may
       implement a custom copy constructor:

        \code
        struct Person
        {
            Person() = default;
            Person(const Person& other) : Person()
            {
                name = other.name; // OK: assignment after default construction
            }

            Property<String> name = "John";
        };

        Person person;
        Person person2 = person; // OK: copy constructor of name data member is
       never called \endcode


     */
    template <class ValType> class Property : virtual public IValueAccessor<ValType>
    {
      private:
        template <typename> struct int_
        {
            using type = int;
        };

        template <typename T> class overloadsArrowOperator
        {
            template <typename C = T>
            static uint8_t _test(int dummy, decltype(&(*((C *)nullptr)->operator->())) pDummy = nullptr);

            template <typename C = T> static uint16_t _test(...);

            static_assert(sizeof(_test<T>(0)) != 0, "This should never trigger");

          public:
            enum
            {
                value = sizeof(_test<T>(0)) == sizeof(uint8_t) ? 1 : 0
            };
        };

        using backing_t = Backing<ValType>;
        using internal_backing_t = InternalValueBacking<ValType>;
        using gs_backing_t = GetterSetterBacking<ValType>;
        using setter_backing_t = SetterBacking<ValType>;

      public:
        using value_accessor_t_ptr = typename Backing<ValType>::value_accessor_t_ptr;

        Property() : _backing(std::make_shared<internal_backing_t>()) {}
        Property(const Property &) = delete;
        ~Property() override = default;

        /** Constructs a Property instance from a given value */
        Property(ValType value) : _backing(std::make_shared<internal_backing_t>())
        {
            set(value, false /* do not notify on initial set */);
        }

        /** Constructs a Property instance from a GetterSetter object */
        Property(const GetterSetter<ValType> &getterSetter)
        {
            _backing =
                std::make_shared<gs_backing_t>(getterSetter.getter(), getterSetter.setter(), getterSetter.member());
        }

        Property(const Setter<ValType> &setter) { _backing = std::make_shared<setter_backing_t>(setter.setter()); }

        /** Assigns the given value to the property */
        Property &operator=(const ValType &value)
        {
            set(value);
            return *this;
        }

        /** Whether the property's value equals the given value */
        bool operator==(const ValType &value) const { return get() == value; }

        /** Whether the property's string value equals the given C string */
        bool operator==(const char *cString) const { return Property<ValType>::operator==(String(cString)); }

        /** Whether the property's value does not equal the given value */
        bool operator!=(const ValType &value) const { return get() != value; }

        /** Whether the property's string value does not equal the given C
         * string */
        bool operator!=(const char *cString) const { return Property<ValType>::operator!=(String(cString)); }

        /** Returns the property's value – intended for use with the `auto`
         * keyword */
        ValType operator*() const { return get(); }

        /** Returns the property's value – used when casting explicitly or
         * implicitly */
        operator ValType() const { return get(); }

        /** Provides access to members of non-primitive pointer types */
        template <typename U = ValType, typename std::enable_if<overloadsArrowOperator<U>::value, int>::type = 0>
        const ValType operator->() const
        {
            return get();
        }

        /** Provides access to members of non-primitive types */
        template <typename U = ValType, typename std::enable_if<!overloadsArrowOperator<U>::value, int>::type = 0>
        const typename backing_t::Proxy operator->() const
        {
            return _backing->proxy();
        }

        /** Assigns the given property value */
        Property &operator=(const Property &otherProperty)
        {
            if (&otherProperty == this) {
                return *this;
            }

            _backing->set(otherProperty.backing()->get());
            return *this;
        }

        /** Binds to the given property */
        void bind(Property<ValType> &sourceProperty, BindMode bindMode = BindMode::bidirectional)
        {
            _backing->bind(sourceProperty.backing());
            if (bindMode == BindMode::bidirectional) {
                sourceProperty.backing()->bind(_backing);
            }
        }

        /** Returns the INotifier object used for posting change notifications
         */
        auto &onChange() const { return _backing->onChange(); }

        /** Copies and returns the current property value */
        ValType get() const override { return _backing->get(); }

        /** Connects the property's backing to another property's backing */
        const Property &connect(const Property<ValType> &otherProperty) const
        {
            _backing = otherProperty._backing;
            return *this;
        }

        /** Sets the property's value and notifies observers if the value
         * changed */
        void set(ValType value, bool notify = true) { _backing->set(value, notify); }

        /** Returns the property's Backing object */
        const auto backing() const { return _backing; }

        Property operator+() const { return +this->get(); }

        Property operator-() const { return -this->get(); }

        Property operator+(const Property &otherProperty) const { return this->get() + ValType(otherProperty); }

        Property operator-(const Property &otherProperty) const { return this->get() - ValType(otherProperty); }

        Property operator*(const Property &otherProperty) const { return this->get() * ValType(otherProperty); }

        Property operator/(const Property &otherProperty) const { return this->get() / ValType(otherProperty); }

        Property operator%(const Property &otherProperty) const { return this->get() % ValType(otherProperty); }

        Property operator~() const { return ~(this->get()); }

        Property operator&(const Property &otherProperty) const { return this->get() & ValType(otherProperty); }

        Property operator|(const Property &otherProperty) const { return this->get() | ValType(otherProperty); }

        Property operator^(const Property &otherProperty) const { return this->get() ^ ValType(otherProperty); }

        Property operator<<(const Property &otherProperty) const { return this->get() << ValType(otherProperty); }

        Property operator>>(const Property &otherProperty) const { return this->get() >> ValType(otherProperty); }

        ValType operator+(const ValType &other) const { return this->get() + other; }

        ValType operator-(const ValType &other) const { return this->get() - other; }

        ValType operator*(const ValType &other) const { return this->get() * other; }

        ValType operator/(const ValType &other) const { return this->get() / other; }

        ValType operator%(const ValType &other) const { return this->get() % other; }

        ValType operator&(const ValType &other) const { return this->get() & other; }

        ValType operator|(const ValType &other) const { return this->get() | other; }

        ValType operator^(const ValType &other) const { return this->get() ^ other; }

        ValType operator<<(const ValType &other) const { return this->get() << other; }

        ValType operator>>(const ValType &other) const { return this->get() >> other; }

        Property &operator+=(const Property &otherProperty)
        {
            set(this->get() + ValType(otherProperty));
            return *this;
        }

        Property &operator-=(const Property &otherProperty)
        {
            set(this->get() - ValType(otherProperty));
            return *this;
        }

        Property &operator*=(const Property &otherProperty)
        {
            set(this->get() * ValType(otherProperty));
            return *this;
        }

        Property &operator/=(const Property &otherProperty)
        {
            set(this->get() / ValType(otherProperty));
            return *this;
        }

        Property &operator%=(const Property &otherProperty)
        {
            set(this->get() % ValType(otherProperty));
            return *this;
        }

        Property &operator&=(const Property &otherProperty)
        {
            set(this->get() & ValType(otherProperty));
            return *this;
        }

        Property &operator|=(const Property &otherProperty)
        {
            set(this->get() | ValType(otherProperty));
            return *this;
        }

        Property &operator^=(const Property &otherProperty)
        {
            set(this->get() ^ ValType(otherProperty));
            return *this;
        }

        Property &operator<<=(const Property &otherProperty)
        {
            set(this->get() << ValType(otherProperty));
            return *this;
        }

        Property &operator>>=(const Property &otherProperty)
        {
            set(this->get() >> ValType(otherProperty));
            return *this;
        }

      private:
        mutable std::shared_ptr<backing_t> _backing;
    };

    template <typename CHAR_TYPE, class CHAR_TRAITS, typename PROP_VALUE>
    std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &operator<<(std::basic_ostream<CHAR_TYPE, CHAR_TRAITS> &stream,
                                                           const Property<PROP_VALUE> &s)
    {
        // note that if there is no << operator for PROP_VALUE then that is not
        // a problem. Since this << operator is a template all that does is
        // remove the operator from the list of possible overloads. So it would
        // be as if there was no << operator for Property<PROP_VALUE>, which is
        // exactly what we would want in this case.

        return stream << s.get();
    }
}
