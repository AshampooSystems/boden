# Properties

## Intro

Properties are intended to be exposed as public data members of a class:

```c++
using namespace bdn;

class Circle
{
public:
    Property<double> radius;
};

// Usage

Circle circle;
circle.radius = 10.;
double radius = circle.radius; // value will be 10.
```

## Initial Property Values

Properties can be initialized in the member initialization list of a class:

```c++
using namespace bdn;

class Circle
{
public:
    Property<double> radius = 10.;
};
```

Alternatively, a property's value can also be initialzed in the constructor of a class:

```c++
using namespace bdn;

class Circle
{
public:
    Circle() : radius(10.) {}

private:
    Property<double> radius;
};
```

## Read-only Access

If you want to provide users of your class with read access to certain properties only,
you can declare `Property` private and provide a public `const Property` reference:

```c++
using namespace bdn;

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
```

## Getters and Setters

Properties allow for custom getter and/or setter functions. You may
implement these as outlined in the following example:

```c++
using namespace bdn;

class Person
{
public:
    Property<std::string> name = {
        GetterSetter<std::string>{
        // Pointer to the instance which provides the getter/setter
        this,
        // Pointer to getter member function
        &Person::name,
        // Pointer to setter member function
        &person::setName,
        // Pointer to data member storing name's value
        &_name
        }
    };

    std::string name() const {
        return _name;
    }

    // Besides setting the property value, setters must return 
    // a bool indicating whether the property's value has been 
    // changed by the set operation
    bool setName(const std::string& name) {
        if (name != _name) {
            _name = name;
            return true; // value did change
        }

        return false; // value did not change
    }

private:
    std::string _name;
};
```

In the example above, we provided simple custom implementations for both
a getter and a setter function.

It is also legal to provide only a getter with no setter and no member
pointer. In that case the property is runtime read-only. That is, an
exception is thrown when the setter is called.

If no getter is provided, then a member pointer must be given. If
neither getter nor setter are specified, the Property class will
substitute both functions with default implementations.

## Data Bindings

A property can be bound to another property so as to synchronize their
values:

```c++
using namespace bdn;
using namespace bdn::ui;

class ViewModel
{
public:
    Property<std::string> buttonText = "Hello world!";
};

class MainViewController : public Base
{
public:
    MainViewController(ViewModel* viewModel)
    {
        // Create the button
        _button = std::make_shared<Button>();

        // Update the button's label when the buttonText property of
		// ViewModel changes 
        button.label.bind(viewModle.buttonText);

        // [...]
    }

private:
    std::shared_ptr<Button> _button;
};
```

## Change Notifications

Properties can notify observers when their value changes. Observers can
register for receiving notifications using the onChange() method.

## `auto` Keyword

Properties support the C++11 `auto` keyword. However, you have to
dereference a property using the `*` operator when using `auto`:

```c++
using namespace bdn;

Property<std::string> nameProperty = "John";
auto name = nameProperty;  // Error: copy constructor is explicitly deleted 
auto name = *nameProperty; // type of name will be deduced to std::string 
```

Alternatively, `get()` can be used when working with `auto`:

```c++
using namespace bdn;

Property<std::string> nameProperty = "John";
auto name = nameProperty.get();
```

## Non-Primitive Value Types

Properties support non-primitive value types such as user-defined
classes and structs. Members can be read using the arrow operator:

```c++
using namespace bdn;

struct Person
{
    std::string name = "John";
    int age = 30;
};

Property<Person> person;
std::string name = person->name;
int age = person->age;
```

Writing to a member of a non-primitive type inside a property is not
supported:

```c++
using namespace bdn;

Property<Person> person;
person->name = "Jack"; // Error: no viable overloaded =
```

If you want to write to a property holding a non-primitive type, use
`get()` and `set()` to copy and modify the property's value:

```c++
using namespace bdn;

Property<Person> personProperty;
Person person = personProperty.get();
person.name = "Jack";
personProperty.set(person);
```

## Pointer Types

Pointer types, including smart pointers, are supported by the `Property`
class. Consider the following example:

```c++
using namespace bdn;

class Model : public Base
{
public:
    Property<std::string> labelText;
};

class ViewModel : public Base
{
public:
    Property<std::shared_ptr<Model>> model;
};

std::shared_ptr<ViewModel> viewModel = std::make_shared<ViewModel>();
// Do something with the view model
```

When using pointer types, `Property`'s arrow operator can be used to
write to nested properties:

```c++
std::shared_ptr<ViewModel> viewModel = std::make_shared<ViewModel>();
viewModel->model->labelText = "Hello world!";
```

## Copying

The Property class is not copy constructible. However, property values
can be copied by default constructing a `Property` instance and then
assigning using `operator =`:

```c++
using namespace bdn;

Property<std::string> name;
Property<std::string> badNameCopy = name; // Error: copy constructor 
									 // is explicitly deleted

Property<std::string> goodNameCopy;
goodNameCopy = name; // OK: value of name is copied to value 
					 // of goodNameCopy by assignment
```

Consequently, structs or classes containing `Property` data members are
also not copy constructible:

```c++
using namespace bdn;

struct Person
{
    Property<std::string> name = "John";
};

Person person;
Person person2 = person; // Error: copy constructor of Person
						 // is implicitly deleted 
```

If your struct or class needs to be copy constructible, you may
implement a custom copy constructor:

```c++
using namespace bdn;

struct Person
{
    Person() = default;
    Person(const Person& other) : Person()
    {
        name = other.name; // OK: assignment after default construction
    }

    Property<std::string> name = "John";
};

Person person;
Person person2 = person; // OK: copy constructor of name 
						 // data member is never called 
```

## Reference

[Property reference](../../reference/foundation/property.md)
