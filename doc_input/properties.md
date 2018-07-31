# Properties

## Introduction

Boden supports the notion of "properties". Properties manage the access to a variable of an arbitrary type and add
some useful functionality on top. In some ways Boden properties are very similar to the property concepts of other
programming languages (e.g. C#). However, there are also some notable differences.

Properties support change notifications and data binding. I.e. you can subscribe a function to a property and
the function will be called whenever the property value changes. You can also bind two properties together
so that whenever the value of one property changes, then the other property is automatically set to the same
value.

Properties also enable future extensibility by ensuring that the behaviour can be changed later without
breaking the public interface of the property owner.

In Boden, properties are a very light weight concept that is based on simple member functions. A property is
not an object - it is merely a promise that the property owner provides a certain set of functions.

For example, a property named `exampleProp` with a value of the type ExampleType consists of the following
member functions:

    // a getter function that returns the properties' value
    ExampleType exampleProp() const;

    // a setter function that changes the properties' value
    void setExampleProp( const ExampleType& newValue );

    // a function that returns a notifier object for the property.
    // The notifier will fire whenever the property value changes.
    IPropertyNotifier<ExampleType>& examplePropChanged() const;

    // Last but not least the type of the property must be also be
    // indicated via a typedef
    typedef ExampleType PropertyValueType_exampleProp;


Any class that implements these functions and provides the typedef can be said to have the `exampleProp` property.

Now it would be very tedious to write these functions oneself, so Boden provides a set of macros that take
care of all the internals.

Let's look at an example:

    class Person
    {
    public:
        BDN_PROPERTY( double, height, setHeight );
    };

This example declares a class Person with property called `height`. The `height` property has a value of type
double. Its setter function is called setHeight. Note that the name of the setter function must be passed explicitly
to the BDN_PROPERTY macro.

Accessing a property is very simple:

    Person somePerson = ...;

    double h = somePerson.height(); // returns the property value

    somePerson.setHeight( 182 );    // changes the height.

This example shows one important aspect of Boden properties: they are used like normal functions. While that might not
seem as "fancy" as a complex Property class with magic operators and automatic conversions, it has a lot of advantages:

- Boden properties are highly efficient. When the change notifier (and data binding) are not used with a given property
  then there is almost no additional overhead compared to traditional getter/setter functions.
- Converting a normal getter function to a property can often be done without breaking backwards compatibility. So you can
  add support for change notifications and data binding later.
- Since properties consist of normal functions, the C++ overloading semantics apply to them. You can declare
  abstract properties in your interfaces and provide the implementation in a derived class. You can also override
  properties from base classes.
- Overloading properties and implementing abstract properties have the same compiler support for warnings and compile
  time errors that normal virtual functions have. So if you declare an abstract property and do not provide an implementation
  then you will get a compile time error. If you "accidentally" override/overshadow a property from a base class then you will get a warning.
  And so on.
- When needed, the property implementation can be customized completely. Using the macros is optional (although recommended).
  If heavy customization is needed then the three functions and the typedef can be implemented manually and the resulting
  property will still work normally. It can also be bound to other properties that use the macros.


## Property binding

Properties can be "bound" to other properties. Afterwards, whenever the other property changes then the bound property is automatically
set to the same value.

Note that the two properties can have different names, but for a simple binding they must have the same value types. Properties with
different values can also be bound together, but that is a little bit more complicated (see section of filtered binding below).

Example:

    class Person
    {
    public:
        BDN_PROPERTY( double, height, setHeight );
    };

    // In this example, "HeightView" is a class that visualizes the height in some way.
    // This could also be any other class that has a property of type double.
    class HeightView
    {
    public:

        BDN_PROPERTY( double, value, setValue );        
    };


    // In this example we skip over the initialization of the objects, as this does not matter.
    Person      somePerson = ...;
    HeightView  view = ...;

    // We now bind the "value" property of the HeightView to the "heigth" property of the person object.
    BDN_BIND_PROPERTY_TO( view, value, somePerson, height );

    // Now we change the height of the person object. This will
    // automatically also update the bound property "value" in the HeightView.
    somePerson.setHeight( 123 );

    // value should now also be 123
    assert( view.value() == 123 );


### Bidirectional bindings

Note that the binding only works in one direction. I.e. when we change the bound property then the original property
is **not** updated automatically. However, that can also easily be achieved by binding in both directions:

    class Person... // see above
    class HeightView... // see above

    Person      somePerson = ...;
    HeightView  view = ...;

    // we now bind in both directions.
    BDN_BIND_PROPERTY_TO( view, value, somePerson, height );
    BDN_BIND_PROPERTY_TO( somePerson, height, view, value );

    // now we can modify any of the two properties and the other is automatically updated
    
    view.setValue( 456 ); // this also sets somePerson.height
    assert( somePerson.height() == 456 );

    somePerson.setHeight( 789 );    // this also sets view.value
    assert( view.value() == 789 );



### Bindings with different but compatible types

The property binding system also supports automatic type conversions for data bindings.
You can bind two properties with different value types together, as long as there is an implicit conversion
defined.

For example, you can bind a property of type `double` to a property of type `int` (since ints can be implicitly converted to `double`).
Note that the other way around does not work (at least not without the compiler issuing a warning), since data might be lost
when you assign a `double` value to an integer value.

Example:

    // here we do the same thing as in the examples above, except that the Person
    // class uses integer and the HeightView property uses doubles.

    class Person
    {
    public:
        // an integer property: the height in centimeters
        BDN_PROPERTY( int, heightCm, setHeightCm );
    };

    
    class HeightView
    {
    public:

        // HeightView still accepts doubles
        BDN_PROPERTY( double, value, setValue );        
    };


    Person      somePerson = ...;
    HeightView  view = ...;

    // We can still bind the heightCm value to the HeightView, even though
    // one is an integer and the other a double.
    BDN_BIND_PROPERTY_TO( view, value, somePerson, heightCm );

    


### Filtered bindings

When one property is bound to another in the normal way, then both must have the same type or a compatible type.
And afterwards both properties end up with the exact same value.

However, Boden also supports property binding with a filter. These can be used to to modify the value that the bound property
is set to. You can even use them to bind two properties of completely different types together.

Let's look at an example:

    // some class that represents a background operation  of some kind
    class MyOperation
    {
    public:

        // a property that indicates the progress of the operation in percent
        BDN_PROPERTY( double, progress, setProgress )
    };


    // Some view class that shows the current status
    class StatusView
    {
    public:

        // a property with the current status text
        BDN_PROPERTY( String, statusText, setStatusText );
    };

    // For this example, we assume that we got the two objects from somewhere.
    // It does not matter where they cam from.
    MyOperation op = ...;
    StatusView  view = ...;

    // Bind the status text view to the progress property.
    // Note that statusText is of type String and progress of type double.
    // So we need to provide a filter function. Here we use a lambda function,
    // but any type of function is possible.
    auto filterFunc = [](double newProgressValue) {
        // generate a status text string from the double value
        return std::to_string(newProgressValue) + " % completed";
    };
    BDN_BIND_PROPERTY_TO_WITH_FILTER( view, statusText, MyOperation, progress, filterFunc );

    // Now when we modify the progress value the status text is automatically
    // set to the output value of our filter function
    op.setProgress( 42 );
    assert( view.statusText() == "42 % completed" );


The example shows how one can connect two properties together with a simple filter function in between.
Apart from the filtering taking place, these property bindings behave exactly the same as all other property
bindings.


### Reference counting and destruction behaviour

Property bindings do not cause the reference count of the owner to be increased. I.e. the binding will not keep
any of the two objects alive.

So that begs the question what happens when one of the two sides of the binding is destroyed? The binding will
be silently invalidated and removed. This happens completely automatically, so you do not need to be concerned
about this case.

### Requirements for owner objects

When property binding is used, then the owners of both properties must support Boden's weak pointer system.

That means that:

- both owner objects must implement IBase (or be derived from bdn::Base)
- both owner objects must be allocated with bdn::newObj()

Note that these restrictions are necessary to ensure that the property bindings can be safely invalidated when
one of the two objects is destroyed.


## Change notifications

Apart from data binding, properties also support simple change notifications. Each property has an associated
`xyzChanged` method (see introduction) that returns a reference to a notifier object.

You can subscribe any function to this notifier object. The function will be called automatically
whenever the property changes.

See `bdn::IPropertyNotifier` for more information.

### Property modifications DURING change notifications

A corner case is what happens when a property is modified while the change notification calls for an earlier
modification are still in progress. This can happen when one subscribed function does something that updates the property value.

This case is fully supported. If the property value changes during the notification process then all the notification
calls happen as expected. Each subscribed function will get called with the up-to-date value of the property at that
time.

## Read-only properties and access control

There are often cases when one wants to expose a value as read-only. The property system supports this with a special
macro:

    class SomeClass
    {
    public:

        BDN_PROPERTY_WITH_CUSTOM_ACCESS( String, public, myText, private, setMyText );
    };

This example creates a property `myText` whose setter is private. I.e. only the owner class itself can modify
the property -- everyone else can only read it.

Note that the macro uses the normal C++ access specifiers: public, protected and private. They work the same way.
So protected could also have been used to declare the setter as non-public, but also allow derived classes to access it.


## Multi threading

Properties do not support multi-threaded access. You should use them from one thread only.



