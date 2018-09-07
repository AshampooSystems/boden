Properties: Internals and design decisions {#property_internals}
===============================================


This document describes the inner workings and design decisions of Boden's property system.

Goals
-----

The property system should support:

- properties should support binding - i.e. it should be possible to connect two properties so that
  changes to one automatically modify the other. Binding can be useful in many circumstances,
  the prime example being connecting view models to views.
- abstract properties, i.e. properties for abstract interface classes. These
  should be safe to use, i.e. we should get a compiler error if we forget to provide an implementation
  for the abstract property (similar to how virtual functions work).
- properties that store their value internally
- properties that execute custom getter and setter methods
- changing the behaviour of properties inherited from a base class (for example, replacing getter and/or setter)
- inheriting from a base class with a property X and then adding additional interface classes which also provide
  the same property
- accessing the property should feel "natural"
- the property system should be easy to learn and understand


Design considerations
---------------------

### Abstract properties

Since we want to support abstract properties in a convenient way, we need some support from the language / compiler.
The abstract properties should be safe to use, i.e. it should be impossible to forget to provide an implementation
for an abstract property. If one forgets the implementation (or forgets to "connect" it to the abstract property)
then we want to have a compile time error.

We want a compile time error because otherwise the properties would be prone to errors. If somehow no implementation
is "connected" to the abstract property and we do not get a compile error, then accessing it
would have to cause either a crash or an exception. The unfortunate thing is that the implementation for the property might 
actually exist in the derived class, but it was forgotten to connect the property implementation with the abstract property.
So that means that accessing the property via a reference to the actual object's class would work. But accessing
it via a reference to the interface would cause an exception. So one would
have to write unit tests that access every property of every interface, to check if the connection to
the actual property is correct. That is not reasonable.

Luckily, C++ already has a concept that creates connections like this automatically: virtual functions.
If we have two interfaces with the same abstract methods, then we can simply implement that method in a derived
class and the compiler automatically "connects" the abstract end points of the method to the implementation.
We want to use that fact to get the same behaviour for abstract properties.


### Overriding base class properties

We also want a way to override the behaviour of base class properties. We could use the same virtual method
mechanism that we used for abstract properties. There are also some other options.
 
However, there is another aspect to consider: the base class may have a constructor. And the constructor may
access the property. If we override the base class property then that override will take effect only after
the base class constructor is executed. So when the base class constructor accesses the property then it will see
its own base class version, before the behaviour is overridden. That has two consequences:

- When overriding we have to make sure that we copy the value of the base property.
- We also have to make sure that change listeners and property bindings are copied or moved over as well.

One way to achieve that would be to simply only have one place where listeners and bindings are registered.
That means that we would have to prevent derived classes from redeclaring these registries when they override
the behaviour of the property.


### Problems with inheritance and abstract properties

We want the property macros that the developer has to use later to be as easy and simple to understand as
possible. As such, we should minimize the number of macro variants that the user needs.

However, there are several challenges to that:

- Abstract properties definitely need a separate macro, since abstract properties have to be different in some
  way from "normal" properties (since they need to generate a compiler error when there is no implementation for them).

- The initial property implementation needs to be able to connect to abstract properties inherited from an interface
  or base class. It also needs to be able to work without a warning when there are no inherited abstract properties.

  Since we use virtual functions for abstract properties, we need to somehow indicate if the "override"
  function specifier should be set for the property implementation functions.

- When we want to redefine the behaviour of a base class property (for example, adding a getter/setter)
  then we also need a separate macro.
  That is because we need to ensure that listeners attached to the base class property are kept and that
  the value from the base class property is copied.
  If the listener registry is separate from the property itself, then we need to ensure that
  we do not overshadow it with a new registry instance. If it is built into the property then we must add code
  that copies the base class listeners to the new registry.

- We also need a way to handle the case when a class adds a new interface that has a property
  that a base class already has an implementation for. In that case the derived class will inherit two properties
  with the same name and the compiler will not know which one to use.
  So the virtual function from the interface must be redeclared to disambiguate between the two versions.

- Last but not least we need a way to change the behaviour of an inherited property (for example,
  by setting a new getter and/or setter).

So, the property system must support the following cases:

1. abstract properties
2. a simple, new property that was not inherited from a base class
3. a property that implements an abstract property
4. some way to redefine the behaviour of a base class property
5. some way to disambiguating a newly inherited abstract property for which there already is an
   implementation
6. some way to disambiguate a newly inherited abstract property and at the same time redefining
   the behaviour of the base property.
7. All cases that provide a property implementation must also have a way to optionally configure a custom getter/setter method


Thoughts about the visible interface for properties
---------------------------------------------------

### Properties as objects

One possible way to implement properties (in fact, one that is used by many frameworks), is to have a property object of some kind.
For example, if a class Address would have a property called "name" then myAddress.name would refer to that property.

This approach has the advantage that the property can be used in place of a normal value. The object could support implicit conversion
to the property value, assignment etc.

However, the approach also has disavantages. While the property might work like a reference to its internal value in some cases,
in others it does not. For example, one cannot use the . operator to access fields of the value, for example (since the dot operator
cannot be overloaded). Each read access to the property would also have to call the internal getter (if one is configured), so
accessing the property multiple times in quick succession can be costly performance-wise. The developer might not expect that,
since it seems like a simple read access to a value.

There is also a drawback extensibility. Lets say a text class has has a method "length()" that returns the length of the text.
Initially there is no support for binding or change listeners, it is simply a normal getter that returns some internal value.
Then lets assume that we want to extend that and provide support for property binding. If we do not want to break backwards
compatibility then we would have to add a new property with a different name and keep the old length() function anyway.
So we end up with a new property that has a sub-optimal name, plus an old function that does the same as accessing the property.

There is also a disadvantage complexity disadvantage. Since the property needs to be able to call methods of its owner class
(getters, setters), it has to have a reference to its owner. That makes the property object quite complex and requires several levels
of indirection to handle the parent reference and to forwards calls to it. While careful optimization can reduce the overhead,
it is still a complex construct.

The property object also has the disadvantage that it is quite hard to customize because of the complexity. Replacing the default
implementation completely with something different requires a high level of understanding of the underlying complexities (so as
to not create circular references, etc.).

### The Boden way: properties as a set of methods, not an object

For Boden we decided to not use property objects. We wanted something that is a little simpler and more lightweight in the
common case. And we wanted a solution that enables a clear path to "upgrading" a simple getter to a full property, without breaking
backwards compatibility.

In Boden properties consist of a set of four virtual functions that are added directly to the properties owner object:

- a getter method that has the same name as the property (i.e. "length()" for a property named length)
- a setter method with an arbitrary name. By convention the name is "set" followed by the property name in camel case
  (e.g. setLength).
- a method that returns a notifier object for "changed" events (named lengthChanged() in our example)
- a method that returns a special reference object that can represent the property on its own when needed (called lengthRef() )

Since the property functions are just normal virtual functions, abstract properties are also normal pure virtual functions.
The BDN_PROPERTY macro and its variants and helper macros are simply a convenient way to generate these functions with a single statement.

Each of these functions can be overridden and customized, if needed. If one wants to customize heavily, one can also simply write them
oneself from scratch, without using any macros.

The most common case is a property without custom getter or setter functionality that simply returns and sets an internal value. This
case simply generates trivial methods - similar to what a developer would write by hand in those simple cases. So the Boden properties
are very light weight, with minimal overhead.

A very important feature of the property system is the "upgradability" that we discussed above. If a class starts out with just
a "length()"" method, for example, and one wants to extend it to a full property later then that is no problem.
The old length() can simply be replaced with the property, which will provide a compatible new method with the same name.

Of course this only works if the Boden naming conventions for getters are followed - otherwise the generated getter of the new
property will end up having a different name than the old method.

Another advantage is that the code that uses the property will be easy to understand for any C++ programmer. Since property accesses
are simple getter/setter calls that have been used by programmers since the beginning of time, everyone will understand what the code
does. With a property object one would have to understand the property system first to understand the code that uses the properties.

The problems that can occur when property objects have structs or class instances as their value were already discussed in the
previous section. Basically the property objects behave like a reference most of the time, but when one accesses a field of their
value then one cannot use the dot operator (instead the -> operator has to be used). This type of thing does not happen with the
Boden property system, since ther getter method simply returns the internal value directly, without any wrappers around it.

Another advantage is that the property owner can remain implcitly copiable (something that one has to forbid because of technical
implications if one uses a property object).

And last but not least the simple method based system makes it easier to generate interface code for other programming languages
(for example with SWIG). Since all object oriented languages support normal methods, there is no problem to generate an interface
for them. Property objects on the other hand would probably require custom handling and mapping code for most target languages.











