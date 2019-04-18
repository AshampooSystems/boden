# Wrapping a Java class

Wrapping a Java class in Boden is done using the classes in ```bdn::java::wrapper```.
There are two classes, `Object` and `JTObject`. `JTObject` is a convenience class that uses templates to make creating new wrapper simpler. 

## First Steps

In this example we show the simplest way to wrap a java class:

```C++
constexpr const char kMyJavaClassName[] = "io/mycompany/MyJavaClass";
using MyJavaClass = bdn::java::wrapper::JTObject<kMyJavaClassName>;
```

With these definitions you can now instantiate a Java class from C++ in your code:

```C++
MyJavaClass myClass;
```

This will automatically instantiate the Java class for you.

## Wrapping Java Class Methods

To be able to add methods to your wrapper class, add the following to the previous example code:

```C++
constexpr const char kMyJavaClassName[] = "io/mycompany/MyJavaClass";
class MyJavaClass : public bdn::java::wrapper::JTObject<kMyJavaClassName>
{
public:
	using bdn::java::wrapper::JTObject<kMyJavaClassName>::JTObject;
};
```

Now that you have your class ready, you can start to add methods to it. Boden provides the convenience class `JavaMethod` to simplify the process.

```C++ hl_lines="7"
constexpr const char kMyJavaClassName[] = "io/mycompany/MyJavaClass";
class MyJavaClass : public bdn::java::wrapper::JTObject<kMyJavaClassName>
{
public:
	using bdn::java::wrapper::JTObject<kMyJavaClassName>::JTObject;
public:
	bdn::JavaMethod<void(bdn::JavaString)> testFunction{this, "testFunction"};
};
```

JavaMethod itself is a template that takes a function pointer style argument. It needs to be initialized with a pointer to the object (```this```) and the name of the function has be be provided as a string (in this case `testFunction`).

Now you can call the function:

```C++
MyJavaClass myClass;
myClass.testFunction("Hello World");
```

## Deriving from JTObject Base Classes

To derive from a class, you pass through the new class name as a template argument. Building on the previous example you change `MyJavaClass` to the following (changes are highlighted below):

```C++ hl_lines="2 3 6 10"
constexpr const char kMyJavaClassName[] = "io/mycompany/MyJavaClass";
template<const char* javaClassName = kMyJavaClassName>
class BaseMyJavaClass : public bdn::java::wrapper::JTObject<javaClassName>
{
public:
	using bdn::java::wrapper::JTObject<javaClassName>::JTObject;
public:
	bdn::JavaMethod<void(bdn::JavaString)> testFunction{this, "testFunction"};
};
using MyJavaClass = BaseMyJavaClass<>;
```

In the first highlighted line you change MyJavaClass to be a template class taking a ```const char*``` so you can override the Java class name. For convenience, we set the original name as a default argument.

In the second line you change the class name to ```BaseMyJavaClass```. This again is for convenience. In the end we create a ```using``` declaration so our code can continue to use ```MyJavaClass``` as a class name.

Now, just as before, you can create a new class that derives from `BaseMyJavaClass`:

```C++
constexpr const char kDerivedJavaClassName[] = "io/mycompany/DervivedJavaClass";
class DerivedJavaClass : public BaseMyJavaClass<kDerivedJavaClassName>
{
public:
	using BaseMyJavaClass<kDerivedJavaClassName>::BaseMyJavaClass;
};
```

You can now instantiate the derived class. You do not need to redefine methods in the derived class if your Java class overloads them. They will work automatically as long as they are defined in the base class.

```C++
DerivedJavaClass object;
object.testFunction("Hello World");
```
