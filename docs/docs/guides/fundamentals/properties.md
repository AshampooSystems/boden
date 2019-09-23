# Properties

The Boden Framework provides the [`Property`](../../reference/foundation/property.md) class to make working with data displayed on the user interface of an application easier.

A property represents a public value of arbitrary type that can be observed by [notification listeners](#change-notifications) and bound to other properties using a built-in [data binding mechanism](#data-bindings). This is useful especially for user interface code as data is commonly shared between multiple widgets and other class instances like view models.

Properties also make it easy to expose a [read-only](#read-only-access) version of a data member publicly and keep write access private. They can be extended with [getter and setter functions](#getters-and-setters) transparently and they come with support for the [`auto` keyword](#auto-keyword) and `std::optional`.

## Exposing Properties

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

## Initializing Property Values

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

## Data Bindings

Two property values can easily be synchronized by setting up a binding:

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
        button->label.bind(viewModel->buttonText);

        // [...]
    }

private:
    std::shared_ptr<Button> _button;
};
```

The `bind()` method sets up a bidirectional (or two-way) data binding by default. That is, regardless of whether you change the value of `viewModel->buttonText` or `button->label`, the other property will automatically be updated to the new value.

You can also set up a unidirectional (or one-way) data binding:

```c++
button->label.bind(viewModel->buttonText, BindMode::unidirectional);
```

In this case, `viewModel->buttonText` will be updated when changing `button->label`, but not the other way around.

!!! note
    Data bindings update the values of bound properties synchronously. You should use `bind()` only when working with properties on the same thread. See [Thread-Safety](#thread-safety) for more information.

## Change Notifications

Properties can notify observers when their value changes. Observers can register for receiving notifications using the `onChange()` method:

```c++
Property<std::string> name;
name.onChange() += [](auto property) {
    std::cout << property.get() << std::endl;
}
```

`onChange()` returns a [`Notifier`](../../reference/foundation/notifier.md) object which can be used to subscribe or unsubscribe notification handler functions.

## Read-Only Access

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
    // Property accessible for reading and writing from inside the Circle class
    Property<double> _radius;
}
```

## Getters and Setters

You can implement custom getter and/or setter functions and `Property` will call them when its value is updated or retrieved:

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

While the example above contains both a getter and a setter, it is also OK to provide only a getter with no setter and no member pointer. In that case the property is runtime read-only and an exception is thrown when you try to call its setter.

If no getter is provided, a member pointer must be given. If neither getter nor setter are specified, `Property` will automatically substitute both functions with a default implementation.

!!! note
    Using `Property` makes it easier for you to refactor your application when you need to use custom getter or setter logic instead of the default implementation. You can add a getter and/or setter to your `Property` instance without having to change the code using your property.

    Please also note that getters return a copy of the property's data. Hence, you cannot write to non-primitive value types returned from a getter. See [Non-Primitive Value Types](#non-primitive-value-types) for details.

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

Properties support non-primitive value types such as user-defined classes and structs. Members can be read using the arrow operator:

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

Writing to a member of a non-primitive type inside a property is not supported since property getters return copies of the property's value:

```c++
using namespace bdn;

Property<Person> person;
person->name = "Jack"; // Error: no viable overloaded =
```

If you want to write to a property holding a non-primitive type, use `get()` and `set()` to copy and modify the property's value:

```c++
using namespace bdn;

Property<Person> personProperty;
Person person = personProperty.get();
person.name = "Jack";
personProperty.set(person);
```

## Pointer Types

Pointer types, including smart pointers, are supported by the `Property` class. Consider the following example:

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

`Property` is not copy constructible, but property values can be copied by default constructing a `Property` instance and then assigning another property's value using `operator =`:

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

## Thread-Safety

`Property` is currently not thread-safe.

If you want to work with properties from multiple threads, make a copy of the respective property object's data and then use that copy on your worker thread. Do not read a single `Property` instance's value among different threads as this may cause undefined behavior.

If you need to write to a property instance from more than one thread, use [`DispatchQueue`](../../reference/foundation/dispatch_queue.md) to queue a property value change on the thread the property is being read on. Writing to a single property instance from multiple threads simultaneously may result in undefined behavior.

Do not use `bind()` to set up a data binding with properties that are accessed from different threads. Doing so may also result in undefined behavior.

!!! note
    If you find that you need to frequently read/write to properties from multiple threads, consider using a plain data member or `std::shared_ptr` instead of `Property`. Also, you might want to rearchitect your design to not use multiple threads operating directly on the user interface as this can lead to very complex code without a real benefit.

    As a rule of thumb, implement all UI logic on one thread and offload CPU intensive work to worker threads operating on data only. Copy data instead of sharing it between the UI thread and workers. Use [`DispatchQueue`](../../reference/foundation/dispatch_queue.md) to update the UI. Keep your design simple.

## Further Reading

To see the use of `Property` in a real application example, please refer to our [TodoMVC Tutorial on Medium](https://medium.com/ashampoo-systems/getting-started-with-c-17-mobile-cross-platform-development-using-boden-b47cb0f9a9a1).

## Reference

[Property Reference](../../reference/foundation/property.md)
